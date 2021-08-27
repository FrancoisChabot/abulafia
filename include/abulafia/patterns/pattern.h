//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_PATTERN_H_INCLUDED
#define ABULAFIA_PATTERNS_PATTERN_H_INCLUDED

#include <iterator>
#include <utility>

#include "abulafia/result.h"
#include "abulafia/token.h"

namespace abu {

namespace details {
using input_archetype = std::istream_iterator;
using forward_archetype = std::istream_iterator;
}  // namespace details

template <typename T>
concept ForwardPattern = requires(T x, details::forward_archetype b,
                                  details::forward_archetype e) {
  { ::abu::parse(b, e, x) } -> Result;
  { ::abu::check(b, e, x) } -> std::same_as<result<void>>;
};

// Input patterns can be parsed/check using an input iterator
template <typename T>
concept InputPattern = requires(T x, details::input_archetype b,
                                details::input_archetype e) {
  { ::abu::parse(b, e, x) } -> Result;
  { ::abu::check(b, e, x) } -> std::same_as<result<void>>;
};

// Inheriting from pattern<CrtpT>
template <typename CrtpT>
class pattern {
 public:
  using pattern_type = CrtpT;

  template <typename ActT>
  constexpr auto operator[](ActT act) const {
    return action(*static_cast<pattern_type const*>(this), std::move(act));
  }
};

namespace details {
template <typename CrtpT>
void pattern_test(pattern<CrtpT>&);
}

template <typename T>
struct to_pattern;

template <PatternConvertible T>
auto as_pattern(const T& p) {
  return to_pattern<T>{}(p);
}

template <typename T>
concept TruePattern = requires(T x) {
  ::abu::details::pattern_test(x);
};

template <typename T>
concept StrongPattern = requires(T x) {
  ::abu::is_strong_pattern<T>;
  { ::abu::to_pattern<T>{}(x) } -> Pattern;
};

template <typename T>
concept WeakPattern = requires(T x) {
  ::abu::is_weak_pattern<T>;
  { ::abu::to_pattern<T>{}(x) } -> Pattern;
};

}  // namespace abu

#endif