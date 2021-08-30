//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_DISCARD_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_DISCARD_H_INCLUDED

#include "abulafia/parsers/coro/operation.h"
#include "abulafia/patterns.h"

namespace abu::coro {

namespace discard_ {

template <Context Ctx, Pattern Op>
struct parse_op<Ctx, pat::discard<Op>> {
  constexpr parse_op(Ctx& ctx) : child_op_(ctx) {}

  template <typename Cb>
  constexpr void on_tokens(Ctx& ctx) {
    return child_op_.on_tokens(ctx);
  }

  template <typename Cb>
  constexpr void on_end(const pattern_type& pat) {
    child_op_.on_end(ctx);
  }

  child_op<Ctx, Op> child_op_;
};

}  // namespace abu::coro

#endif