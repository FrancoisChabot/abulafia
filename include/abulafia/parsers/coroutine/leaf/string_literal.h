//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_STRING_LITERAL_H_
#define ABULAFIA_PARSERS_COROUTINE_STRING_LITERAL_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/leaf/string_literal.h"
#include "abulafia/support/assert.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {
/*
template <typename CTX_T, typename DST_T, typename CHAR_T>
class Parser<CTX_T, DST_T, StringLiteral<CHAR_T>>
    : public ParserBase<CTX_T, DST_T> {
  using PAT_T = StringLiteral<CHAR_T>;

  typename std::basic_string<CHAR_T>::const_iterator next_expected_;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst), next_expected_(pat.begin()) {}

  result consume(CTX_T& ctx, Nil&, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }

    while (1) {
      if (next_expected_ == pat.end()) {
        return result::SUCCESS;
      }

      if (ctx.empty()) {
        return ctx.final_buffer() ? result::FAILURE : result::PARTIAL;
      }

      auto next = ctx.next();
      if (next == *next_expected_) {
        ctx.advance();
        ++next_expected_;
      } else {
        return result::FAILURE;
      }
    }
  }
};
*/
}  // namespace ABULAFIA_NAMESPACE

#endif
