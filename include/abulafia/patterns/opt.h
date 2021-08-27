//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_OPT_H_INCLUDED
#define ABULAFIA_PATTERNS_OPT_H_INCLUDED

#include <concepts>
#include <iterator>
#include <optional>

#include "abulafia/pattern.h"

namespace abu {

namespace pat {
template <Pattern OpT>
class opt {
 public:
  using operand_type = OpT;

  explicit constexpr opt(operand_type op) : operand_(std::move(op)) {}
  constexpr operand_type const& operand() const { return operand_; }

 private:
  [[no_unique_address]] operand_type operand_;
};
}  // namespace pat

namespace _opt {
template <typename T>
struct wrap_result : std::type_identity<std::optional<T>> {};

template <>
struct wrap_result<void> : std::type_identity<void> {};
}  // namespace _opt

template <Pattern P, std::input_iterator I>
struct parse_value<pat::opt<P>, I>
    : public _opt::wrap_result<parse_value_t<P, I>> {};

template <Pattern P>
struct basic_parser<pat::opt<P>> {
  template <std::forward_iterator I, std::sentinel_for<I> S>
  static constexpr parse_result_t<pat::opt<P>, I> parse(
      I& i, S e, const pat::opt<P>& pat) {
    auto op_result = abu::parse(i, e, pat.operand());

    using res_t = parse_result_t<pat::opt<P>, I>;

    if (op_result) {
      return res_t{*op_result};
    }

    return res_t{std::nullopt};
  }

  template <std::forward_iterator I, std::sentinel_for<I> S>
  static constexpr check_result_t check(I& i, S e, const pat::opt<P>& pat) {
    auto rb = i;
    auto op_result = abu::check(i, e, pat.operand());

    if (!op_result) {
      i = rb;
      return {};
    }

    return {};
  }
};
}  // namespace abu

#endif