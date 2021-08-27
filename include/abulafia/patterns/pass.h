//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_PASS_H_INCLUDED
#define ABULAFIA_PATTERNS_PASS_H_INCLUDED

#include <concepts>
#include <iterator>

#include "abulafia/patterns/pattern.h"

namespace abu {

// The Character pattern checks the next token against its character
// set. If the test passes, the next character is emmited and it succeeds.
namespace pat {
struct pass : public pattern<pass> {};
}  // namespace pat

template <std::input_iterator I, std::sentinel_for<I> S>
constexpr result<void> parse(I& i, S e, const pat::pass& pat) {
  return {};
}

template <std::input_iterator I, std::sentinel_for<I> S>
constexpr check_result_t check(I& i, S e, const pat::pass& pat) {
  return {};
}

}  // namespace abu

#endif