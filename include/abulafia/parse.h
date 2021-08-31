//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSE_H_INCLUDED
#define ABULAFIA_PARSE_H_INCLUDED

#include <ranges>

#include "abulafia/parsers/coro.h"
#include "abulafia/pattern.h"

namespace abu {

template <std::forward_iterator I, std::sentinel_for<I> S, Pattern Pat>
constexpr auto parse(I& b, const S& e, const Pat& pat) {
  using result_type = parsed_value_t<Pat, std::iter_value_t<I>>;
  result_type result;
  auto result_assign = [&](result_type r) { result = std::move(r); };

  auto root_parser = abu::coro::make_parser<char>(pat, result_assign);

  op_result status = root_parser.add_data(b, e);

  if (status.is_partial()) {
    status = root_parser.end();
  }

  if (!status.is_success()) {
    // We know for a fact that this is not data starvation.
    throw no_match_error{};
  }
  return result;
}

template <std::forward_iterator I,
          std::sentinel_for<I> S,
          PatternConvertible Pat>
constexpr auto parse(I& b, const S& e, const Pat& pat_like) {
  return parse(b, e, as_pattern(pat_like));
}

template <std::ranges::forward_range R, PatternLike Pat>
constexpr auto parse(const R& range, const Pat& pat) {
  auto b = std::ranges::begin(range);
  auto e = std::ranges::end(range);
  return parse(b, e, pat);
}

/////////////////////////////////////////

template <std::forward_iterator I, std::sentinel_for<I> S, Pattern Pat>
constexpr bool match(I& b, const S& e, const Pat& pat) {
  // auto root_matcher = abu::coro::make_matcher<I, S>(pat);

  // auto status = root_matcher.advance(b, e);

  // if (status.is_partial()) {
  //   return root_matcher.end().is_success();
  // }

  // return status.is_success();
  (void)b;
  (void)e;
  (void)pat;
  return true;
}

template <std::forward_iterator I,
          std::sentinel_for<I> S,
          PatternConvertible Pat>
constexpr auto match(I& b, const S& e, const Pat& pat_like) {
  return match(b, e, as_pattern(pat_like));
}

template <std::ranges::forward_range R, PatternLike Pat>
constexpr auto match(const R& range, const Pat& pat) {
  auto b = std::ranges::begin(range);
  auto e = std::ranges::end(range);
  return match(b, e, pat);
}

}  // namespace abu

#endif