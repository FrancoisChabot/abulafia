//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_UNARY_NOT_H_
#define ABULAFIA_PATTERNS_UNARY_NOT_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/pattern.h"
#include "abulafia/support/assert.h"

namespace ABULAFIA_NAMESPACE {

template <typename PAT_T>
class Not : public Pattern<Not<PAT_T>> {
  PAT_T child_;

 public:
  Not(const PAT_T& child) : child_(child) {}
  Not(PAT_T&& child) : child_(std::move(child)) {}

  PAT_T const& operand() const { return child_; }
};

template <typename PAT_T, typename RECUR_TAG>
struct pattern_traits<Not<PAT_T>, RECUR_TAG> : public default_pattern_traits {
  using attr_type = Nil;

  enum {

    // if we can get our info by peeking, then we will not need to backtrack
    BACKTRACKS = !pattern_traits<PAT_T, RECUR_TAG>::PEEKABLE,
    PEEKABLE = pattern_traits<PAT_T, RECUR_TAG>::PEEKABLE,
    FAILS_CLEANLY = true,
    MAY_NOT_CONSUME = true,
    ATOMIC = true,
  };
};

template <typename PAT_T, typename CTX_T>
struct pat_attr_t<Not<PAT_T>, CTX_T> {
  using attr_type = Nil;
};
}  // namespace ABULAFIA_NAMESPACE

#endif
