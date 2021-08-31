//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_CONTEXT_H_INCLUDED
#define ABULAFIA_CONTEXT_H_INCLUDED

#include "abulafia/policy.h"
#include "abulafia/token.h"

namespace abu {

enum class op_type {
  parse,
  match,

  // Not an actual op type
  any,
};

template <typename T>
concept DataContext = requires {
  typename T::token_type;
  typename T::policies;
};

template <Token Tok, Policy Pol>
struct data_context {
  using token_type = Tok;
  using policies = Pol;
};

}  // namespace abu

#endif