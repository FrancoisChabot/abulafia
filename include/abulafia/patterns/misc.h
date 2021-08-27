//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_MISC_H_INCLUDED
#define ABULAFIA_PATTERNS_MISC_H_INCLUDED

#include "abulafia/pattern.h"

namespace abu {

namespace pat {
struct eoi {};   // End of input
struct fail {};  // Always fails
struct pass {};  // Always passes
}  // namespace pat

// ***** eoi *****
template <>
struct basic_parser<pat::eoi> {
  template <std::input_iterator I, std::sentinel_for<I> S>
  static constexpr result<void> parse(I& i, S e, const pat::eoi&) noexcept {
    if (i != e) {
      return error{};
    }
    return {};
  }
};

// ***** fail *****
template <>
struct basic_parser<pat::fail> {
  template <std::input_iterator I, std::sentinel_for<I> S>
  static constexpr result<void> parse(I&, S, const pat::fail&) noexcept {
    return error{};
  }
};

// ***** pass *****
template <>
struct basic_parser<pat::pass> {
  template <std::input_iterator I, std::sentinel_for<I> S>
  static constexpr result<void> parse(I&, S, const pat::pass&) noexcept {
    return {};
  }
};

static_assert(InputPattern<pat::eoi>);
static_assert(InputPattern<pat::fail>);
static_assert(InputPattern<pat::pass>);
}  // namespace abu

#endif