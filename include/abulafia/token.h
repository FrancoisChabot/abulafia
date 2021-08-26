//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_TOKEN_H_INCLUDED
#define ABULAFIA_TOKEN_H_INCLUDED

#include <concepts>

namespace abu {

// While tokens are only required to be regular, it's possible for additional
// requirements
template <typename T>
concept Token = std::regular<T>;

template <typename T, typename TokT>
concept TokenSet = requires {
  Token<TokT>;
  std::predicate<T, TokT>;
};

}  // namespace abu
#endif