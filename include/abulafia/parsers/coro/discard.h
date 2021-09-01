//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_DISCARD_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_DISCARD_H_INCLUDED

#include "abulafia/parsers/coro/parser.h"
#include "abulafia/patterns.h"
#include "abulafia/utils.h"

namespace abu::coro {

template <PatternTemplate<pat::discard> auto pattern,
          Policies auto policies,
          DataSource Data>
class matcher<pattern, policies, Data> {
 public:
  using operand_paraser = matcher<pattern.operand, policies, Data>;

  constexpr matcher(const Data& data) : child_(data) {}

  constexpr op_result on_tokens(Data& data) { return child_.on_tokens(data); }
  constexpr op_result on_end(Data& data) { return child_.on_end(data); }

 private:
  operand_paraser child_;
};

}  // namespace abu::coro

#endif