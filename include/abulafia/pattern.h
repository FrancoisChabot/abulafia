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
#include "abulafia/context.h"
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

// All patterns are required to be able to operate on forward iterators.
template <typename T>
concept Pattern =
    std::same_as<typename pattern_traits<T>::pattern_tag, real_pattern_tag>;

///////////////////////////
template <typename T>
using pattern_tag_t = typename pattern_traits<T>::pattern_tag;

template <typename T, DataContext Ctx>
using parsed_value_ctx_t = typename T::template parsed_value_type<Ctx>;

template <typename T, Token Tok, Policy Pol=default_policy>
using parsed_value_t = parsed_value_ctx_t<T, data_context<Tok, Pol>>;
///////////////////////////

template <typename T>
concept PatternConvertible = requires(T x) {
  { pattern_traits<T>::to_pattern(x) } -> Pattern;
};

template <typename T>
concept StrongPattern =
    PatternConvertible<T> && std::same_as<pattern_tag_t<T>, strong_pattern_tag>;

template <typename T>
concept WeakPattern =
    PatternConvertible<T> && std::same_as<pattern_tag_t<T>, strong_pattern_tag>;

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

template <typename T>
concept OpContext =
    DataContext<T> && Pattern<typename T::pattern_type> && requires {
  { T::operation_type } -> std::same_as<const op_type>;
};

template <typename T>
concept MatchContext = OpContext<T> &&(T::operation_type == op_type::match);

template <typename T>
concept ParseContext =
    OpContext<T> &&(T::operation_type == op_type::parse) && requires {
  typename T::value_type;
};

template <Token Tok, Policy Pol, Pattern Pat, op_type OpType>
struct op_context;

template <Token Tok, Policy Pol, Pattern Pat>
struct op_context<Tok, Pol, Pat, op_type::parse> {
  static constexpr op_type operation_type = op_type::parse;
  using token_type = Tok;
  using policies = Pol;
  using pattern_type = Pat;
  using value_type = parsed_value_t<Pat, token_type, policies>;
};

template <Token Tok, Policy Pol, Pattern Pat>
struct op_context<Tok, Pol, Pat, op_type::match> {
  static constexpr op_type operation_type = op_type::match;
  using token_type = Tok;
  using policies = Pol;
  using pattern_type = Pat;
};

///////////////////////////
template <typename T, template <typename... U> typename PatTmpl>
concept PatternContext = DataContext<T> && Pattern<typename T::pattern_type> &&
    requires(typename T::pattern_type x) {
  { PatTmpl(x) } -> std::same_as<typename T::pattern_type>;
};

}  // namespace abu

#endif