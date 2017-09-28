//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_INT_H_
#define ABULAFIA_PARSERS_COROUTINE_INT_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/parsers/helpers/digit_values.h"
#include "abulafia/patterns/leaf/numeric/int.h"
#include "abulafia/support/assert.h"

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, int BASE, std::size_t DIGITS_MIN,
          std::size_t DIGITS_MAX>
class Parser<CTX_T, DST_T, Int<BASE, DIGITS_MIN, DIGITS_MAX>>
    : public ParserBase<CTX_T, DST_T> {
  using digit_vals = DigitValues<BASE>;
  using PAT_T = Int<BASE, DIGITS_MIN, DIGITS_MAX>;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const&)
      : ParserBase<CTX_T, DST_T>(ctx, dst) {
    dst = 0;
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
      if (digit_count_ == 0 && look_for_sign_) {
        look_for_sign_ = false;
        if (next == '-') {
          ctx.advance();
          neg_ = true;
          continue;
        }

        if (next == '+') {
          ctx.advance();
          continue;
        }
      }

      if (digit_vals::is_valid(next)) {
        dst *= BASE;
        dst += digit_vals::value(next);

        ++digit_count_;
        ctx.advance();

        if (digit_count_ == DIGITS_MAX) {
          if (neg_) {
            dst *= -1;
          }
          return result::SUCCESS;
        }
      } else {
        if (neg_) {
          dst *= -1;
        }
        return digit_count_ >= DIGITS_MIN ? result::SUCCESS : result::FAILURE;
      }
    }
  }

 private:
  std::size_t digit_count_ = 0;
  bool look_for_sign_ = true;
  bool neg_ = false;
};

}  // namespace ABULAFIA_NAMESPACE

#endif