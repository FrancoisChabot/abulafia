//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_REPEAT_H_INCLUDED
#define ABULAFIA_PATTERNS_REPEAT_H_INCLUDED

#include <array>
#include <concepts>
#include <iterator>
#include <string>
#include <vector>

#include "abulafia/pattern.h"

namespace abu {

namespace pat {
template <Pattern OpT, std::size_t Min, std::size_t Max>
class repeat {
  static_assert(Max == 0 || Max >= Min);
  static_assert(!(Min == 1 && Max == 1),
                "Don't use a repeat for a single value.");

 public:
  using operand_type = OpT;

  static constexpr std::size_t min_reps = Min;
  static constexpr std::size_t max_reps = Max;

  constexpr repeat(operand_type op) : operand_(std::move(op)) {}
  constexpr operand_type const& operand() const { return operand_; }

 private:
  [[no_unique_address]] operand_type operand_;
};
}  // namespace pat

namespace _repeat {

// If we are about to create a vector of
template <typename T, typename U>
struct promote_to_string : std::type_identity<T> {};

template <typename T>
struct promote_to_string<std::vector<T>, T>
    : std::type_identity<std::basic_string<char>> {};

template <typename T, std::size_t Min, std::size_t Max, std::input_iterator I>
struct promote_to_array : std::type_identity<std::vector<T>> {};

template <typename T, std::size_t Rep, std::input_iterator I>
struct promote_to_array<T, Rep, Rep, I>
    : std::type_identity<std::array<T, Rep>> {};

template <typename T, std::size_t Min, std::size_t Max, std::input_iterator I>
using wrap_result =
    promote_to_string<typename promote_to_array<T, Min, Max, I>::type,
                      std::iter_value_t<I>>;

}  // namespace _repeat

template <Pattern P, std::size_t Min, std::size_t Max, std::input_iterator I>
struct parse_value<pat::repeat<P, Min, Max>, I>
    : public _repeat::wrap_result<parse_value_t<P, I>, Min, Max, I> {};

template <Pattern P, std::size_t Min, std::size_t Max>
struct basic_parser<pat::repeat<P, Min, Max>> {
    // ***** check *****
  template <std::forward_iterator I, std::sentinel_for<I> S>
  static constexpr check_result_t check(I& i,
                                        S e,
                                        const pat::repeat<P, Min, Max>& pat) {
    auto rb = i;
    std::size_t reps = 0;

    while (Max == 0 || reps < Max) {
      auto op_res = abu::check(i, e, pat);
      if (!op_res) {
        break;
      }
      ++reps;
    }

    if (reps < Min) {
      i = rb;
      return error{};
    }
    return {};
  }

  // ***** parse *****
  template <std::forward_iterator I, std::sentinel_for<I> S>
  static constexpr parse_result_t<pat::repeat<P, Min, Max>, I> parse(
      I& i, S e, const pat::repeat<P, Min, Max>& pat) requires(Min != Max ||
                                                               Max == 0) {
    parse_value_t<pat::repeat<P, Min, Max>, I> result;

    auto rb = i;
    std::size_t reps = 0;

    while (Max == 0 || reps < Max) {
      auto op_res = abu::parse(i, e, pat.operand());
      if (!op_res) {
        break;
      }
      result.push_back(std::move(*op_res));
      ++reps;
    }

    if (reps < Min) {
      i = rb;
      return error{};
    }
    return result;
  }

  // Special handling of Fixed-sized repeats, since we can't push_back into an
  // array.
  template <std::forward_iterator I, std::sentinel_for<I> S>
  static constexpr parse_result_t<pat::repeat<P, Min, Max>, I> parse(
      I& i, S e, const pat::repeat<P, Min, Max>& pat) requires(Min == Max &&
                                                               Max != 0) {
    using landing_type = parse_value_t<pat::repeat<P, Min, Max>, I>;
    landing_type result;

    auto rb = i;
    if (parse_array_recur<I, S, landing_type, 0>(i, e, pat, result)) {
      return result;
    }
    i = rb;
    return error{};
  }

 private
     : template <std::forward_iterator I,
                 std::sentinel_for<I> S,
                 typename L,
                 std::size_t N>
       static constexpr bool
       parse_array_recur(I& i, S e, const pat::repeat<P, Min, Max>& pat, L& l) {
    if constexpr (N == Min) {
      return true;
    } else {
      auto op_res = abu::parse(i, e, pat.operand());
      if (!op_res) {
        return false;
      } else {
        std::get<N>(l) = *op_res;
        return parse_array_recur<I, S, L, N + 1>(i, e, pat, l);
      }
    }
  }
};
}  // namespace abu

#endif