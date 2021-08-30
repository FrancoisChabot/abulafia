//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERN_H_INCLUDED
#define ABULAFIA_PATTERN_H_INCLUDED

#include <iterator>

#include "abulafia/archetypes.h"
#include "abulafia/policy.h"
#include "abulafia/token.h"

namespace abu {

// Real patterns have to be implemented by a parser
struct real_pattern_tag {
  using pattern_tag = real_pattern_tag;
};

// Strong patterns
//  - can be converted to real patterns
//  - can be used in unary operators as if they were real patterns
//  - can be used in binary operators as if they were real patterns
struct strong_pattern_tag {
  using pattern_tag = strong_pattern_tag;
};

// Weak patterns
//  - can be converted to real patterns
//  - can be used in binary operators with real or strong patterns
struct weak_pattern_tag {
  using pattern_tag = weak_pattern_tag;
};

template <typename T>
struct pattern_traits {
  using pattern_tag = typename T::pattern_tag;
};

template <typename T>
using pattern_tag_t = typename pattern_traits<T>::pattern_tag;

// All patterns are required to be able to operate on forward iterators.
template <typename T>
concept Pattern = std::same_as<pattern_tag_t<T>, real_pattern_tag>;

template <typename T>
concept PatternConvertible = requires(T x) {
  { pattern_traits<T>::to_pattern(x) } -> Pattern;
};

template <typename T>
concept PatternLike = Pattern<T> || PatternConvertible<T>;

template <typename T>
concept StrongPattern =
    PatternConvertible<T> && std::same_as<pattern_tag_t<T>, strong_pattern_tag>;

template <typename T>
concept WeakPattern =
    PatternConvertible<T> && std::same_as<pattern_tag_t<T>, strong_pattern_tag>;

template <PatternConvertible T>
constexpr auto as_pattern(const T& p) {
  return pattern_traits<T>::to_pattern(p);
}

template <Pattern T>
constexpr const T& as_pattern(const T& p) {
  return p;
}

template <typename T>
concept Context = requires {
  typename T::policies;
  typename T::token_type;
};

template<typename T, Context Ctx>
using pattern_value_t = typename T::template value_type<Ctx>;

}  // namespace abu

#endif