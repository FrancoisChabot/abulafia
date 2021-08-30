//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_OPERATION_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_OPERATION_H_INCLUDED

#include <concepts>
#include <iterator>

#include "abulafia/pattern.h"
#include "abulafia/policy.h"
#include "abulafia/token.h"

namespace abu::coro {

enum class status { success, failure, partial };

template <typename T>
concept Context = requires(T x) {
  typename T::policies;
};

template <typename T>
concept InputContext = Context<T> && requires(T x) {
  typename T::token_type;
  typename T::iterator_type;

  std::input_iterator<typename T::iterator_type>;

  { x.iterator } -> std::same_as<typename T::iterator_type>;
  { x.end } -> std::sentinel_for<typename T::iterator_type>;
};

template <typename T>
concept ForwardContext = InputContext<T> && requires(T x) {
  typename T::checkpoint_type;

  { x.checkpoint() } -> std::same_as<typename T::checkpoint_type>;
};

template <Context Ctx, Pattern... Ops>
struct child_op {};

template <Context ctx, Pattern Pat>
struct parse_op;

template <std::input_iterator I,
          std::sentinel_for<I> S,
          Pattern Pat,
          Policy Pol = default_policy>
struct context {
  constexpr context(I& init_ite, S init_end, const Pat&)
      : iterator(init_ite), end(init_end) {}

  using policies = Pol;
  using token_type = std::iter_value_t<I>;

  template <typename Cb>
  constexpr status on_tokens(const Pat&, const Cb&) {
    return status::success;
  }

  template <typename Cb>
  constexpr status on_end(const Pat&, const Cb&) {
    return status::success;
  }

  I& iterator;
  S end;

  parse_op<>
};
}  // namespace abu::coro

#endif