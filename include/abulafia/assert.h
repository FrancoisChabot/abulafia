//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_ASSERT_H_INCLUDED
#define ABULAFIA_ASSERT_H_INCLUDED

#include <cassert>
#include <stdexcept>

namespace abu {
#if defined(_MSC_VER)
inline void unreachable [[noreturn]] () { std::terminate(); }
#elif defined(__GNUC__)
inline void unreachable [[noreturn]] () { __builtin_unreachable(); }
#elif defined(__clang__)
// Clang technically has __builtin_unreachable, but as of clang 5.0.0, this
// leads to much better optimization
inline void unreachable [[noreturn]] () { std::terminate(); }
#endif
}  // namespace abu

#define abu_assert assert

#if defined(NDEBUG)
#define abu_assume(condition) \
  if (!(condition)) unreachable()
#else
#define abu_assume(condition) \
  if (!(condition)) abu_assert(condition)
#endif

#define abu_precondition(condition) abu_assume(condition)
#endif