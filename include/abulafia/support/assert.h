//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_SUPPORT_ASSUME_H_
#define ABULAFIA_SUPPORT_ASSUME_H_

#include "abulafia/config.h"

#include <cassert>
#include <exception>

namespace ABULAFIA_NAMESPACE {
#if defined(_MSC_VER)
// needed for abu_unreachable()
inline __declspec(noreturn) void unreachable() {}
#endif
}

// TODO: add handling of other compilers as appropriate.
#if defined(__GNUC__) && defined(NDEBUG)
#define abu_assume(condition) \
  if (!(condition)) __builtin_unreachable()

#elif defined(__clang__) && defined(NDEBUG)
// Clang technically has __builtin_unreachable, but as of clang 5.0.0, this
// leads to much better optimization
#define abu_assume(condition) \
  if (!(condition)) std::terminate();
#else
#define abu_assume(condition) assert(condition)
#endif

#if defined(__GNUC__)
#define abu_unreachable() __builtin_unreachable()
#elif defined(__clang__)
#define abu_unreachable() __builtin_unreachable()
#elif defined(_MSC_VER)
#define abu_unreachable() unreachable()
#endif

#endif