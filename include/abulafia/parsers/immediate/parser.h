//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_IMMEDIATE_PARSER_H_INCLUDED
#define ABULAFIA_PARSERS_IMMEDIATE_PARSER_H_INCLUDED

#include "abulafia/op_result.h"
#include "abulafia/utils.h"

namespace abu::imm {

template <Pattern auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct parser;

template <Pattern auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct matcher {
  static constexpr op_result match(I& i, const S& e) {
    abu::details_::null_sink_t null_sink;
    return parser<pattern, policies, I, S>::parse(null_sink, i, e);
  }
};

template <Pattern auto pattern,
          Policies auto policies = default_policies,
          typename Dst,
          std::input_iterator I,
          std::sentinel_for<I> S>
constexpr op_result parse(Dst& dst, I& i, const S& e) {
  return parser<pattern, policies, I, S>::parse(dst, i, e);
}

// Unless specified otherwise, we defer to the parser when matching.
template <Pattern auto pattern,
          Policies auto policies = default_policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
constexpr op_result match(I& i, const S& e) {
  return matcher<pattern, policies, I, S>::match(i, e);
}

}  // namespace abu::imm

#endif