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
  using context_type = abu::coro::context<I, S, Pat>;
  using result_type = pattern_value_t<Pat, context_type>;

  abu::coro::context<I, S, Pat> root_ctx{b, e, pat};

  std::optional<result_type> result;

  auto status =
      root_ctx.on_tokens(pat, [&](result_type r) { result = std::move(r); });

  if (status == coro::status::partial) {
    status =
        root_ctx.on_end(pat, [&](result_type r) { result = std::move(r); });
  }

  if (status == coro::status::failure) {
    throw parse_error{};
  }

  return *result;
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

/*
struct sink {
  bool& done;
  constexpr void operator()() const { done = true; }

  template <typename T>
  constexpr void operator()(const T&) const {
    done = true;
  }
};

template <std::forward_iterator I, std::sentinel_for<I> S, Pattern Pat>
constexpr bool match(I& b, const S& e, const Pat& pat) {
  using token_type = std::iter_value_t<I>;

  abu::coro::parse_op<token_type, Pat> operation;

  bool done = false;
  sink dst{done};
  try {
    operation.on_tokens(b, e, pat, dst);
    if (!done) {
      operation.on_end(pat, dst);
    }
  } catch (parse_error&) {
    return false;
  }
  return true;
}

template <std::forward_iterator I,
          std::sentinel_for<I> S,
          PatternConvertible Pat>
constexpr bool match(I& b, const S& e, const Pat& pat_like) {
  return match(b, e, as_pattern(pat_like));
}

template <std::ranges::forward_range R, PatternLike Pat>
constexpr bool match(const R& range, const Pat& pat) {
  auto b = std::ranges::begin(range);
  auto e = std::ranges::end(range);
  return match(b, e, pat);
}
*/
}  // namespace abu

#endif