//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_EXCEPT_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_EXCEPT_H_INCLUDED

#include <variant>

#include "abulafia/parsers/coro/operation.h"
#include "abulafia/patterns.h"

namespace abu::coro {

namespace except_ {

struct handler {
  bool& feedback;

  constexpr void operator()() const { feedback = true; }
  constexpr void operator()(const auto&) const { feedback = true; }
};
}  // namespace except_

template <Token Tok, Pattern Op, Pattern Except, Policy Pol>
struct parse_op<Tok, pat::except<Op, Except>, Pol> {
  using pattern_type = pat::except<Op, Except>;
  using value_type = typename parse_op<Tok, Op, Pol>::value_type;

  using except_child_type = parse_op<Tok, Op, Pol>;
  using op_child_type = parse_op<Tok, Op, Pol>;

  using child_type = std::variant<except_child_type, op_child_type>;

  constexpr parse_op() : child_(except_child_type{}) {}

  template <std::input_iterator I, std::sentinel_for<I> S, typename Cb>
  constexpr void on_tokens(I& i,
                           const S& s,
                           const pattern_type& pat,
                           const Cb& complete) {
    if (child_.index() == 0) {
      bool passed = false;
      try {
        std::get<0>(child_).on_tokens(
            i, s, pat.except, except_::handler{passed});
      } catch (parse_error&) {
        child_ = op_child_type{};
      }

      if (passed) {
        throw parse_error{};
      }
    }

    if (child_.index() == 1) {
      std::get<1>(child_).on_tokens(i, s, pat.operand, complete);
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