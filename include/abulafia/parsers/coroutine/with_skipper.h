//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_WITH_SKIPPER_H_
#define ABULAFIA_PARSERS_COROUTINE_WITH_SKIPPER_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/with_skipper.h"

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T,
          typename SKIP_T>
class WithSkipperImpl {
  using ctx_t = CTX_T;
  using dst_t = DST_T;
  using pat_t = WithSkipper<CHILD_PAT_T, SKIP_T>;

  using sub_ctx_t = typename ctx_t::template set_skipper_t<SKIP_T>;
  Parser<sub_ctx_t, dst_t, REQ_T, CHILD_PAT_T> child_parser_;

 public:
  WithSkipperImpl(ctx_t ctx, dst_t dst, pat_t const& pat)
      : child_parser_(sub_ctx_t(ctx.data(), pat.getSkip()), dst,
                      pat.getChild()) {}

  Result consume(ctx_t ctx, dst_t dst, pat_t const& pat) {
    return child_parser_.consume(sub_ctx_t(ctx.data(), pat.getSkip()), dst,
                                 pat.getChild());
  }
};

template <typename CHILD_PAT_T, typename SKIP_T>
struct ParserFactory<WithSkipper<CHILD_PAT_T, SKIP_T>> {
  using pat_t = WithSkipper<CHILD_PAT_T, SKIP_T>;

  static constexpr DstBehavior dst_behavior() {
    return ParserFactory<CHILD_PAT_T>::dst_behavior();
  }

  enum {
    ATOMIC = ParserFactory<CHILD_PAT_T>::ATOMIC,
    FAILS_CLEANLY = ParserFactory<CHILD_PAT_T>::FAILS_CLEANLY,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = WithSkipperImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T, SKIP_T>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
