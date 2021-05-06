//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// p0_starter.h
//
// Identification: src/include/primer/p0_starter.h
//
// Copyright (c) 2015-2020, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <utility>

#include "common/logger.h"

namespace bustub {

/*
 * The base class defining a Matrix
 */
template <typename T>
class Matrix {
 protected:
  // NOTE(P0): Add implementation
  Matrix(int r, int c) : rows(r), cols(c), linear(new T[r * c]) {
    for (int i = 0; i < r * c; i++) {
      linear[i] = 0;
    }
  }

  // # of rows in the matrix
  int rows;
  // # of Columns in the matrix
  int cols;
  // Flattened array containing the elements of the matrix
  // NOTE(P0) : Allocate the array in the constructor. Don't forget to free up
  // the array in the destructor.
  T *linear;

 public:
  // Return the # of rows in the matrix
  virtual int GetRows() = 0;

  // Return the # of columns in the matrix
  virtual int GetColumns() = 0;

  // Return the (i,j)th  matrix element
  virtual T GetElem(int i, int j) = 0;

  // Sets the (i,j)th  matrix element to val
  virtual void SetElem(int i, int j, T val) = 0;

  // Sets the matrix elements based on the array arr
  virtual void MatImport(T *arr) = 0;

  // NOTE(P0): Add implementation
  virtual ~Matrix() { delete[] linear; }
};

template <typename T>
class RowMatrix : public Matrix<T> {
 public:
  // NOTE(P0): Add implementation
  RowMatrix(int r, int c) : Matrix<T>(r, c) {
    data_ = new int *[r];
    for (int i = 0; i < r; i++) {
      data_[i] = new int[c];
      for (int j = 0; j < c; j++) {
        data_[i][j] = 0;
      }
    }
  }

  // NOTE(P0): Add implementation
  int GetRows() override { return this->rows; }

  // NOTE(P0): Add implementation
  int GetColumns() override { return this->cols; }

  // NOTE(P0): Add implementation
  T GetElem(int i, int j) override {
    if (i < 0 || i >= GetRows() || j < 0 || j >= GetColumns()) {
      LOG_ERROR("Invalid access: row %d column %d", i, j);
      return -1;
    }
    return data_[i][j];
  }

  // NOTE(P0): Add implementation
  void SetElem(int i, int j, T val) override {
    if (i < 0 || i >= GetRows() || j < 0 || j >= GetColumns()) {
      LOG_ERROR("Invalid access: row %d column %d", i, j);
      return;
    }
    data_[i][j] = val;
    this->linear[i * GetColumns() + j] = val;
  }

  // NOTE(P0): Add implementation
  void MatImport(T *arr) override {
    for (int i = 0; i < GetRows(); i++) {
      for (int j = 0; j < GetColumns(); j++) {
        int position = i * GetColumns() + j;
        data_[i][j] = arr[position];
        this->linear[position] = arr[position];
      }
    }
  }

  // NOTE(P0): Add implementation
  ~RowMatrix() override {
    for (int i = 0; i < GetRows(); i++) {
      delete[] data_[i];
    }
    delete[] data_;
  }

 private:
  // 2D array containing the elements of the matrix in row-major format
  // NOTE(P0): Allocate the array of row pointers in the constructor. Use these pointers
  // to point to corresponding elements of the 'linear' array.
  // Don't forget to free up the array in the destructor.
  T **data_;
};

template <typename T>
class RowMatrixOperations {
 public:
  // Compute (mat1 + mat2) and return the result.
  // Return nullptr if dimensions mismatch for input matrices.
  static std::unique_ptr<RowMatrix<T>> AddMatrices(std::unique_ptr<RowMatrix<T>> mat1,
                                                   std::unique_ptr<RowMatrix<T>> mat2) {
    if (mat1->GetRows() != mat2->GetRows() || mat1->GetColumns() != mat2->GetColumns()) {
      LOG_ERROR("Bad request: Mat1[%d*%d], Mat2[%d*%d]", mat1->GetRows(), mat1->GetColumns(), mat2->GetRows(),
                mat2->GetColumns());
      return std::unique_ptr<RowMatrix<T>>(nullptr);
    }
    std::unique_ptr<RowMatrix<T>> mat(new RowMatrix<T>(mat1->GetRows(), mat1->GetColumns()));
    for (int i = 0; i < mat->GetRows(); i++) {
      for (int j = 0; j < mat->GetColumns(); j++) {
        mat->SetElem(i, j, mat1->GetElem(i, j) + mat2->GetElem(i, j));
      }
    }
    return mat;
  }

  // Compute matrix multiplication (mat1 * mat2) and return the result.
  // Return nullptr if dimensions mismatch for input matrices.
  static std::unique_ptr<RowMatrix<T>> MultiplyMatrices(std::unique_ptr<RowMatrix<T>> mat1,
                                                        std::unique_ptr<RowMatrix<T>> mat2) {
    if (mat1->GetColumns() != mat2->GetRows()) {
      LOG_ERROR("Bad Request: Mat1[%d*%d] * Mat2[%d*%d]", mat1->GetRows(), mat1->GetColumns(), mat2->GetRows(),
                mat2->GetColumns());
      return std::unique_ptr<RowMatrix<T>>(nullptr);
    }
    std::unique_ptr<RowMatrix<T>> mat(new RowMatrix<T>(mat1->GetRows(), mat2->GetColumns()));
    for (int i = 0; i < mat1->GetRows(); i++) {
      for (int j = 0; j < mat2->GetColumns(); j++) {
        for (int k = 0; k < mat1->GetColumns(); k++) {
          mat->SetElem(i, j, mat->GetElem(i, j) + mat1->GetElem(i, k) * mat2->GetElem(k, j));
        }
      }
    }
    return mat;
  }

  // Simplified GEMM (general matrix multiply) operation
  // Compute (matA * matB + matC). Return nullptr if dimensions mismatch for input matrices
  static std::unique_ptr<RowMatrix<T>> GemmMatrices(std::unique_ptr<RowMatrix<T>> matA,
                                                    std::unique_ptr<RowMatrix<T>> matB,
                                                    std::unique_ptr<RowMatrix<T>> matC) {
    std::unique_ptr<RowMatrix<T>> multi_res(std::move(MultiplyMatrices(std::move(matA), std::move(matB))));
    if (multi_res == nullptr) {
      LOG_ERROR("Multiply result is NULL");
      return multi_res;
    }
    std::unique_ptr<RowMatrix<T>> add_res(std::move(AddMatrices(std::move(multi_res), std::move(matC))));
    if (add_res == nullptr) {
      LOG_ERROR("Add result is NULL");
      return add_res;
    }
    return add_res;
  }
};
}  // namespace bustub
