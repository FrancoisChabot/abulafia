//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_ABULAFIA_PARSE_H_INCLUDED
#define ABULAFIA_ABULAFIA_PARSE_H_INCLUDED

#include <ranges>

#include "abulafia/patterns/pattern.h"

namespace abu {
template <std::ranges::input_range Data, Pattern PatT>
constexpr auto parse(const Data& data, const PatT& pat) {
  static_assert(pattern_can_match<PatT, std::ranges::range_value_t<Data>>);

  auto beg = std::ranges::begin(data);
  return parse(beg, std::ranges::end(data), pat);
}

template <std::ranges::input_range Data, Pattern PatT>
constexpr auto check(const Data& data, const PatT& pat) {
  static_assert(pattern_can_match<PatT, std::ranges::range_value_t<Data>>);

  auto beg = std::ranges::begin(data);
  return check(beg, std::ranges::end(data), pat);
}
}  // namespace abu

#endif