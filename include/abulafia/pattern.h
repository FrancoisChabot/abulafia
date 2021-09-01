//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERN_H_INCLUDED
#define ABULAFIA_PATTERN_H_INCLUDED

#include <iterator>

#include "abulafia/data_source.h"
#include "abulafia/policies.h"

namespace abu {

// Real patterns have to be implemented by a parser
struct real_pattern_tag {};
struct strong_pattern_tag {};
struct weak_pattern_tag {};

template <typename T>
struct pattern_traits {
  using pattern_category = typename T::pattern_category;

  template <Policies auto policies, DataSource Data>
  using value_type = typename T::template value_type<policies, Data>;
};

template <typename T>
using pattern_category_t = typename pattern_traits<T>::pattern_category;

template <typename T>
concept Pattern = std::is_same_v<pattern_category_t<T>, real_pattern_tag>;

template <typename T, Policies auto policies, DataSource Data>
using parsed_value_t =
    typename pattern_traits<T>::template value_type<policies, Data>;

///////////////////////////

///////////////////////////

template <typename T>
concept PatternConvertible = requires(T x) {
  { pattern_traits<T>::to_pattern(x) } -> Pattern;
};

template <typename T>
concept StrongPattern = PatternConvertible<T> &&
    std::same_as<pattern_category_t<T>, strong_pattern_tag>;

template <typename T>
concept WeakPattern = PatternConvertible<T> &&
    std::same_as<pattern_category_t<T>, strong_pattern_tag>;

///////////////////////////
template <typename T>
concept PatternLike = Pattern<T> || PatternConvertible<T>;

template <PatternConvertible T>
constexpr auto as_pattern(const T& p) {
  return pattern_traits<T>::to_pattern(p);
}

template <Pattern T>
constexpr const T& as_pattern(const T& p) {
  return p;
}

template <typename T, template <typename...> typename U>
concept PatternTemplate = requires(T x) {
  { U(x) } -> std::same_as<T>;
};
}  // namespace abu
#endif