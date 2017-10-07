//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_LEXEME_H_
#define ABULAFIA_PARSERS_COROUTINE_LEXEME_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/unary/lexeme.h"
#include "abulafia/patterns/leaf/fail.h"

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T>
class LexemeImpl {
  using pat_t = Lexeme<CHILD_PAT_T>;
  using sub_ctx_t = typename CTX_T::template set_skipper_t<Fail>;

  using child_parser_t = Parser<sub_ctx_t, DST_T, REQ_T, CHILD_PAT_T>;

  child_parser_t parser_;

 public:
  LexemeImpl(CTX_T ctx, DST_T dst, pat_t const& pat)
      : parser_(sub_ctx_t(ctx.data(), fail), dst, pat.operand()) {
  }

  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    return parser_.consume(sub_ctx_t(ctx.data(), fail), dst, pat.operand());
  }

  Result peek(CTX_T ctx, pat_t const& pat) {
    return parser_.peek(sub_ctx_t(ctx.data(), fail), pat.operand());
  }
};

template <typename CHILD_PAT_T>
struct ParserFactory<Lexeme<CHILD_PAT_T>> {
  using pat_t = Lexeme<CHILD_PAT_T>;

  static constexpr DstBehavior dst_behavior() { return ParserFactory<CHILD_PAT_T>::CHILD_PAT_T; }

  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = LexemeImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
