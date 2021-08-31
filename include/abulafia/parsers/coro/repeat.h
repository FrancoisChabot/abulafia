
//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROD_REPEAT_H_INCLUDED
#define ABULAFIA_PARSERS_COROD_REPEAT_H_INCLUDED

#include "abulafia/parsers/coro/child_operation.h"
#include "abulafia/parsers/coro/context.h"
#include "abulafia/patterns.h"

namespace abu::coro {

template <ContextFor<pat::repeat_tag> Ctx>
class operation<Ctx> {
 public:
  using pattern_type = typename Ctx::pattern_type;
  using child_type = child_op<Ctx, &pattern_type::operand>;
  // using checkpoint_type = typename Ctx::checkpoint_type;

  static constexpr std::size_t min_reps = pattern_type::min_reps;
  static constexpr std::size_t max_reps = pattern_type::max_reps;

  using value_type = parsed_value_t<pattern_type, Ctx>;

  constexpr operation(const Ctx& ctx)
      : /*checkpoint_{ctx.checkpoint()},*/ child_op_(ctx) {}

  template <typename CbT>
  constexpr op_result on_tokens(const Ctx& ctx, const CbT& cb) {
    while (true) {
      auto res = child_op_.on_tokens(
          ctx, [this](char v) { result_.push_back(std::move(v)); });
      if (res.is_partial()) {
        return partial_result;
      }

      if (res.is_match_failure()) {
        return finish_(ctx, cb);
      }

      abu_assume(res.is_success());

      if (max_reps != 0 && result_.size() == max_reps) {
        return finish_(ctx, cb);
      } else {
        child_op_.reset(ctx);
      }
    }
  }

  template <typename CbT = noop_type>
  constexpr op_result on_end(const Ctx& ctx, const CbT& cb = {}) {
    while (true) {
      auto res = child_op_.on_end(
          ctx, [this](char v) { result_.push_back(std::move(v)); });
      if (res.is_match_failure()) {
        return finish_(ctx, cb);
      }

      abu_assume(res.is_success());

      if (max_reps != 0 && result_.size() == max_reps) {
        return finish_(ctx, cb);
      } else {
        child_op_.reset(ctx);
      }
    }
  }

 private:
  template <typename CbT = noop_type>
  constexpr op_result finish_(const Ctx&, const CbT& cb = {}) {
    if (result_.size() >= min_reps) {
      cb(std::move(result_));
      return success;
    } else {
      // ctx.rollback(checkpoint_);
      return failure_t{};
    }
  }

  // checkpoint_type checkpoint_;
  value_type result_;
  child_type child_op_;
};

}  // namespace abu::coro

#endif
