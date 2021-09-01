//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_TOK_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_TOK_H_INCLUDED

#include "abulafia/parsers/coro/parser.h"
#include "abulafia/patterns.h"

namespace abu::coro {

template <PatternTemplate<pat::tok> auto pattern,
          Policies auto policies,
          DataSource Data>
class parser<pattern, policies, Data> {
 public:
  constexpr parser(const Data&) {}

  template <ParseCallback<pattern, policies, Data> CbT>
  constexpr op_result on_tokens(Data& data, const CbT& cb) {
    if (data.empty()) {
      return partial_result;
    }

    if (!pattern.allowed(data.peek())) {
      return failure_t{};
    }

    cb(data.read());

    return success;
  }

  template <ParseCallback<pattern, policies, Data> CbT>
  constexpr op_result on_end(Data&, const CbT&) {
    return failure_t{};
  }
};

}  // namespace abu::coro

#endif