//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_UNARY_H_INCLUDED
#define ABULAFIA_PATTERNS_UNARY_H_INCLUDED

#include <string>

#include "abulafia/pattern.h"
#include "abulafia/token.h"

namespace abu::pat {
// ############# Misc ############# //

// ***** tok *****
template <TokenSet TokSetT>
struct tok {
  [[no_unique_address]] TokSetT allowed;

  // traits
  using pattern_category = real_pattern_tag;

  static constexpr bool fails_cleanly = true;

  template <Token Tok, Policies auto policies>
  using value_type = Tok;
};

// ***** repeat *****
template <Pattern Op>
struct repeat {
  [[no_unique_address]] Op operand;
  std::size_t min;
  std::size_t max;

  // traits
  using pattern_category = real_pattern_tag;

  static constexpr bool fails_cleanly = true;

  template <Token Tok, Policies auto policies>
  using value_type = std::basic_string<char>;
};

// ***** discard *****
template <Pattern Op>
struct discard {
  [[no_unique_address]] Op operand;

  using pattern_category = real_pattern_tag;

  static constexpr bool fails_cleanly = Op::fails_cleanly;

  template <Token Tok, Policies auto policies>
  using value_type = void;
};

/*

// ***** End of input *****
struct eoi_tag {};
struct fail_tag {};
struct pass_tag {};




// ***** Always fails *****
struct fail {
  using pattern_tag = fail;
};

// ***** Always Passes *****
struct pass {
  using pattern_tag = pass;
};

// ############# Terminal ############# //

// ***** Single token from token set*****
template <TokenSet TokSetT>
struct tok {
  using pattern_tag = real_pattern_tag;
  using token_set_type = TokSetT;

  template <DataContext Ctx>
  using parsed_value_type = typename Ctx::token_type;

  [[no_unique_address]] token_set_type allowed;
};

// ***** Token sequence literal *****
template <std::ranges::range SeqT>
struct lit_seq {
  using pattern_tag = real_pattern_tag;

  template <DataContext>
  using parsed_value_type = void;

  [[no_unique_address]] SeqT expected;
};

// ############# Unary ############# //
// ***** raw *****
template <Pattern Op>
struct raw {
  using pattern_tag = real_pattern_tag;
  using operand_type = Op;

  template <DataContext Ctx>
  using parsed_value_type = std::basic_string<typename Ctx::token_type>;

  [[no_unique_address]] operand_type operand;
};



// ***** optional *****
namespace opt_ {

template <typename T>
struct value : public std::type_identity<std::optional<T>> {};

template <typename T>
struct value<std::optional<T>> : public std::type_identity<std::optional<T>> {};

template <>
struct value<void> : public std::type_identity<void> {};

template <typename T>
using value_t = typename value<T>::type;

}  // namespace opt_
template <Pattern Op>
struct optional {
  using pattern_tag = real_pattern_tag;
  using operand_type = Op;

  template <DataContext Ctx>
  using parsed_value_type = opt_::value_t<parsed_value_ctx_t<operand_type,
Ctx>>;

  [[no_unique_address]] operand_type operand;
};


// ***** action *****
template <Pattern Op, typename Act>
struct action {
  using pattern_tag = real_pattern_tag;
  using operand_type = Op;
  using action_type = Act;

  [[no_unique_address]] operand_type operand;
  [[no_unique_address]] action_type action;
};

// ############# Binary ############# //

// ***** list *****
template <Pattern Op, Pattern Delim>
struct list {
  using pattern_tag = real_pattern_tag;
  using operand_type = Op;
  using delimiter_type = Delim;

  template <DataContext Ctx>
  using parsed_value_type = std::vector<parsed_value_ctx_t<operand_type, Ctx>>;

  [[no_unique_address]] operand_type operand;
  [[no_unique_address]] delimiter_type modifier;
};

// ***** except *****
template <Pattern Op, Pattern Except>
struct except {
  using pattern_tag = real_pattern_tag;
  using operand_type = Op;
  using except_type = Except;

  template <DataContext Ctx>
  using parsed_value_type = parsed_value_ctx_t<operand_type, Ctx>;

  [[no_unique_address]] operand_type operand;
  [[no_unique_address]] except_type except;
};

// ############# Nary ############# //
// ***** seq *****
template <Pattern... Ops>
struct seq {
  using pattern_tag = real_pattern_tag;
  using operands_type = std::tuple<Ops...>;

  [[no_unique_address]] operands_type operands;
};

// ***** alt *****
template <Pattern... Ops>
struct alt {
  using pattern_tag = real_pattern_tag;
  using operands_type = std::tuple<Ops...>;

  [[no_unique_address]] operands_type operands;
};
*/
}  // namespace abu::pat

#endif