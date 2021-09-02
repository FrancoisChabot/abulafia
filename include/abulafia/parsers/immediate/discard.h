//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_IMMEDIATE_DISCARD_H_INCLUDED
#define ABULAFIA_PARSERS_IMMEDIATE_DISCARD_H_INCLUDED

#include "abulafia/parsers/immediate/parser.h"
#include "abulafia/patterns.h"

namespace abu::imm {

template <PatternTemplate<pat::discard> auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct matcher<pattern, policies, I, S> {
  static constexpr op_result match(I& i, const S& e) {
    using op_matcher = matcher<pattern.operand, policies, I, S>;
    return op_matcher::match(i, e);
  }
};

}  // namespace abu::imm

#endif