//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_DATASOURCES_CONTAINER_SEQUENCE_H_
#define ABULAFIA_DATASOURCES_CONTAINER_SEQUENCE_H_

#include "abulafia/config.h"

#include <cassert>
#include <list>
#include <memory>
#include <vector>

namespace ABULAFIA_NAMESPACE {

// This Data source will hold on to pre-allocated buffers until it can guarantee
// that it will not need to roll back to them.
// Pros:
//  - Resumable
//  - Does not perform any large allocation/moves
// Cons:
//  - If only 1 byte of a buffer is needed for rollback, the entire buffer
//    will be kept.

enum class IsFinal { FINAL, NOT_FINAL };

template <typename CONTAINER_T>
class ContainerSequenceDataSource {
  using iterator = typename CONTAINER_T::const_iterator;
  using buffer_list_t = std::list<std::shared_ptr<CONTAINER_T>>;
  using buffer_iterator = typename buffer_list_t::iterator;

  buffer_list_t buffers_;
  bool final_ = false;

  iterator current_;
  buffer_iterator current_buffer_;

  using rollback_entry_t = std::pair<iterator, buffer_iterator>;
  std::vector<rollback_entry_t> rollback_stack_;
  unsigned int empty_rollbacks_ = 0;


 public:
  using value_type = typename CONTAINER_T::value_type;

  enum { IS_RESUMABLE = true };

  ContainerSequenceDataSource() : current_buffer_(buffers_.end()) {}

  void add_buffer(std::shared_ptr<CONTAINER_T> b,
                  IsFinal f = IsFinal::NOT_FINAL) {
    assert(!final_);

    bool is_empty = empty();

    buffers_.push_back(b);

    // if we were empty, bootstrap.
    if (is_empty) {
      current_buffer_ = std::prev(buffers_.end());
      current_ = (*current_buffer_)->begin();
      for (unsigned int i = 0; i < empty_rollbacks_; ++i) {
        rollback_stack_.emplace_back(current_, current_buffer_);
      }
      empty_rollbacks_ = 0;
    }

    final_ = f == IsFinal::FINAL;
  }

  void add_buffer(std::unique_ptr<CONTAINER_T> b,
                  IsFinal f = IsFinal::NOT_FINAL) {
    add_buffer(std::shared_ptr<CONTAINER_T>(std::move(b)), f);
  }

  void add_buffer(CONTAINER_T b, IsFinal f = IsFinal::NOT_FINAL) {
    add_buffer(std::make_shared<CONTAINER_T>(std::move(b)), f);
  }

  bool final_buffer() const { return final_; }

  value_type next() const {
    assert(!empty());
    return *current_;
  }

  void advance() {
    current_++;
    if (current_ == (*current_buffer_)->end()) {
      auto next_buffer = std::next(current_buffer_);
      if (next_buffer != buffers_.end()) {
        current_ = (*next_buffer)->begin();
      } else {
        current_ = iterator();
      }

      // iI we are done with the front buffer, and the rollback stack
      // has no hold on it, dump the buffer.
      if (current_buffer_ == buffers_.begin()) {
        bool held_for_rollback =
            !rollback_stack_.empty() &&
            rollback_stack_.front().second == current_buffer_;
        if (!held_for_rollback) {
          buffers_.pop_front();
        }
      }

      current_buffer_ = next_buffer;
    }
  }

  bool empty() const { return current_buffer_ == buffers_.end(); }

  void prepare_rollback() {

    if (empty()) {
      ++empty_rollbacks_;
    }
    else {
      rollback_stack_.emplace_back(current_, current_buffer_);
    }
  }

  void commit_rollback() {
    if (empty_rollbacks_) {
      --empty_rollbacks_;
    }
    else {
      current_buffer_ = rollback_stack_.back().second;
      current_ = rollback_stack_.back().first;
      rollback_stack_.pop_back();
      cleanup_rollback_();
    }
  }

  void cancel_rollback() {
    if (empty_rollbacks_) {
      --empty_rollbacks_;
    }
    else {
      rollback_stack_.pop_back();
      cleanup_rollback_();
    }
  }

  bool isResumable() {return true;}
  
 private:
  void cleanup_rollback_() {
    // The only hold that matters is the front of the rollback stack.
    // This means cleanup will only happen when the entire stack has unrolled.
    if (rollback_stack_.empty()) {
      while (current_buffer_ != buffers_.begin()) {
        buffers_.pop_front();
      }
    }
  }
};

}  // namespace ABULAFIA_NAMESPACE

#endif
