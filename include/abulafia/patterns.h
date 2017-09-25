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
#include "abulafia/patterns/leaf/pass.h"

namespace ABULAFIA_NAMESPACE {

static constexpr Int<10, 1, 0> int_;
static constexpr Uint<10, 1, 0> uint_;

static constexpr Eoi eoi;
static constexpr Fail fail;
static constexpr Pass pass;
}  // namespace ABULAFIA_NAMESPACE
#endif