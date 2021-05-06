//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.cpp
//
// Identification: src/buffer/lru_replacer.cpp
//
// Copyright_ (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_replacer.h"

#include <memory>

namespace bustub {

LRUReplacer::LRUReplacer(size_t num_pages) : capacity_(num_pages) {
  head_ = std::make_shared<Node>(-1);
  tail_ = std::make_shared<Node>(-1);
  head_->right_ = tail_;
  tail_->left_ = head_;
}

LRUReplacer::~LRUReplacer() = default;

bool LRUReplacer::Victim(frame_id_t *frame_id) {
  std::scoped_lock lock(mu_);
  if (page_table_.empty()) {
    return false;
  }
  auto last_node = tail_->left_;
  remove(last_node->frame_id_);
  *frame_id = last_node->frame_id_;
  return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
  std::scoped_lock lock(mu_);
  if (page_table_.find(frame_id) != page_table_.end()) {
    remove(frame_id);
  }
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
  std::scoped_lock lock(mu_);
  auto iter = page_table_.find(frame_id);
  if (iter == page_table_.end()) {
    if (page_table_.size() >= capacity_) {
      // Remove item
      while (page_table_.size() >= capacity_) {
        auto p = tail_->left_;
        remove(p->frame_id_);
      }
    }
    auto newNode = std::make_shared<Node>(frame_id);
    insert(newNode);
  }
}

size_t LRUReplacer::Size() {
  std::scoped_lock lock(mu_);
  auto size_ = page_table_.size();
  return size_;
}

void LRUReplacer::insert(const std::shared_ptr<Node> &node) {
  if (node == nullptr) {
    return;
  }
  node->right_ = head_->right_;
  node->left_ = head_;
  head_->right_->left_ = node;
  head_->right_ = node;
  page_table_[node->frame_id_] = node;
}

bool LRUReplacer::remove(const frame_id_t &frame_id) {
  auto iter = page_table_.find(frame_id);
  if (iter == page_table_.end()) {
    return false;
  }
  auto node = iter->second;
  node->right_->left_ = node->left_;
  node->left_->right_ = node->right_;
  page_table_.erase(frame_id);
  return true;
}

}  // namespace bustub
