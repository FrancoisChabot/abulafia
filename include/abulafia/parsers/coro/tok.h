//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_TOK_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_TOK_H_INCLUDED

#include "abulafia/parsers/coro/operation.h"
#include "abulafia/patterns.h"

namespace abu::coro {

template <InputContext Ctx, TokenSet TSet>
struct parse_op<Ctx, pat::tok<TSet>> {
  constexpr parse_op(Ctx&) {}

  template <typename Cb>
  constexpr status on_tokens(Ctx& ctx, const Cb& complete) {
    const auto& next = ctx.next();
    if (ctx.iterator != ctx.end) {
      if (!ctx.pattern.allowed(*ctx.iterator)) {
        return status::failure;
      }
      complete(*ctx.iterator++);
      return status::success;
    }

    return status::partial;
  }

  template <typename Cb>
  constexpr status on_end(Ctx&, const Cb&) {
    return status::failure;
  }
};

}  // namespace abu::coro

#endif