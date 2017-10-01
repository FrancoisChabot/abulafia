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
class UintImpl {
public:
  using ctx_t = CTX_T;
  using dst_t = DST_T;
  using pat_t = Uint<BASE, DIGITS_MIN, DIGITS_MAX>;
  using digit_vals = DigitValues<BASE>;

  template<typename NEW_DST_T>
  using change_dst_t = UintImpl<CTX_T, NEW_DST_T, BASE, DIGITS_MIN, DIGITS_MAX>;

   UintImpl(ctx_t, dst_t dst, pat_t const&) {
    dst.get() = 0;
  }

  result peek(ctx_t ctx, pat_t const&) const {
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

  result consume(ctx_t ctx, dst_t dst, pat_t const&) {
    while (true) {
      if (ctx.data().empty()) {
        if (ctx.data().final_buffer()) {
          return digit_count_ >= DIGITS_MIN ? result::SUCCESS : result::FAILURE;
        } else {
          return result::PARTIAL;
        }
      }

      auto next = ctx.data().next();
      if (digit_vals::is_valid(next)) {
        dst.get() *= BASE;
        dst.get() += digit_vals::value(next);

        ++digit_count_;
        ctx.data().advance();

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

template <typename CTX_T, typename DST_T, int BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX>
struct ParserFactory<CTX_T, DST_T, Uint<BASE, DIGITS_MIN, DIGITS_MAX>> {
  static auto create(CTX_T ctx, DST_T dst, Uint<BASE, DIGITS_MIN, DIGITS_MAX> const& pat) {
    constexpr bool apply_atomic = !std::is_same<Nil, DST_T>::value;
    constexpr bool apply_fails_cleanly = DIGITS_MAX != 1;

    using a = UintImpl<CTX_T, DST_T, BASE, DIGITS_MIN, DIGITS_MAX>;
    using b = fulfill_req_if_t<a, Req::ATOMIC, apply_atomic>;
    using c = fulfill_req_if_t<b, Req::FAILS_CLEANLY, apply_fails_cleanly>;

    return c(ctx, dst, pat);
  }
};

}  // namespace ABULAFIA_NAMESPACE

#endif