//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_REQUIREMENT_H_
#define ABULAFIA_PARSERS_COROUTINE_REQUIREMENT_H_

#include "abulafia/config.h"
#include "abulafia/parsers/coroutine/adapters/atomic.h"
#include "abulafia/parsers/coroutine/adapters/clean_failure.h"

#define PARSER_REQUEST_TABLE                                  \
  PARSER_REQUEST_ENTRY(ATOMIC, AtomicAdapter)                \
  PARSER_REQUEST_ENTRY(FAILS_CLEANLY, CleanFailureAdapter)

namespace ABULAFIA_NAMESPACE {

  enum class Req {
#define PARSER_REQUEST_ENTRY(R, A) R,
    PARSER_REQUEST_TABLE
#undef PARSER_REQUEST_ENTRY
  };

  struct DefaultReqs {
    enum {
#define PARSER_REQUEST_ENTRY(R, A) R = false,
      PARSER_REQUEST_TABLE
#undef PARSER_REQUEST_ENTRY
    };
  };

  template<typename PARSER_T, Req req, bool enable, typename enable_2=void>
  struct ReqFullfiller {
    using type = PARSER_T;
  };

#define PARSER_REQUEST_ENTRY(Request, Adapter) \
  template<typename PARSER_T> \
  struct ReqFullfiller<PARSER_T,              \
                       Req:: Request,         \
                       true,                  \
                       std::enable_if_t<      \
                         PARSER_T::ctx_t::req_t:: Request >> { \
    using type = Adapter <PARSER_T>;                                \
  };                                                                

  PARSER_REQUEST_TABLE
#undef PARSER_REQUEST_ENTRY

  template<typename PARSER_T, Req req, bool enable>
  using fulfill_req_if_t = typename ReqFullfiller<PARSER_T, req, enable>::type;
}  // namespace ABULAFIA_NAMESPACE

#undef PARSER_REQUEST_TABLE
#endif