//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_PATTERN_H_INCLUDED
#define ABULAFIA_PATTERNS_PATTERN_H_INCLUDED

#include <utility>

#include "abulafia/result.h"
#include "abulafia/token.h"

namespace abu {

template <typename CrtpT>
class pattern {
 public:
  using pattern_type = CrtpT;

  template <typename ActT>
  auto operator[](ActT act) const {
    return apply_action(*static_cast<pattern_type const*>(this),
                        std::move(act));
  }
};

namespace details {
template <typename CrtpT>
void pattern_test(pattern<CrtpT>&);
}

template <typename T>
concept Pattern = requires(T x) {
  typename T::value_type;
  ::abu::details::pattern_test(x);
  { T::template can_match<char> } -> std::convertible_to<bool>;
};

template <Pattern T>
using pattern_value_t = typename T::value_type;

template <Pattern PatT, Token TokT>
static constexpr bool pattern_can_match = PatT::template can_match<TokT>;

template <Pattern T>
using parse_result_t = result<pattern_value_t<T>>;

using check_result_t = result<void>;

// ***** Pattern Convertible ***** //
template <typename T>
struct to_pattern;

template <typename T>
concept PatternConvertible = requires(T x) {
  { ::abu::to_pattern<T>{}(x) } -> Pattern;
};

}  // namespace abu

#endif