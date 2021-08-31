//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSE_H_INCLUDED
#define ABULAFIA_PARSE_H_INCLUDED

#include <ranges>

#include "abulafia/parser.h"
#include "abulafia/parsers/coro.h"
#include "abulafia/pattern.h"

namespace abu {

template <Policy Pol = default_policy,
          std::forward_iterator I,
          std::sentinel_for<I> S,
          Pattern Pat>
constexpr auto parse(I& b, const S& e, const Pat& pat) {
  using result_type = parsed_value_t<Pat, std::iter_value_t<I>>;
  std::optional<result_type> result;
  auto result_assign = [&](result_type r) { result = std::move(r); };

  using root_ctx = coro::context<I, S, Pol, Pat>;
  using root_op = coro::operation<root_ctx>;

  root_ctx ctx{b, e, pat};
  root_op op{ctx};

  auto status = op.on_tokens(ctx, result_assign);

  if (status.is_partial()) {
    status = op.on_end(ctx, result_assign);
  }

  if (status.is_match_failure()) {
    throw no_match_error{};
  }

  return std::move(*result);
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

template <Policy Pol = default_policy,
          std::forward_iterator I,
          std::sentinel_for<I> S,
          Pattern Pat>
constexpr bool match(I& b, const S& e, const Pat& pat) {
  using root_ctx = coro::context<I, S, Pol, Pat>;
  using root_op = coro::operation<root_ctx>;

  root_ctx ctx{b, e, pat};
  root_op op{ctx};

  auto status = op.on_tokens(ctx);

  if (status.is_partial()) {
    status = op.on_end(ctx);
  }

  return status.is_success();
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