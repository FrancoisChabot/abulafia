//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_CONTEXTS_SKIPPER_ADAPTER_H_
#define ABULAFIA_CONTEXTS_SKIPPER_ADAPTER_H_

#include "abulafia/config.h"

#include <cassert>
#include <stack>

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename SKIP_PAT_T>
class SkipperAdapter {
  CTX_T& parent_ctx_;
  SKIP_PAT_T const& skip_pat_;

 public:
  enum { HAS_SKIPPER = true, IS_RESUMABLE = CTX_T::IS_RESUMABLE };

  using skip_pattern_t = SKIP_PAT_T;
  using value_type = typename CTX_T::value_type;
  using base_ctx_t = typename CTX_T::base_ctx_t;

  base_ctx_t& root_ctx() { return parent_ctx_.root_ctx(); }
  skip_pattern_t const& skipPattern() const { return skip_pat_; }

  SkipperAdapter(CTX_T& parent, SKIP_PAT_T const& skip)
      : parent_ctx_(parent), skip_pat_(skip) {}

  bool final_buffer() const { return parent_ctx_.final_buffer(); }

  void prepare_rollback() { parent_ctx_.prepare_rollback(); }

  void commit_rollback() { parent_ctx_.commit_rollback(); }

  void cancel_rollback() { parent_ctx_.cancel_rollback(); }

  value_type next() const { return parent_ctx_.next(); }

  void advance() { parent_ctx_.advance(); }

  bool empty() const { return parent_ctx_.empty(); }
};

}  // namespace ABULAFIA_NAMESPACE

#endif
