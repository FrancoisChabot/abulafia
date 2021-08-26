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

// The Character pattern checks the next token against its character
// set. If the test passes, the next character is emmited and it succeeds.
template <Token TokT, TokenSet<TokT> TokSetT>
class tok_t : public pattern<tok_t<TokT, TokSetT>> {
 public:
  using token_set_type = TokSetT;
  using token_type = TokT;
  using value_type = token_type;

  template <Token U>
  static constexpr bool can_match = std::is_same_v<U, token_type>;

  explicit constexpr tok_t(token_set_type tokens) noexcept
      : tokens_(std::move(tokens)) {}

  constexpr bool matches(const token_type& c) const noexcept {
    return tokens_(c);
  }

 private:
  [[no_unique_address]] token_set_type tokens_;
};

template <typename T>
concept TokPattern = requires(T x) {
  { tok_t(x) } -> std::same_as<T>;
};

template <std::input_iterator I, std::sentinel_for<I> S, TokPattern PatT>
constexpr parse_result_t<PatT> parse(I& begin, S end, const PatT& pat) {
  if (begin != end) {
    auto t = *begin;
    if (pat.matches(t)) {
      ++begin;
      return t;
    }
  }
  return error{};
}

template <std::input_iterator I, std::sentinel_for<I> S, TokPattern PatT>
constexpr check_result_t check(I& begin, S end, const PatT& pat) {
  return parse(begin, end, pat);
}

}  // namespace abu

#endif