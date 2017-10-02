//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_FAIL_H_
#define ABULAFIA_PARSERS_COROUTINE_FAIL_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/leaf/fail.h"
#include "abulafia/support/assert.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T>
class FailImpl{
  using ctx_t = CTX_T;
  using dst_t = Nil;
  using pat_t = Fail;

 public:
   FailImpl(CTX_T ctx, Nil, PAT_T const&)
      : ParserBase<CTX_T>(ctx) {}

  Result consume(CTX_T, Nil, PAT_T const&) { return Result::FAILURE; }
  Result peek(CTX_T, PAT_T const&) { return Result::FAILURE; }
};

template <typename CTX_T, typename REQ_T>
struct ParserFactory<CTX_T, Nil, REQ_T, Fail> {
  static auto create(CTX_T ctx, Nil dst, Fail const& pat) {
    return FailImpl(ctx, dst, pat);
  }
};


}  // namespace ABULAFIA_NAMESPACE

#endif
