//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_ADAPTER_SKIP_H_
#define ABULAFIA_PARSERS_COROUTINE_ADAPTER_SKIP_H_

#include "abulafia/config.h"

#include "abulafia/context.h"
#include "abulafia/dst_wrapper/select_wrapper.h"
#include "abulafia/parsers/coroutine/dst_behavior.h"
#include "abulafia/patterns/leaf/fail.h"
#include "abulafia/result.h"

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
struct Parser_t;

// This is to be used to enforce ATOMIC on a parser
// that does not naturally meet that requirement
template <typename CTX_T, typename DST_T, typename REQ_T,
          typename PARSER_FACTORY_T>
class SkipAdapter {
 public:
  using pat_t = typename PARSER_FACTORY_T::pat_t;

  using child_parser_t =
      typename PARSER_FACTORY_T::template type<CTX_T, DST_T, REQ_T>;

  using skip_context_t = Context<typename CTX_T::datasource_t, Fail, Nil>;

  struct skip_req_t {
    enum { ATOMIC = false, FAILS_CLEANLY = true, CONSUMES_ON_SUCCESS = true };
  };

  using skip_parser_t =
      Parser_t<skip_context_t, Nil, skip_req_t, typename CTX_T::skip_pattern_t>;

  SkipAdapter(CTX_T ctx, DST_T dst, pat_t const& pat)
      : skip_parser_(skip_context_t(ctx.data(), fail, nil), nil, ctx.skipper()),
        adapted_parser_(ctx, dst, pat) {}

  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    while (!skipping_done_) {
      auto status = skip_parser_.consume(skip_context_t(ctx.data(), fail, nil),
                                         nil, ctx.skipper());
      switch (status) {
        case Result::SUCCESS:
          skip_parser_ = skip_parser_t(skip_context_t(ctx.data(), fail, nil),
                                       nil, ctx.skipper());
          break;
        case Result::FAILURE:
          skipping_done_ = true;
          break;
        case Result::PARTIAL:
          return Result::PARTIAL;
          break;
      }
    }
    return adapted_parser_.consume(ctx, dst, pat);
  }

 private:
  bool skipping_done_ = false;
  skip_parser_t skip_parser_;
  child_parser_t adapted_parser_;
};

template <typename FACTORY_T>
struct SkipFactoryAdapter {
  using pat_t = typename FACTORY_T::pat_t;

  static constexpr DstBehavior dst_behavior() {
    return FACTORY_T::dst_behavior();
  }

  enum {
    ATOMIC = FACTORY_T::ATOMIC,
    FAILS_CLEANLY = FACTORY_T::FAILS_CLEANLY,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = SkipAdapter<CTX_T, DST_T, REQ_T, FACTORY_T>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif