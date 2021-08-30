//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_TOKEN_H_INCLUDED
#define ABULAFIA_TOKEN_H_INCLUDED

#include <concepts>
#include <type_traits>

#include "abulafia/archetypes.h"

namespace abu {

// Additional requirements can be added by individual patterns.
template <typename T>
concept Token = std::is_copy_constructible_v<T>;

template <typename T>
concept TokenSet = std::predicate<T, archetypes::token>;

struct any_token {
  constexpr bool operator()(const auto&) const { return true; }
};
}  // namespace abu
#endif