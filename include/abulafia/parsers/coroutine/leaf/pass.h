//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_PASS_H_
#define ABULAFIA_PARSERS_COROUTINE_PASS_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/leaf/pass.h"
#include "abulafia/support/assert.h"

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename REQ_T>
class PassImpl {
  static_assert(!REQ_T::CONSUMES_ON_SUCCESS);

  using ctx_t = CTX_T;
  using dst_t = Nil;
  using pat_t = Pass;

 public:
  PassImpl(CTX_T, Nil, Pass const&) {}

  Result consume(CTX_T, Nil, Pass const&) { return Result::SUCCESS; }
  Result peek(CTX_T, Pass const&) { return Result::SUCCESS; }
};

template<>
struct ParserFactory<Pass> {
  using pat_t = Pass;

  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };

  template<typename CTX_T, typename DST_T, typename REQ_T>
  using type = PassImpl<CTX_T, REQ_T>;
};


}  // namespace ABULAFIA_NAMESPACE

#endif
