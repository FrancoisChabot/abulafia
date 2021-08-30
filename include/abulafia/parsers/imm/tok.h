//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_IMM_TOK_H_INCLUDED
#define ABULAFIA_PARSERS_IMM_TOK_H_INCLUDED

#include <iterator>

#include "abulafia/parse_error.h"
#include "abulafia/patterns/tok.h"

namespace abu {

template <TokenSet T>
struct basic_matcher<pat::tok<T>> {
  const pat::tok<T>& pattern;

  template <std::input_iterator I, std::sentinel_for<I> S>
  constexpr bool operator()(I& i, S e) {
    bool result = i != e && pattern.allows(*i);
    if (result) {
      ++i;
    }
    return result;
  }
};

template <TokenSet T>
struct basic_parser<pat::tok<T>> {
  const pat::tok<T>& pattern;

  template <std::input_iterator I, std::sentinel_for<I> S>
  constexpr std::iter_value_t<I> operator()(I& i, S e) {
    if (i == e || !pattern.allows(*i)) {
      throw parse_error{};
    }
    return *i++;
  }
};

}  // namespace abu

#endif