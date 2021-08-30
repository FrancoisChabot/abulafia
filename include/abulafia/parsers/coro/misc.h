//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_MISC_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_MISC_H_INCLUDED

#include "abulafia/parsers/coro/operation.h"
#include "abulafia/patterns.h"

namespace abu::coro {

// ***** eoi *****
template <Token Tok, Policy Pol>
struct parse_op<Tok, pat::eoi, Pol> {
  using pattern_type = pat::eoi;
  using value_type = void;

  template <std::input_iterator I, std::sentinel_for<I> S, typename Cb>
  constexpr void on_tokens(I& i,
                 const S& s,
                 const pattern_type& pat,
                 const Cb& complete) {
    if (i != s) {
      throw parse_error{};
    }
  }

  template <typename Cb>
  constexpr void on_end(const pattern_type&, const Cb& cb) {
    cb();
  }
};

// ***** fail *****
template <Token Tok, Policy Pol>
struct parse_op<Tok, pat::fail, Pol> {
  using pattern_type = pat::fail;
  using value_type = void;

  template <std::input_iterator I, std::sentinel_for<I> S, typename Cb>
  constexpr void on_tokens(I& i,
                 const S& s,
                 const pattern_type& pat,
                 const Cb& complete) {
    throw parse_error{};
  }

  template <typename Cb>
  constexpr void on_end(const pattern_type&, const Cb& cb) {
    throw parse_error{};
  }
};

// ***** pass *****
template <Token Tok, Policy Pol>
struct parse_op<Tok, pat::pass, Pol> {
  using pattern_type = pat::pass;
  using value_type = void;

  template <std::input_iterator I, std::sentinel_for<I> S, typename Cb>
  constexpr void on_tokens(I& i,
                 const S& s,
                 const pattern_type& pat,
                 const Cb& complete) {
    complete();
  }

  template <typename Cb>
  constexpr void on_end(const pattern_type&, const Cb& complete) {
    complete();
  }
};

}  // namespace abu::coro

#endif