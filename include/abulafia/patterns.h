//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_PATTERNS_H_
#define ABULAFIA_PATTERNS_PATTERNS_H_

#include "abulafia/config.h"

#include "abulafia/patterns/leaf/eoi.h"
#include "abulafia/patterns/leaf/fail.h"
#include "abulafia/patterns/leaf/numeric/int.h"
#include "abulafia/patterns/leaf/numeric/uint.h"
#include "abulafia/patterns/leaf/pass.h"

#include "abulafia/patterns/binary/except.h"
#include "abulafia/patterns/binary/list.h"

namespace ABULAFIA_NAMESPACE {

// There is no harm in providing global instances of stateless parsers:
static constexpr Int<10, 1, 0> int_;
static constexpr Uint<10, 1, 0> uint_;

static constexpr Eoi eoi;
static constexpr Fail fail;
static constexpr Pass pass;


// Function-base API
template <typename LHS_T, typename RHS_T>
auto except(LHS_T&& lhs, RHS_T&& rhs) {
  return Except<pattern_t<LHS_T>, pattern_t<RHS_T>>(
      make_pattern(forward<LHS_T>(lhs)), make_pattern(forward<RHS_T>(rhs)));
}

template <typename LHS_T, typename RHS_T>
auto list(LHS_T&& lhs, RHS_T&& rhs) {
  return List<pattern_t<LHS_T>, pattern_t<RHS_T>>(
      make_pattern(forward<LHS_T>(lhs)), make_pattern(forward<RHS_T>(rhs)));
}

}  // namespace ABULAFIA_NAMESPACE
#endif