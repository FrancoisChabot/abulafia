//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_CHAR_SYMBOL_H_
#define ABULAFIA_PARSERS_COROUTINE_CHAR_SYMBOL_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/leaf/char_symbol.h"
#include "abulafia/support/assert.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename CHAR_T, typename VAL_T>
class CharSymbolImpl {
 public:
  using pat_t = CharSymbol<CHAR_T, VAL_T>;

  CharSymbolImpl(CTX_T, DST_T, pat_t const&) {}

  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    if (ctx.data().empty()) {
      return ctx.data().final_buffer() ? Result::FAILURE : Result::PARTIAL;
    }

    auto next = ctx.data().next();
    auto found = pat.mapping().find(next);
    if (found == pat.mapping().end()) {
      return Result::FAILURE;
    }

    dst = found->second;
    return Result::SUCCESS;
  }

  Result peek(CTX_T ctx, pat_t const& pat) {
    if (ctx.data().empty()) {
      return ctx.data().final_buffer() ? Result::FAILURE : Result::PARTIAL;
    }

    auto next = ctx.data().next();
    auto found = pat.mapping().find(next);
    if (found == pat.mapping().end()) {
      return Result::FAILURE;
    }
    return Result::SUCCESS;
  }
};

template <typename CHAR_T, typename VAL_T>
struct ParserFactory<CharSymbol<CHAR_T, VAL_T>> {
  using pat_t = CharSymbol<CHAR_T, VAL_T>;

  static constexpr DstBehavior dst_behavior() { return DstBehavior::VALUE; }

  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = CharSymbolImpl<CTX_T, DST_T, CHAR_T, VAL_T>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
