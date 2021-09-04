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

namespace details_ {
template <PatternTemplate<pat::repeat> auto pattern,
          Policies auto policies,
          std::input_iterator I>
struct basic_repeat {
  static constexpr bool is_done(std::size_t count) {
    return pattern.max > 0 && count >= pattern.max;
  }

  static constexpr op_result final_result(std::size_t count) {
    if (count >= pattern.min) {
      return success;
    } else {
      return failure_t{};
    }
  }
};
}  // namespace details_

template <PatternTemplate<pat::repeat> auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct parser<pattern, policies, I, S>
    : public details_::basic_repeat<pattern, policies, I> {
  template <typename Dst>
  static constexpr op_result parse(Dst& dst, I& i, const S& e) {
    using basic = details_::basic_repeat<pattern, policies, I>;
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
        if (basic::is_done(result_val.size())) {
          break;
        }
      } else {
        break;
      }
    }

    auto result = basic::final_result(result_val.size());
    if(result.is_success()) {
      dst = result_val;
    }
    return result;
  }
};

template <PatternTemplate<pat::repeat> auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct matcher<pattern, policies, I, S>
    : public details_::basic_repeat<pattern, policies, I> {
  static constexpr op_result match(I& i, const S& e) {
    using basic = details_::basic_repeat<pattern, policies, I>;

    using token_type = std::iter_value_t<I>;
    using value_type = parsed_value_t<decltype(pattern), token_type, policies>;
    using op_value_type =
        parsed_value_t<decltype(pattern.operand), token_type, policies>;
    using op_matcher = matcher<pattern.operand, policies, I, S>;

    std::size_t count = 0;
    while (1) {
      auto child_res = op_matcher::match(i, e);

      if (basic::is_done(count)) {
        ++count;
        if (pattern.max > 0 && count >= pattern.max) {
          break;
        }
      } else {
        break;
      }
    }

    return basic::final_result(count);
  }
};

}  // namespace abu::imm

#endif