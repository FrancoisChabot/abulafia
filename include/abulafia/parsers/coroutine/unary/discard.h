//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_DISCARD_H_
#define ABULAFIA_PARSERS_COROUTINE_DISCARD_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/unary/discard.h"

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T>
class DiscardImpl {
  using ctx_t = CTX_T;
  using dst_t = DST_T;
  using req_t = REQ_T;
  using pat_t = Discard<CHILD_PAT_T>;

  using child_parser_t = Parser<ctx_t, Nil, req_t, CHILD_PAT_T>;

  child_parser_t child_parser_;

 public:
  DiscardImpl(ctx_t ctx, dst_t, pat_t const& pat)
      : child_parser_(ctx, nil, pat.operand()) {}

  Result consume(ctx_t ctx, dst_t, pat_t const& pat) {
    return child_parser_.consume(ctx, nil, pat.operand());
  }
};

template <typename CHILD_PAT_T>
struct ParserFactory<Discard<CHILD_PAT_T>> {
  using pat_t = Discard<CHILD_PAT_T>;

  static constexpr DstBehavior dst_behavior() { return DstBehavior::IGNORE; }

  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = DiscardImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T>;
};
}  // namespace ABULAFIA_NAMESPACE

#endif
