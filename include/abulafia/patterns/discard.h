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

#include "abulafia/pattern.h"
namespace abu {

namespace pat {
template <Pattern OpT>
class discard {
 public:
  using operand_type = OpT;

  constexpr discard(operand_type op) : operand_(std::move(op)) {}
  constexpr operand_type const& operand() const { return operand_; }

 private:
  [[no_unique_address]] operand_type operand_;
};
}  // namespace pat

template <Pattern OpT>
struct basic_parser<pat::discard<OpT>> {
  template <std::input_iterator I, std::sentinel_for<I> S>
  static constexpr check_result_t parse(I& i,
                                        S e,
                                        const pat::discard<OpT>& pat) {
    return check(i, e, pat.operand());
  }
};
}  // namespace abu

#endif