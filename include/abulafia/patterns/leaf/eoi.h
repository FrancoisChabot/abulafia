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

}  // namespace ABULAFIA_NAMESPACE

#endif
