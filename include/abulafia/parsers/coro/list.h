//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_LIST_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_LIST_H_INCLUDED

#include <variant>

#include "abulafia/parsers/coro/operation.h"
#include "abulafia/patterns.h"

namespace abu::coro {

namespace list_ {
template <typename T, std::size_t Min, std::size_t Max>
struct value : public std::type_identity<std::vector<T>> {};

template <std::size_t Min, std::size_t Max>
struct value<char, Min, Max>
    : public std::type_identity<std::basic_string<char>> {};

// template <typename T, std::size_t N>
// struct value<T, N, N> : public std::type_identity<std::array<T, N>> {};

template <typename T, std::size_t Min, std::size_t Max>
using value_t = typename value<T, Min, Max>::type;

}  // namespace list_

template <Token Tok, Pattern Op, Pattern Delim, Policy Pol>
struct parse_op<Tok, pat::list<Op, Delim>, Pol> {
  using pattern_type = pat::list<Op, Delim>;
  using value_type =
      list_::value_t<typename parse_op<Tok, Operand>::value_type> > ;

  using op_child_type = parse_op<Tok, Op, Pol>;
  using delim_child_type = parse_op<Tok, Delim, Pol>;

  using child_type = std::variant<op_child_type, delim_child_type>;

  constexpr parse_op() : child_(op_child_type{}) {}

  template <std::input_iterator I, std::sentinel_for<I> S, typename Cb>
  constexpr void on_tokens(I& i,
                           const S& s,
                           const pattern_type& pat,
                           const Cb& complete) {
    bool keep_going = true;
    while (keep_going) {
      if (child_.index() == 0) {
      }

      if (child_.index() == 1) {
      }
    }
  }

  template <typename Cb>
  constexpr void on_end(const pattern_type& pat, const Cb& complete) {
    if (child_.index() == 0) {
      bool passed = false;
      try {
        std::get<0>(child_).on_end(pat.except, except_::handler{passed});
      } catch (parse_error&) {
        child_ = op_child_type{};
      }
      if (passed) {
        throw parse_error{};
      }
    }

    if (child_.index() == 1) {
      std::get<1>(child_).on_end(pat.operand, complete);
    }
  }

  child_type child_;
};

}  // namespace abu::coro

#endif