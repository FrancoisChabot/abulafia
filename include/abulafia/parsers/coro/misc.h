//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_MISC_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_MISC_H_INCLUDED

#include "abulafia/parsers/coro/parser.h"
#include "abulafia/patterns.h"
#include "abulafia/utils.h"

namespace abu::coro {

template <std::same_as<pat::eoi> auto pattern,
          Policies auto policies,
          DataSource Data>
class matcher<pattern, policies, Data> {
 public:
  constexpr matcher(const Data&) {}

  constexpr op_result on_tokens(Data&) { return partial_result; }
  constexpr op_result on_end(Data& d) {
    if (d.empty()) {
      return success;
    } else {
      return failure_t{};
    }
  }
};

template <std::same_as<pat::pass> auto pattern,
          Policies auto policies,
          DataSource Data>
class matcher<pattern, policies, Data> {
 public:
  constexpr matcher(const Data&) {}

  constexpr op_result on_tokens(Data&) { return success; }
  constexpr op_result on_end(Data&) { return success; }
};

template <std::same_as<pat::fail> auto pattern,
          Policies auto policies,
          DataSource Data>
class matcher<pattern, policies, Data> {
 public:
  constexpr matcher(const Data&) {}

  constexpr op_result on_tokens(Data&) { return failure_t{}; }
  constexpr op_result on_end(Data&) { return failure_t{}; }
};

}  // namespace abu::coro

#endif