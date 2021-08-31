
//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROD_REPEAT_H_INCLUDED
#define ABULAFIA_PARSERS_COROD_REPEAT_H_INCLUDED

#include "abulafia/parsers/coro/child_operation.h"
#include "abulafia/parsers/coro/operation.h"
#include "abulafia/patterns.h"

namespace abu::coro {

template <ParseContext Ctx, Pattern Op, std::size_t Min, std::size_t Max>
class operation<Ctx, pat::repeat<Op, Min, Max>> {
  static_assert(ForwardContext<Ctx>);

 public:
  using pattern_type = pat::repeat<Op, Min, Max>;
  using child_type = child_op<Ctx, &pattern_type::operand>;
  using checkpoint_type = typename Ctx::checkpoint_type;

  using value_type = pattern_value_t<pattern_type, Ctx>;

  constexpr operation(const Ctx& ctx)
      : checkpoint_{ctx.checkpoint()}, child_op_(ctx) {}

  constexpr op_result on_tokens(const Ctx& ctx) {
    while (true) {
      auto res = child_op_.on_tokens(ctx);
      if (res.is_partial()) {
        return partial_result_tag{};
      }

      if (res.is_match_failure()) {
        return finish_(ctx);
      }

      abu_assume(res.is_success());
      result_.push_back(std::move(res.value()));

      if (Max != 0 && result_.size() == Max) {
        return finish_(ctx);
      } else {
        child_op_.reset(ctx);
      }
    }
  }

  constexpr op_result on_end(const Ctx& ctx) {
    while (true) {
      auto res = child_op_.on_end(ctx);
      if (res.is_match_failure()) {
        return finish_(ctx);
      }

      abu_assume(res.is_success());
      result_.push_back(std::move(res.value()));

      if (Max != 0 && result_.size() == Max) {
        return finish_(ctx);
      } else {
        child_op_.reset(ctx);
      }
    }
  }

 private:
  constexpr op_result finish_(const Ctx& ctx) {
    if (result_.size() >= Min) {
      ctx.result_value = std::move(result_);
      return {};
    } else {
      ctx.rollback(checkpoint_);
      return match_failure_t{};
    }
  }

  checkpoint_type checkpoint_;
  value_type result_;
  child_type child_op_;
};

}  // namespace abu::coro

#endif
