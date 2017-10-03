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
#include "abulafia/patterns/leaf/pass.h"
#include "abulafia/support/assert.h"

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T>
class FailImpl {
  using ctx_t = CTX_T;
  using dst_t = Nil;
  using pat_t = Fail;

 public:
  FailImpl(CTX_T, Nil, Fail const&) {}

  Result consume(CTX_T, Nil, Fail const&) { return Result::FAILURE; }
  Result peek(CTX_T, Fail const&) { return Result::FAILURE; }
};

template<>
struct ParserFactory<Fail> {
  using pat_t = Fail;

  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };

  template<typename CTX_T, typename DST_T, typename REQ_T>
  using type = FailImpl<CTX_T>;
};


}  // namespace ABULAFIA_NAMESPACE

#endif
