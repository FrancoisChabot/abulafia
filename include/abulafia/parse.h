//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSE_H_INCLUDED
#define ABULAFIA_PARSE_H_INCLUDED

#include <ranges>

#include "abulafia/pattern.h"

namespace abu {

// ***** parse *****

template <std::forward_iterator I, std::sentinel_for<I> S, PatternLike Pat>
constexpr auto parse(I& b, const S& e, const Pat& pat_like) {
  const auto& pat = as_pattern(pat_like);

  using parser_type = basic_parser<std::decay_t<decltype(pat)>>;
  return parser_type::parse(b, e, pat);
}

template <std::ranges::forward_range R, PatternLike Pat>
constexpr auto parse(const R& range, const Pat& pat) {
  auto b = std::ranges::begin(range);
  auto e = std::ranges::end(range);
  return parse(b, e, pat);
}

// ***** check *****

template <std::forward_iterator I, std::sentinel_for<I> S, Pattern Pat>
constexpr auto check(I& b,
                     const S& e,
                     const Pat& pat) requires(ExplicitelyCheckable<Pat>) {
  using parser_type = basic_parser<std::decay_t<decltype(pat)>>;
  return parser_type::check(b, e, pat);
}

template <std::forward_iterator I, std::sentinel_for<I> S, Pattern Pat>
constexpr auto check(I& b,
                     const S& e,
                     const Pat& pat) requires(!ExplicitelyCheckable<Pat>) {
  using parser_type = basic_parser<std::decay_t<decltype(pat)>>;

  static_assert(TrivialResult<decltype(parser_type::parse(b, e, pat))>,
                "Using parse() to check a non-trivial result");

  return parser_type::parse(b, e, pat);
}

template <std::forward_iterator I,
          std::sentinel_for<I> S,
          PatternConvertible Pat>
constexpr auto check(I& b, const S& e, const Pat& pat_like) {
  return check(b, e, as_pattern(pat_like));
}

template <std::ranges::forward_range R, PatternLike Pat>
constexpr auto check(const R& range, const Pat& pat) {
  auto b = std::ranges::begin(range);
  auto e = std::ranges::end(range);
  return check(b, e, pat);
}

}  // namespace abu

#endif