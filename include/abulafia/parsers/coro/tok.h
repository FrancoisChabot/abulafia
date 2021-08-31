//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_TOK_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_TOK_H_INCLUDED

#include "abulafia/parsers/coro/child_operation.h"
#include "abulafia/parsers/coro/context.h"
#include "abulafia/patterns.h"

namespace abu::coro {

// template <ParseContext Ctx, TokenSet TSet>
// class operation<Ctx, pat::tok<TSet>> {
template <ContextFor<pat::tok_tag> Ctx>
class operation<Ctx> {
 public:
  constexpr operation(const Ctx&) {}

  template <typename CbT = noop_type>
  constexpr op_result on_tokens(const Ctx& ctx, const CbT& cb = {}) {
    if (ctx.iterator == ctx.end) {
      return partial_result;
    }

    if (!ctx.pattern.allowed(*ctx.iterator)) {
      return failure_t{};
    }

    cb(*ctx.iterator++);

    return success;
  }

  template <typename CbT = noop_type>
  constexpr op_result on_end(const Ctx&, const CbT& = {}) {
    return failure_t{};
  }
};

}  // namespace abu::coro

#endif