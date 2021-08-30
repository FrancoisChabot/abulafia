//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_IMM_OPT_H_INCLUDED
#define ABULAFIA_PARSERS_IMM_OPT_H_INCLUDED

#include "abulafia/patterns/opt.h"

namespace abu {
template <Pattern P>
struct basic_parser<pat::opt<P>> {
  template <std::forward_iterator I, std::sentinel_for<I> S>
  static constexpr parse_result_t<std::iter_value_t<I>, pat::opt<P>> parse(
      I& i, S e, const pat::opt<P>& pat) {
    auto op_result = abu::parse(i, e, pat.operand());

    using res_t = parse_result_t<std::iter_value_t<I>, pat::opt<P>>;

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