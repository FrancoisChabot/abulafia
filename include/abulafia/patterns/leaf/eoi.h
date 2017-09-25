//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_LEAF_EOI_H_
#define ABULAFIA_PATTERNS_LEAF_EOI_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/pattern.h"

namespace ABULAFIA_NAMESPACE {

// The End-of-input expects to hit the end of the input data, and won't emit
// anything.
class Eoi : public Pattern<Eoi> {};

template <typename RECUR_TAG>
struct pattern_traits<Eoi, RECUR_TAG> : public default_pattern_traits {
  using attr_type = Nil;

  enum {
    BACKTRACKS = false,
    FAILS_CLEANLY = true,
    MAY_NOT_CONSUME = false,
    PEEKABLE = true,
    ATOMIC = true,
  };
};

template <typename CTX_T>
struct pat_attr_t<Eoi, CTX_T> {
  using attr_type = Nil;
};

template <typename CTX_T>
class Parser<CTX_T, Nil, Eoi> : public ParserBase<CTX_T, Nil> {
  using PAT_T = Eoi;

 public:
  Parser(CTX_T& ctx, Nil& dst, PAT_T const&)
      : ParserBase<CTX_T, Nil>(ctx, dst) {}

  result consume(CTX_T& ctx, Nil&, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }

    return peek(ctx, pat);
  }

  result peek(CTX_T& ctx, PAT_T const&) {
    if (ctx.empty()) {
      return ctx.final_buffer() ? result::SUCCESS : result::PARTIAL;
    }
    return result::FAILURE;
  }
};

}  // namespace ABULAFIA_NAMESPACE

#endif
