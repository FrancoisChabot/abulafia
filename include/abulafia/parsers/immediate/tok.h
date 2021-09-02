//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_IMMEDIATE_TOK_H_INCLUDED
#define ABULAFIA_PARSERS_IMMEDIATE_TOK_H_INCLUDED

#include "abulafia/parsers/immediate/parser.h"
#include "abulafia/patterns.h"

namespace abu::imm {

template <PatternTemplate<pat::tok> auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct parser<pattern, policies, I, S> {
  template <typename Dst>
  static constexpr op_result parse(Dst& dst, I& i, const S& e) {
    if (i != e && pattern.allowed(*i)) {
      dst = *i++;
      return success;
    }
    return failure_t{};
  }
};

}  // namespace abu::imm

#endif