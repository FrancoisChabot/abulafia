//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_EXCEPT_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_EXCEPT_H_INCLUDED

#include "abulafia/parsers/coro/child_operation.h"
#include "abulafia/parsers/coro/operation.h"
#include "abulafia/patterns.h"

namespace abu::coro {

template <ContextFor<pat::except> Ctx>
class operation<Ctx> {
 public:
  using pattern_type = typename Ctx::pattern_type;

  using except_child_op = child_op<Ctx, &pattern_type::except, op_type::match>;
  using operand_child_op = child_op<Ctx, &pattern_type::operand>;

  using value_type = typename operand_child_op::value_type;

  // ***** constructor ***** //
  constexpr operation(const Ctx& ctx) : child_op_(ctx) {}

  // ***** on_tokens ***** //
  constexpr coro_result<value_type> on_tokens(const Ctx& ctx) {
    // Test for the excluded pattern
    if (child_op_.index() == 0) {
      auto tmp = child_op_.on_tokens(child_index<0>, ctx);
      if (tmp.is_success()) {
        return match_failure_t{};
      } else if (tmp.is_match_failure()) {
        child_op_.reset(child_index<0>, child_index<1>, ctx);
      }
    }

    // Propagate to the operand parser.
    if (child_op_.index() == 1) {
      return child_op_.on_tokens(child_index<1>, ctx);
    }
    return partial_result_tag{};
  }

  // ***** on_end ***** //
  constexpr ::abu::parse_result<value_type> on_end(const Ctx& ctx) {
    if (child_op_.index() == 0) {
      auto tmp = child_op_.on_end(child_index<0>, ctx);
      if (tmp.is_success()) {
        return match_failure_t{};
      }
      child_op_.reset(child_index<0>, child_index<1>, ctx);
    }

    abu_assume(child_op_.index() == 1);

    return child_op_.on_end(child_index<1>, ctx);
  }

 private:
  child_op_set<except_child_op, operand_child_op> child_op_;
};

}  // namespace abu::coro

#endif