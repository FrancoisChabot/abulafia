//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_IMMEDIATE_MISC_H_INCLUDED
#define ABULAFIA_PARSERS_IMMEDIATE_MISC_H_INCLUDED

#include "abulafia/parsers/immediate/parser.h"
#include "abulafia/patterns.h"

namespace abu::imm {

template <std::same_as<pat::eoi> auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct matcher<pattern, policies, I, S> {
  static constexpr op_result match(I& i, const S& e) {
    if (i == e) {
      return success;
    } else {
      return failure_t{};
    }
  }
};

template <std::same_as<pat::pass> auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct matcher<pattern, policies, I, S> {
  static constexpr op_result match(I& i, const S& e) { return success; }
};

template <std::same_as<pat::fail> auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct matcher<pattern, policies, I, S> {
  static constexpr op_result match(I& i, const S& e) { return failure_t{}; }
};

}  // namespace abu::imm

#endif