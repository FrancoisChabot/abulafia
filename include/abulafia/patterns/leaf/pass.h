//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_LEAF_PASS_H_
#define ABULAFIA_PATTERNS_LEAF_PASS_H_

#include "abulafia/config.h"

#include "abulafia/patterns/pattern.h"

namespace ABULAFIA_NAMESPACE {

// The Fail pattern always fails, and does not emit anything.
class Pass : public Pattern<Pass> {};

static constexpr Pass pass;

}  // namespace ABULAFIA_NAMESPACE

#endif
