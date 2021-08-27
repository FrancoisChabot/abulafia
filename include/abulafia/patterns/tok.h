//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_TOK_H_INCLUDED
#define ABULAFIA_PATTERNS_TOK_H_INCLUDED

#include <concepts>
#include <iterator>

#include "abulafia/patterns/pattern.h"
#include "abulafia/token.h"

namespace abu {

namespace pat {
// The Character pattern checks the next token against its character
// set. If the test passes, the next character is emmited and it succeeds.
template <TokenSet TokSetT>
class tok : public pattern<tok<TokSetT>> {
 public:
  using token_set_type = TokSetT;

  explicit constexpr tok(token_set_type tokens) noexcept
      : tokens_(std::move(tokens)) {}

  template <Token TokT>
  constexpr bool matches(const TokT& c) const noexcept {
    return tokens_(c);
  }

 private:
  [[no_unique_address]] token_set_type tokens_;
};

}  // namespace pat

template <std::input_iterator I, std::sentinel_for<I> S, TokenSet T>
constexpr result<std::iter_value_t<I>> parse(I& i, S e,
                                             const pat::tok<T>& pat) {
  if (i != e) {
    auto t = *i;
    if (pat.matches(t)) {
      ++i;
      return t;
    }
  }
  return error{};
}

template <std::input_iterator I, std::sentinel_for<I> S, TokenSet T>
constexpr check_result_t check(I& i, S e, const pat::tok<T>& pat) {
  return parse(i, e, pat);
}

}  // namespace abu

#endif