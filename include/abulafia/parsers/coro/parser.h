//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_PARSER_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_PARSER_H_INCLUDED

#include "abulafia/data_source.h"
#include "abulafia/op_result.h"
#include "abulafia/pattern.h"
#include "abulafia/policies.h"
#include "abulafia/utils.h"

namespace abu::coro {

template <Pattern auto pattern, Policies auto policies, DataSource Data>
class parser;

template <Pattern auto pattern, Policies auto policies, DataSource Data>
class matcher : public parser<pattern, policies, Data> {
  using parser<pattern, policies, Data>::parser;

 public:
  constexpr op_result on_tokens(Data& data) {
    return parser<pattern, policies, Data>::on_tokens(data, details_::noop);
  }

  constexpr op_result on_end(Data& data) {
    return parser<pattern, policies, Data>::on_end(data, details_::noop);
  }
};

namespace details_ {

template <typename T>
struct op_pattern_lookup;

template <Pattern auto pattern, Policies auto policies, DataSource Data>
struct op_pattern_lookup<parser<pattern, policies, Data>> {
  static constexpr auto value = pattern;
};

template <Pattern auto pattern, Policies auto policies, DataSource Data>
struct op_pattern_lookup<matcher<pattern, policies, Data>> {
  static constexpr auto value = pattern;
};

}  // namespace details_

template <typename T>
inline constexpr auto op_pattern = details_::op_pattern_lookup<T>::value;

template <typename T, auto pattern, typename Tok, auto policies>
concept ParseCallback = Token<Tok> &&
    requires(T x, parsed_value_t<decltype(pattern), Tok, policies>&& val) {
  { x(std::move(val)) } -> std::same_as<void>;
};

namespace details_ {
  template <typename Parser, Token Tok>
  struct parse_callback_archetype {
    static constexpr default_policies_type policies = default_policies;
    using value_t = parsed_value_t<decltype(op_pattern<Parser>), Tok, policies>;
    void operator()(value_t&&);
  };
}  // namespace details_

template <typename T, typename Data>
concept Parser =
    requires(T x,
             Data& d,
             const Data& cd,
             const details_::parse_callback_archetype<T, Data>& cb) {
  T(cd);
  { x.on_tokens(d, cb) } -> std::same_as<op_result>;
  { x.on_end(cb) } -> std::same_as<op_result>;
};

template <typename T, typename Data>
concept Matcher = requires(T x, Data& d, const Data& cd) {
  T(cd);
  { x.on_tokens(d) } -> std::same_as<op_result>;
  { x.on_end(d) } -> std::same_as<op_result>;
};
}  // namespace abu::coro

#endif