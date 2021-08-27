//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_DISCARD_H_INCLUDED
#define ABULAFIA_PATTERNS_DISCARD_H_INCLUDED

#include <concepts>
#include <iterator>

#include "abulafia/patterns/pattern.h"

namespace abu {

namespace pat {
template <Pattern OpT>
class discard : public pattern<discard<OpT>> {
 public:
  using operand_type = OpT;

  discard(operand_type op) : operand_(std::move(op)) {}
  operand_type const& operand() const { return operand_; }

 private:
  [[no_unique_address]] operand_type operand_;
};
}  // namespace pat

template <std::input_iterator I, std::sentinel_for<I> S, Pattern T>
constexpr auto parse(I& i, S e, const pat::discard<T>& pat) {
  return check(i, e, pat.operand());
}

template <std::input_iterator I, std::sentinel_for<I> S, Pattern T>
constexpr check_result_t check(I& i, S e, const pat::discard<T>& pat) {
  return check(i, e, pat.operand());
}

}  // namespace abu

#endif