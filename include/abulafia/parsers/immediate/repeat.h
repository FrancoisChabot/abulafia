//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_IMMEDIATE_REPEAT_H_INCLUDED
#define ABULAFIA_PARSERS_IMMEDIATE_REPEAT_H_INCLUDED

#include "abulafia/parsers/immediate/parser.h"
#include "abulafia/patterns.h"

namespace abu::imm {

template <PatternTemplate<pat::repeat> auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct parser<pattern, policies, I, S> {
  template <typename Dst>
  static constexpr op_result parse(Dst& dst, I& i, const S& e) {
    using token_type = std::iter_value_t<I>;

    using value_type = parsed_value_t<decltype(pattern), token_type, policies>;
    using op_value_type =
        parsed_value_t<decltype(pattern.operand), token_type, policies>;
    using op_parser = parser<pattern.operand, policies, I, S>;

    value_type result_val;
    while (1) {
      std::optional<op_value_type> landing;
      auto child_res = op_parser::parse(landing, i, e);
      if (child_res.is_success()) {
        abu_assume(landing);
        result_val.push_back(*landing);
        if (result_val.size() >= pattern.max) {
          break;
        }
      } else {
        break;
      }
    }

    if (result_val.size() >= pattern.min) {
      dst = std::move(result_val);
      return success;
    } else {
      return failure_t{};
    }
  }
};

}  // namespace abu::imm

#endif