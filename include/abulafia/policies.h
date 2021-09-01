//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_POLICIES_H_INCLUDED
#define ABULAFIA_POLICIES_H_INCLUDED

#include <concepts>
#include <vector>

namespace abu {

template <typename T>
concept Policies = requires {
  { T::vector_of_tokens_are_strings } -> std::convertible_to<bool>;
};

struct default_policies_type {
  static constexpr bool vector_of_tokens_are_strings = true;
};

static constexpr default_policies_type default_policies{};

}  // namespace abu
#endif