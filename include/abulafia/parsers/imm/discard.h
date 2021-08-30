//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_IMM_DISCARD_H_INCLUDED
#define ABULAFIA_PARSERS_IMM_DISCARD_H_INCLUDED

#include "abulafia/patterns/discard.h"

namespace abu {

template <Pattern OpT>
struct basic_parser<pat::discard<OpT>> {
  const pat::discard<OpT>& pattern;

  template <std::input_iterator I, std::sentinel_for<I> S>
  constexpr void operator()(I& i, S e) {
    if (!basic_matcher<OpT>{pattern.operand()}(i, e)) {
    }
  }
};

template <Pattern OpT>
struct basic_matcher<pat::discard<OpT>> {
  const pat::discard<OpT>& pattern;
  template <std::input_iterator I, std::sentinel_for<I> S>
  constexpr bool operator()(I& i, S e) {
    return basic_matcher<OpT>{pattern.operand()}(i, e);
  }
};
}  // namespace abu

#endif