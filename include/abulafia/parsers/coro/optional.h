//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_OPT_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_OPT_H_INCLUDED

#include "abulafia/parsers/coro/operation.h"
#include "abulafia/patterns.h"

namespace abu::coro {

namespace opt_ {

template <typename T>
struct value : public std::type_identity<std::optional<T>> {};

template <typename T>
struct value<std::optional<T>> : public std::type_identity<std::optional<T>> {};

template <>
struct value<void> : public std::type_identity<void> {};

template <typename T>
using value_t = typename value<T>::type;

template <typename T, typename Cb>
struct handler {
  const Cb& cb;

  constexpr void operator()(T v) const { cb(std::move(v)); }
  constexpr void nullopt() const { cb(std::nullopt); }
};

template <typename Cb>
struct handler<void, Cb> {
  const Cb& cb;

  constexpr void operator()() const { cb(); }
  constexpr void nullopt() const { cb(); }
};
}  // namespace opt_

template <Token Tok, Pattern Op, Policy Pol>
struct parse_op<Tok, pat::optional<Op>, Pol> {
  using pattern_type = pat::optional<Op>;
  using value_type = opt_::value_t<typename parse_op<Tok, Op>::value_type>;

  template <std::input_iterator I, std::sentinel_for<I> S, typename Cb>
  constexpr void on_tokens(I& i,
                           const S& s,
                           const pattern_type& pat,
                           const Cb& complete) {
    opt_::handler<value_type, Cb> handler{complete};

    try {
      child_op_.on_tokens(i, s, pat.operand, handler);
    } catch (parse_error&) {
      handler.nullopt();
    }
  }

  template <typename Cb>
  constexpr void on_end(const pattern_type& pat, const Cb& complete) {
    opt_::handler<value_type, Cb> handler{complete};

    try {
      child_op_.on_end(pat.operand, handler);
    } catch (parse_error&) {
      handler.nullopt();
    }
  }

  parse_op<Tok, Op, Pol> child_op_;
};

}  // namespace abu::coro

#endif