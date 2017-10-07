//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_EOI_H_
#define ABULAFIA_PARSERS_COROUTINE_EOI_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/leaf/eoi.h"
#include "abulafia/support/assert.h"

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T>
class EoiImpl {
  using ctx_t = CTX_T;
  using dst_t = Nil;
  using pat_t = Fail;

 public:
  EoiImpl(CTX_T, Nil, Eoi const&) {}

  Result consume(CTX_T ctx, Nil, Eoi const& pat) { return peek(ctx, pat); }

  Result peek(CTX_T ctx, Eoi const&) {
    if (ctx.data().empty()) {
      return ctx.data().final_buffer() ? Result::SUCCESS : Result::PARTIAL;
    }
    return Result::FAILURE;
  }
};

template <>
struct ParserFactory<Eoi> {
  using pat_t = Eoi;

  static constexpr DstBehavior dst_behavior() { return DstBehavior::IGNORE; }

  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = EoiImpl<CTX_T>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
