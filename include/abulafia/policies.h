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
concept Policies = requires(T x) {
  { x.repeat.tokens_to_string } -> std::convertible_to<bool>;
};

struct default_policies_type {
  struct {
    bool tokens_to_string = true;
  } repeat;
};

static constexpr default_policies_type default_policies{};

// These are hacks to handle a bug in msvc 19.29. It's fixed in 19.30, and
// the hackls should/will be removed once it releases.

namespace pol {
constexpr bool repeat_tokens_to_string(Policies auto policy) {
  return policy.repeat.tokens_to_string;
}
}  // namespace pol

}  // namespace abu
#endif