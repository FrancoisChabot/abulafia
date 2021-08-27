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
#include "abulafia/result.h"
#include "abulafia/token.h"

namespace abu {

// Strict Minimum parser
template <typename T>
struct basic_parser;

template <typename P, std::input_iterator I>
struct parse_value : public std::type_identity<void> {};

template <typename P, std::input_iterator I>
using parse_value_t = typename parse_value<P, I>::type;

template <typename P, std::input_iterator I>
using parse_result_t = result<parse_value_t<P, I>>;

// All patterns are required to be able to operate on forward iterators.
template <typename T>
concept Pattern = requires(archetypes::forward_iterator& b,
                           const archetypes::forward_iterator_sentinel& e,
                           T pat) {
  typename parse_value_t<T, archetypes::forward_iterator>;
  { ::abu::basic_parser<T>::parse(b, e, pat) } -> Result;
};

// Patterns that ALSO work with input iterators
// Todo: this might not be needed as the forward->input wrapper might end up
// universally good.
template <typename T>
concept InputPattern = Pattern<T> &&
    requires(archetypes::input_iterator& b,
             const archetypes::input_iterator_sentinel& e,
             T pat) {
  { ::abu::basic_parser<T>::parse(b, e, pat) } -> Result;
};

using check_result_t = result<void>;

template <typename T>
concept ExplicitelyCheckable =
    requires(T pat,
             archetypes::forward_iterator& b,
             const archetypes::forward_iterator_sentinel& e) {
  { ::abu::basic_parser<T>::check(b, e, pat) } -> std::same_as<check_result_t>;
};

template <typename T>
struct to_pattern;

template <typename T>
concept StrongPattern = !::abu::Pattern<T> && requires(T x) {
  ::abu::to_pattern<T>::is_strong_conversion;
  { ::abu::to_pattern<T>{}(x) } -> Pattern;
};

template <typename T>
concept WeakPattern = !::abu::StrongPattern<T> && requires(T x) {
  { ::abu::to_pattern<T>{}(std::move(x)) } -> Pattern;
};

template <typename T>
concept PatternConvertible = StrongPattern<T> || WeakPattern<T>;

template <typename T>
concept PatternLike = Pattern<T> || PatternConvertible<T>;

template <PatternConvertible T>
constexpr auto as_pattern(const T& p) {
  return to_pattern<T>{}(p);
}

template <Pattern T>
constexpr const T& as_pattern(const T& p) {
  return p;
}

}  // namespace abu

#endif