//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_IMM_REPEAT_H_INCLUDED
#define ABULAFIA_PARSERS_IMM_REPEAT_H_INCLUDED

#include "abulafia/patterns/repeat.h"

namespace abu {

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
  static constexpr parse_result_t<std::iter_value_t<I>,
                                  pat::repeat<P, Min, Max>>
  parse(I& i, S e, const pat::repeat<P, Min, Max>& pat) requires(Min != Max ||
                                                                 Max == 0) {
    parse_value_t<std::iter_value_t<I>, pat::repeat<P, Min, Max>> result;

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
  static constexpr parse_result_t<std::iter_value_t<I>,
                                  pat::repeat<P, Min, Max>>
  parse(I& i, S e, const pat::repeat<P, Min, Max>& pat) requires(Min == Max &&
                                                                 Max != 0) {
    using result_type =
        parse_value_t<std::iter_value_t<I>, pat::repeat<P, Min, Max>>;
    result_type result;

    auto rb = i;
    if (parse_array_recur<I, S, result_type, 0>(i, e, pat, result)) {
      return result;
    }
    i = rb;
    return error{};
  }

 private :

     template <std::forward_iterator I,
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