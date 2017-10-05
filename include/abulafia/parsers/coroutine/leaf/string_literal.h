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
template <typename CTX_T, typename DST_T, typename CHAR_T>
class StringLiteralImpl {
  using PAT_T = StringLiteral<CHAR_T>;

  typename std::basic_string<CHAR_T>::const_iterator next_expected_;

 public:
  StringLiteralImpl(CTX_T, DST_T, PAT_T const& pat)
      : next_expected_(pat.begin()) {}

  Result consume(CTX_T ctx, DST_T, PAT_T const& pat) {
    while (1) {
      if (next_expected_ == pat.end()) {
        return Result::SUCCESS;
      }

      if (ctx.data().empty()) {
        return ctx.data().final_buffer() ? Result::FAILURE : Result::PARTIAL;
      }

      auto next = ctx.data().next();
      if (next == *next_expected_) {
        ctx.data().advance();
        ++next_expected_;
      } else {
        return Result::FAILURE;
      }
    }
  }
};

template <typename CHAR_T>
struct ParserFactory<StringLiteral<CHAR_T>> {
  static constexpr DstBehavior dst_behavior() { return DstBehavior::IGNORE; }

  using pat_t = StringLiteral<CHAR_T>;

  enum {
    ATOMIC = true,
    FAILS_CLEANLY = false,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = StringLiteralImpl<CTX_T, DST_T, CHAR_T>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
