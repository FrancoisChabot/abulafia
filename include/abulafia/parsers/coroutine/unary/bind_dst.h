//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_BIND_DST_H_
#define ABULAFIA_PARSERS_COROUTINE_BIND_DST_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/unary/bind_dst.h"

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T>
class BindDstImpl {
  using pat_t = BindDst<CHILD_PAT_T>;
  using sub_ctx_t = typename CTX_T::template bind_dst<DST_T>;

  struct child_req_t : public REQ_T {
    enum {
      ATOMIC = false,
    };
  };

  using child_parser_t = Parser<sub_ctx_t, DST_T, child_req_t, CHILD_PAT_T>;

  child_parser_t parser_;

 public:
  BindDstImpl(CTX_T ctx, DST_T dst, pat_t const& pat)
      : parser_(sub_ctx_t(ctx.data(), ctx.skipper(), dst), dst, pat.operand()) {
  }

  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    return parser_.consume(sub_ctx_t(ctx.data(), ctx.skipper(), dst), dst,
                           pat.operand());
  }
};

template <typename CHILD_PAT_T>
struct ParserFactory<BindDst<CHILD_PAT_T>> {
  using pat_t = BindDst<CHILD_PAT_T>;

  static constexpr DstBehavior dst_behavior() {
    static_assert(
        ParserFactory<CHILD_PAT_T>::dst_behavior() != DstBehavior::IGNORE,
        "Why are we binding the dst for a Nil pattern?");

    return ParserFactory<CHILD_PAT_T>::dst_behavior();
  }

  enum {
    ATOMIC = false,
    FAILS_CLEANLY = true,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = BindDstImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
