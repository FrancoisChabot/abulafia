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

  Result consume(CTX_T ctx, DST_T dst, PAT_T const& pat) {
    if (ctx.data().empty()) {
      return ctx.data().final_buffer() ? Result::FAILURE : Result::PARTIAL;
    }

    auto next = ctx.data().next();
    if (pat.char_set().is_valid(next)) {
      dst = next;
      ctx.data().advance();
      return Result::SUCCESS;
    }
    return Result::FAILURE;
  }
};
template <typename CHARSET_T >
struct ParserFactory<Char<CHARSET_T>> {
  using pat_t = Char<CHARSET_T>;

  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };

  template<typename CTX_T, typename DST_T, typename REQ_T>
  using type = CharImpl<CTX_T, DST_T, CHARSET_T>;
};
}  // namespace ABULAFIA_NAMESPACE

#endif
