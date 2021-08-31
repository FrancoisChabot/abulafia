//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_DISCARD_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_DISCARD_H_INCLUDED

#include "abulafia/parsers/coro/child_operation.h"
#include "abulafia/parsers/coro/operation.h"
#include "abulafia/patterns.h"

namespace abu::coro {

template <ContextFor<pat::discard> Ctx>
class operation<Ctx> {
 public:
  using pattern_type = typename Ctx::pattern_type;
  using child_type = child_op<Ctx, &pattern_type::operand, op_type::match>;

  constexpr operation(const Ctx& ctx) : child_op_(ctx) {}

  constexpr coro_result<void> on_tokens(const Ctx& ctx) {
    return child_op_.on_tokens(ctx);
  }
  constexpr ::abu::parse_result<void> on_end(const Ctx& ctx) {
    return child_op_.on_end(ctx);
  }

 private:
  child_type child_op_;
};

}  // namespace abu::coro

#endif