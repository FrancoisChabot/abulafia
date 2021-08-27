//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_TOK_H_INCLUDED
#define ABULAFIA_PATTERNS_TOK_H_INCLUDED

#include "abulafia/pattern.h"
#include "abulafia/token.h"

namespace abu {

namespace pat {
// Checks the next token against its token set. If the test passes, the next
// character is emmited and it succeeds.
template <TokenSet TokSetT>
class tok {
 public:
  using token_set_type = TokSetT;

  explicit constexpr tok(token_set_type allowed) noexcept
      : allowed_(std::move(allowed)) {}

  template <Token TokT>
  constexpr bool allows(const TokT& c) const noexcept {
    return allowed_(c);
  }

 private:
  [[no_unique_address]] token_set_type allowed_;
};

}  // namespace pat

template <TokenSet T, std::input_iterator I>
struct parse_value<pat::tok<T>, I>
    : public std::type_identity<std::iter_value_t<I>> {};

template <TokenSet T>
struct basic_parser<pat::tok<T>> {
  template <std::input_iterator I, std::sentinel_for<I> S>
  static constexpr check_result_t check(I& i, S e, const pat::tok<T>& pat) {
    if (i != e && pat.allows(*i)) {
      std::advance(i, 1);
      return {};
    }
    return error{};
  }

  template <std::input_iterator I, std::sentinel_for<I> S>
  static constexpr parse_result_t<pat::tok<T>, I> parse(
      I& i, S e, const pat::tok<T>& pat) {
    if (i != e) {
      auto t = *i;
      if (pat.allows(t)) {
        std::advance(i, 1);
        return t;
      }
    }
    return error{};
  }
};

}  // namespace abu

#endif