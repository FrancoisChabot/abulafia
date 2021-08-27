//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_ABULAFIA_PATTERNS_H_INCLUDED
#define ABULAFIA_ABULAFIA_PATTERNS_H_INCLUDED

#include "abulafia/pattern.h"
#include "abulafia/patterns/discard.h"
#include "abulafia/patterns/lit.h"
#include "abulafia/patterns/misc.h"
#include "abulafia/patterns/opt.h"
#include "abulafia/patterns/repeat.h"
#include "abulafia/patterns/tok.h"

// Quick Reference:

// Patterns:
//   tok                          : matche any token
//   tok(token_set)               : matche any token allowed by the set.
//   lit(Token)                   : expect and discard single token
//   lit(std::input_range<Token>) : expect and discard sequence of token
//   eoi                          : matche end-of-input
//   fail                         : always fail
//   pass                         : always pass
//
//   discard(pat)                 : remove output
//   opt(pat)                     : make optional
//   repeat<min, max>(pat)        : Repeat within range (max=0 means no limit)
//   raw(pat)                     : replace output with sequence of tokens
//   action(pat, act)             : Execute act when pat parses.
//   raw(pat)                     : Replace output with original tokens
//   neg(pat)                     : Fails if pat passes.
//   construct<T>(pat)            : Use the output of pat to construct a T.
//                                  - Tuples are applied.
//
//   exclude(pat, excl)            : pat if not excl
//   list(pat, delim)              : delimited list
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
namespace _api {
struct tok_api {
  template <TokenSet T>
  constexpr auto operator()(T tokset) const {
    return pat::tok{std::move(tokset)};
  }
};

}  // namespace _api
static constexpr _api::tok_api tok;

template <>
struct to_pattern<_api::tok_api> {
  static constexpr bool is_strong_conversion = true;

  constexpr auto operator()(const _api::tok_api&) const {
    return pat::tok([](const auto&) { return true; });
  }
};

template <TokenSet TokSet>
struct to_pattern<TokSet> {
  constexpr auto operator()(TokSet tok_set) const {
    return pat::tok(std::move(tok_set));
  }
};

// ***** lit *****
static constexpr auto lit(auto r) { return pat::lit{std::move(r)}; }

// ***** eoi *****
static constexpr pat::eoi eoi;

// ***** pass *****
static constexpr pat::pass pass;

// ***** fail *****
static constexpr pat::fail fail;


// ***** drop *****
static constexpr auto discard(PatternLike auto pat_like) {
  return pat::discard{as_pattern(pat_like)};
}

// ***** opt *****
static constexpr auto opt(PatternLike auto pat_like) {
  return pat::opt{as_pattern(pat_like)};
}

// ***** repeat *****
template<std::size_t Min, std::size_t Max>
static constexpr auto repeat(PatternLike auto pat_like) {
  using P = std::decay_t<decltype(as_pattern(pat_like))>;
  return pat::repeat<P, Min, Max>{as_pattern(pat_like)};
}



}  // namespace abu

#endif