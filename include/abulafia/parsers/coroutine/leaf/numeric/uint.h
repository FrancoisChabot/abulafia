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

template <typename CTX_T, typename DST_T,
  std::size_t BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX>
class UIntImpl {
public:
  using pat_t = UInt<BASE, DIGITS_MIN, DIGITS_MAX>;

  using digit_vals = DigitValues<BASE>;

  UIntImpl(CTX_T, DST_T dst, pat_t const&) {
    dst.get() = 0;
  }

  Result consume(CTX_T ctx, DST_T dst, pat_t const&) {
    while (true) {
      if (ctx.data().empty()) {
        if (ctx.data().final_buffer()) {
          return digit_count_ >= DIGITS_MIN ? Result::SUCCESS : Result::FAILURE;
        } else {
          return Result::PARTIAL;
        }
      }

      auto next = ctx.data().next();
      if (digit_vals::is_valid(next)) {
        dst.get() *= typename DST_T::dst_type(BASE);
        dst.get() += digit_vals::value(next);

        ++digit_count_;
        ctx.data().advance();

        if (digit_count_ == DIGITS_MAX) {
          return Result::SUCCESS;
        }
      } else {
        return digit_count_ >= DIGITS_MIN ? Result::SUCCESS : Result::FAILURE;
      }
    }
  }

 private:
  std::size_t digit_count_ = 0;
};

template<std::size_t BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX>
struct ParserFactory<UInt<BASE, DIGITS_MIN, DIGITS_MAX>> {
  using pat_t = UInt<BASE, DIGITS_MIN, DIGITS_MAX>;

  enum {
    //TODO: We could provide ATOMIC in the DIGITS_MIN == DIGITS_MAX case
    ATOMIC = false,
    FAILS_CLEANLY = DIGITS_MAX == 1,
  };

  template<typename CTX_T, typename DST_T, typename REQ_T>
  using type = UIntImpl<CTX_T, DST_T, BASE, DIGITS_MIN, DIGITS_MAX>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif