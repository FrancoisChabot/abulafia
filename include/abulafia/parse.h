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

template <PatternLike auto pattern_like,
          Policies auto policies = default_policies,
          std::forward_iterator I,
          std::sentinel_for<I> S>
constexpr bool match(I& b, const S& e) {
  abu::data_chunk data{b, e};

  constexpr auto pattern = as_pattern(pattern_like);
  coro::matcher<pattern, policies, decltype(data)> matcher(data);

  auto status = matcher.on_tokens(data);
  if (status.is_partial()) {
    status = matcher.on_end(data);
  }

  return status.is_success();
}

template <PatternLike auto pattern,
          Policies auto policies = default_policies,
          std::ranges::forward_range R>
constexpr bool match(const R& range) {
  auto b = std::ranges::begin(range);
  auto e = std::ranges::end(range);
  return match<pattern, policies>(b, e);
}

template <PatternLike auto pattern_like,
          Policies auto policies = default_policies,
          std::forward_iterator I,
          std::sentinel_for<I> S>
constexpr auto parse(I& b, const S& e) {
  abu::data_chunk data{b, e};

  constexpr auto pattern = as_pattern(pattern_like);
  coro::parser<pattern, policies, decltype(data)> parser(data);

  using result_type =
      parsed_value_t<decltype(pattern), policies, decltype(data)>;
  std::optional<result_type> result;

  auto assign = [&](auto&& v) { result = std::move(v); };

  auto status = parser.on_tokens(data, assign);
  if (status.is_partial()) {
    status = parser.on_end(data, assign);
  }

  if (!status.is_success()) {
    throw no_match_error{};
  }
  return std::move(*result);
}

template <PatternLike auto pattern,
          Policies auto policies = default_policies,
          std::ranges::forward_range R>
constexpr auto parse(const R& range) {
  auto b = std::ranges::begin(range);
  auto e = std::ranges::end(range);
  return parse<pattern, policies>(b, e);
}

}  // namespace abu

#endif