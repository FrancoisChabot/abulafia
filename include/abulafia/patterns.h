//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_ABULAFIA_PATTERNS_H_INCLUDED
#define ABULAFIA_ABULAFIA_PATTERNS_H_INCLUDED

#include "abulafia/patterns/discard.h"
#include "abulafia/patterns/tok.h"

// Quick Reference:

// Patterns:
//   lit(Token)                   : discards single token
//   lit(std::input_range<Token>) : discards sequence of token
//   tok                          : matches any token
//   tok(token_set)               : matches any token allowed by the set.
//   eoi                          : matches end-of-input
//   fail                         : always fails
//   pass                         : always passes
//
//   discard(pat)                 : remove output
//   opt(pat)                     : make optional
//   repeat<min=0, max=0>(pat)    : Repeat between min and max times
//   action(pat, act)             : Execute act when pat parses.
//   raw(pat)                     : Replace the output of pat with the raw tokens
//   neg(pat)                     : Fails if pat passes.
//   construct<T>(pat)            : Use the output of pat to construct a T. Tuples are applied.
//
//   exclude(op, excl)            : exclusion
//   list(op, delim)              : delimited list
//
//   alt(a, b, ...)               : alternatives
//   sequence(a, b, ...)          : sequence


// Unary operators:
//   Pattern operator op(StrongPattern);

//   -pat               : opt(pat)
//   !pat               : neg(pat)
//   *pat               : repeat<0, 0>(pat)
//   +pat               : repeat<1, 0>(pat)
//
//   pat[act]           : action(pat, act)

// Binary operators:
//   Pattern operator op(StrongPattern, WeakPattern);
//   Pattern operator op(WeakPattern, StrongPattern);

//   op - excl          : exclude(op, excl)
//   op % delim         : list(op, delim)
//
//   a >> b >> ...      : seq(a, b, ...)
//   a | b | ...        : alt(a, b, ...)


// Strong Implicit patterns:
//  token_set    ->  tok(token_set)

// Weak Implicit patterns:
//   Token                   ->  lit(Token)
//   std::input_range<Token> ->  lit(std::input_range<Token>)

// N.B. A lot of the stuff being done in this file could be accomplished with
// CTAD on the pattern types themselves. Since this is not systematically
// the case, factories are used everywhere for consistency.
namespace abu {

// ***** tok *****
struct tok_api_obj {
  template <TokenSet T>
  constexpr auto operator()(T tokset) const {
    return pat::tok{std::move(tokset)};
  }
};
static constexpr tok_api_obj tok;

template <>
struct to_pattern<tok_api_obj> {
  constexpr auto operator()(const tok_api_obj&) const {
    return pat::tok{[](const auto&) { return true; }};
  }
};

template <TokenSet T>
struct to_pattern<T> {
  constexpr auto operator()(T tokset) const {
    return pat::tok{std::move(tokset)};
  }
};

// ***** discard *****
static constexpr auto discard(Pattern auto pat) {
  return pat::discard{std::move(pat)};
}

static constexpr auto discard(const PatternConvertible auto& pat) {
  return pat::discard{as_pattern(pat)};
}

// ***** eoi *****
static constexpr pat::eoi eoi;

// ***** pass *****
static constexpr pat::pass pass;

// ***** fail *****
static constexpr pat::fail fail;
}  // namespace abu

#endif