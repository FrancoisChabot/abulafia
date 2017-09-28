//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_UINT_H_
#define ABULAFIA_PARSERS_COROUTINE_UINT_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/parsers/helpers/digit_values.h"
#include "abulafia/patterns/leaf/numeric/uint.h"
#include "abulafia/support/assert.h"

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, int BASE, std::size_t DIGITS_MIN,
          std::size_t DIGITS_MAX>
class Parser<CTX_T, DST_T, Uint<BASE, DIGITS_MIN, DIGITS_MAX>>
    : public ParserBase<CTX_T, DST_T> {
  using digit_vals = DigitValues<BASE>;
  using PAT_T = Uint<BASE, DIGITS_MIN, DIGITS_MAX>;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const&)
      : ParserBase<CTX_T, DST_T>(ctx, dst) {
    dst = 0;
  }

  result peek(CTX_T const& ctx, PAT_T const&) const {
    static_assert(DIGITS_MIN == 1, "we should not be peeking here.");

    if (ctx.empty()) {
      if (ctx.final_buffer()) {
        return result::FAILURE;
      } else {
        return result::PARTIAL;
      }
    }

    auto next = ctx.next();
    if (digit_vals::is_valid(next)) {
      return result::SUCCESS;
    } else {
      return result::FAILURE;
    }
  }

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const&) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }

    while (true) {
      if (ctx.empty()) {
        if (ctx.final_buffer()) {
          return digit_count_ >= DIGITS_MIN ? result::SUCCESS : result::FAILURE;
        } else {
          return result::PARTIAL;
        }
      }

      auto next = ctx.next();
      if (digit_vals::is_valid(next)) {
        dst *= BASE;
        dst += digit_vals::value(next);

        ++digit_count_;
        ctx.advance();

        if (digit_count_ == DIGITS_MAX) {
          return result::SUCCESS;
        }
      } else {
        return digit_count_ >= DIGITS_MIN ? result::SUCCESS : result::FAILURE;
      }
    }
  }

 private:
  std::size_t digit_count_ = 0;
};
}  // namespace ABULAFIA_NAMESPACE

#endif