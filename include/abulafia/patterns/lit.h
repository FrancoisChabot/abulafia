//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_LIT_H_INCLUDED
#define ABULAFIA_PATTERNS_LIT_H_INCLUDED

#include <concepts>
#include <iterator>

#include "abulafia/patterns/pattern.h"
#include "abulafia/token.h"

namespace abu {

template <typename T>
concept LiteralString = requires(T x) {
  std::input_range<T>;
  Token<std::ranges::range_value_t<T>>;
};

namespace pat {
  template <LiteralString T>
  class lit : public pattern<lit<T>> {
   public:
    explicit constexpr lit(T expected) noexcept
        : expected_(std::move(expected)) {}

   private:
    [[no_unique_address]] T expected_;
  };

  template <LiteralString auto T>
  class lit_ct : public pattern<ct_lit<T>> {};

}  // namespace pat

template <std::input_iterator I, std::sentinel_for<I> S, LiteralString T>
constexpr result<std::iter_value_t<I>> parse(I& i, S e,
                                             const pat::lit<T>& pat) {
  if (i != e) {
    auto t = *i;
    if (pat.matches(t)) {
      ++i;
      return t;
    }
  }
  return error{};
}
}  // namespace abu

#endif