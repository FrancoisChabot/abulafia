//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_LEAF_FAIL_H_
#define ABULAFIA_PATTERNS_LEAF_FAIL_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/pattern.h"

namespace ABULAFIA_NAMESPACE {

// The Fail pattern always fails, and does not emit anything.
class Fail : public Pattern<Fail> {};

template <typename RECUR_TAG>
struct pattern_traits<Fail, RECUR_TAG> : public default_pattern_traits {
  enum {
    ATOMIC = true,
    BACKTRACKS = false,
    FAILS_CLEANLY = true,
    PEEKABLE = true,
    // a bit of a white lie. It never succeeds so it does not matter
    MAY_NOT_CONSUME = false,

  };
};

template <typename CTX_T>
struct pat_attr_t<Fail, CTX_T> {
  using attr_type = Nil;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
