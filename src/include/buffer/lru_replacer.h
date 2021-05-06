//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.h
//
// Identification: src/include/buffer/lru_replacer.h
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#pragma once

#include <list>
#include <memory>
#include <mutex>  // NOLINT
#include <unordered_map>
#include <vector>

#include "buffer/replacer.h"
#include "common/config.h"

namespace bustub {

// Node is the element of linked list.
class Node {
 public:
  explicit Node(frame_id_t frame_id) : frame_id_(frame_id) {}
  frame_id_t frame_id_;
  std::shared_ptr<Node> left_;
  std::shared_ptr<Node> right_;
};

/**
 * LRUReplacer implements the lru replacement policy, which approximates the Least Recently Used policy.
 */
class LRUReplacer : public Replacer {
 public:
  /**
   * Create a new LRUReplacer.
   * @param num_pages the maximum number of pages the LRUReplacer will be required to store
   */
  explicit LRUReplacer(size_t num_pages);

  /**
   * Destroys the LRUReplacer.
   */
  ~LRUReplacer() override;

  bool Victim(frame_id_t *frame_id) override;

  void Pin(frame_id_t frame_id) override;

  void Unpin(frame_id_t frame_id) override;

  size_t Size() override;

 private:
  // NOTE(student): implement me!
  std::mutex mu_;
  std::shared_ptr<Node> head_, tail_;
  std::unordered_map<frame_id_t, std::shared_ptr<Node>> page_table_;
  size_t capacity_;

  void insert(const std::shared_ptr<Node> &node);

  bool remove(const frame_id_t &frame_id);
};

}  // namespace bustub
