//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_CHARACTER_H_
#define ABULAFIA_PARSERS_COROUTINE_CHARACTER_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/leaf/character.h"
#include "abulafia/support/assert.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename CHARSET_T>
class CharImpl {
  using PAT_T = Char<CHARSET_T>;

 public:
   CharImpl(CTX_T, DST_T, PAT_T const&) {}

  result consume(CTX_T ctx, DST_T dst, PAT_T const& pat) {
    if (ctx.data().empty()) {
      return ctx.data().final_buffer() ? result::FAILURE : result::PARTIAL;
    }

    auto next = ctx.data().next();
    if (pat.char_set().is_valid(next)) {
      dst = next;
      ctx.data().advance();
      return result::SUCCESS;
    }
    return result::FAILURE;
  }

  result peek(CTX_T ctx, PAT_T const& pat) const {
    if (ctx.data().empty()) {
      return ctx.data().final_buffer() ? result::FAILURE : result::PARTIAL;
    }

    return pat.char_set().is_valid(ctx.data().next()) ? result::SUCCESS
                                               : result::FAILURE;
  }

};
template <typename CTX_T, typename DST_T, typename CHARSET_T >
struct ParserFactory<CTX_T, DST_T, Char<CHARSET_T>> {
  static auto create(CTX_T ctx, DST_T dst, Char<CHARSET_T> const& pat) {
    return CharImpl<CTX_T, DST_T, CHARSET_T>(ctx, dst, pat);
  }
};
}  // namespace ABULAFIA_NAMESPACE

#endif
