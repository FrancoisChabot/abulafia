//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_CONTEXTS_SINGLE_FORWARD_H_
#define ABULAFIA_CONTEXTS_SINGLE_FORWARD_H_

#include "abulafia/config.h"

#include <cassert>
#include <stack>

namespace ABULAFIA_NAMESPACE {

// It is fed a single set of data at construction, and will never
// receive anything else. It maintains a light rollback stack that
// has no cost associated with in in next/advance/empty.
template <typename ITE_T>
class SingleForwardContext {
  using iterator = ITE_T;
  std::stack<iterator> rollback_stack_;

  iterator current_;
  iterator end_;

 public:
  enum {
    HAS_SKIPPER = false,
  };

  using value_type = decltype(*(ITE_T()));
  using base_ctx_t = SingleForwardContext<ITE_T>;

  base_ctx_t& root_ctx() { return *this; }
  enum { IS_RESUMABLE = false };

  SingleForwardContext(iterator b, iterator e) : current_(b), end_(e) {}
  SingleForwardContext(SingleForwardContext const&) = delete;
  constexpr bool final_buffer() const { return true; }

  void prepare_rollback() { rollback_stack_.push(current_); }

  void commit_rollback() {
    assert(!rollback_stack_.empty());
    current_ = rollback_stack_.top();
    rollback_stack_.pop();
  }

  void cancel_rollback() {
    assert(!rollback_stack_.empty());
    rollback_stack_.pop();
  }

  value_type next() const { return *current_; }

  void advance() {
    assert(!empty());
    current_++;
  }

  bool empty() const { return current_ == end_; }
};

}  // namespace ABULAFIA_NAMESPACE

#endif
