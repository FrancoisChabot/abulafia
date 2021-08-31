//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_TOK_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_TOK_H_INCLUDED

#include "abulafia/parsers/coro/parser.h"
#include "abulafia/patterns.h"

namespace abu::coro {

// template <ParseContext Ctx, TokenSet TSet>
// class operation<Ctx, pat::tok<TSet>> {
template <ContextFor<pat::tok> Ctx>
class operation<Ctx> {
 public:
  constexpr operation(const Ctx&) {}

  constexpr op_result on_tokens(const Ctx& ctx) {
    if (ctx.empty()) {
      return partial_result;
    }

    if (!ctx.pattern.allowed(ctx.peek())) {
      return failure_t{};
    }

    if constexpr (Ctx::operation_type == op_type::parse) {
      ctx.return_value(ctx.get());
    }

    return success;
  }

  constexpr op_result on_end(const Ctx&) { return failure_t{}; }
};

}  // namespace abu::coro

#endif