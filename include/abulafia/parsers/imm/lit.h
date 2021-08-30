//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_IMM_LIT_H_INCLUDED
#define ABULAFIA_PARSERS_IMM_LIT_H_INCLUDED

#include <concepts>
#include <iterator>

#include "abulafia/patterns/lit.h"

namespace abu {
namespace lit_details {
template <typename I, typename T>
concept ValueLiteralIterator = std::input_iterator<I> &&
    requires(const I& i, const T& t) {
  { *i == t } -> std::convertible_to<bool>;
};

template <typename I, typename T>
concept RangeLiteralIterator =
    std::ranges::input_range<T> && !ValueLiteralIterator<I, T> &&
    std::input_iterator<I> &&
    requires(const I& i, const std::ranges::range_value_t<T>& t) {
  { *i == t } -> std::convertible_to<bool>;
};

// TODO: what if there's ambiguity?

}  // namespace lit_details

template <typename T>
struct basic_parser<pat::lit<T>> {
  // When T is a single token
  template <lit_details::ValueLiteralIterator<T> I, std::sentinel_for<I> S>
  static constexpr check_result_t parse(I& i, S e, const pat::lit<T>& pat) {
    if (i != e && *i == pat.expected()) {
      std::advance(i, 1);
      return {};
    }
    return error{};
  }

  // // When T is a range of tokens
  template <lit_details::RangeLiteralIterator<T> I, std::sentinel_for<I> S>
  static constexpr check_result_t parse(I& i, S e, const pat::lit<T>& pat) {
    const auto& pat_i = std::ranges::begin(pat.expected());
    const auto pat_e = std::ranges::end(pat.expected());

    if (std::mismatch(i, e, pat_i, pat_e).second == pat_e) {
      std::advance(i, std::ranges::size(pat.expected()));
      return {};
    }
    return error{};
  }
};

}  // namespace abu

#endif