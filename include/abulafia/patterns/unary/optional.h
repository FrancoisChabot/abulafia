//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_UNARY_OPTIONAL_H_
#define ABULAFIA_PATTERNS_UNARY_OPTIONAL_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/pattern.h"
#include "abulafia/support/assert.h"

#include <optional>

namespace ABULAFIA_NAMESPACE {

template <typename PAT_T>
class Optional : public Pattern<Optional<PAT_T>> {
  PAT_T child_;

 public:
  Optional(PAT_T child) : child_(std::move(child)) {}

  PAT_T const& operand() const { return child_; }
};

template <typename PAT_T, typename RECUR_TAG>
struct pattern_traits<Optional<PAT_T>, RECUR_TAG> : public default_pattern_traits {
  using attr_type = Nil;

  enum {
    BACKTRACKS = pattern_traits<PAT_T, RECUR_TAG>::BACKTRACKS,
    PEEKABLE = true,
    FAILS_CLEANLY = true,
    MAY_NOT_CONSUME = true,
    ATOMIC = true,
    APPENDS_DST = pattern_traits<PAT_T, RECUR_TAG>::APPENDS_DST,
    STABLE_APPENDS = true,
  };
};

template <typename PAT_T, typename CTX_T>
struct pat_attr_t<Optional<PAT_T>, CTX_T> {
  using attr_type = std::optional<attr_t<PAT_T, CTX_T>>;
};
}  // namespace ABULAFIA_NAMESPACE

#endif
