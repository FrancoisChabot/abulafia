//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_MISC_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_MISC_H_INCLUDED

#include "abulafia/parsers/coro/operation.h"
#include "abulafia/patterns.h"

namespace abu::coro {

// ***** eoi *****
template <ContextForTag<pat::eoi> Ctx>
class operation<Ctx> {
 public:
  using pattern_type = typename Ctx::pattern_type;
  using value_type = void;

  constexpr operation(const Ctx&) {}

  constexpr coro_result<void> on_tokens(const Ctx& ctx) {
    if (ctx.iterator != ctx.end) {
      return match_failure_t{};
    }
    return partial_result_tag{};
  }
  constexpr ::abu::parse_result<void> on_end(const Ctx&) { return {}; }
};

// ***** fail *****
template <ContextForTag<pat::fail> Ctx>
class operation<Ctx> {
 public:
  using pattern_type = typename Ctx::pattern_type;

  constexpr operation(const Ctx&) {}

  constexpr coro_result<void> on_tokens(const Ctx&) {
    return match_failure_t{};
  }
  constexpr ::abu::parse_result<void> on_end(const Ctx&) {
    return match_failure_t{};
  }
};

// ***** pass *****
template <ContextForTag<pat::pass> Ctx>
class operation<Ctx> {
 public:
  using pattern_type = typename Ctx::pattern_type;

  constexpr operation(const Ctx&) {}

  constexpr coro_result<void> on_tokens(const Ctx&) { return {}; }
  constexpr ::abu::parse_result<void> on_end(const Ctx&) { return {}; }
};

}  // namespace abu::coro

#endif