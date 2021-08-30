#//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_ARCHETYPES_H_INCLUDED
#define ABULAFIA_ARCHETYPES_H_INCLUDED

#include "abulafia/stdlib_utils.h"

namespace abu {
namespace archetypes {

struct token {
  template <typename T>
  operator T() const noexcept;
};

struct value {
  template <typename T>
  operator T() const noexcept;
};

using input_iterator = std_utils::archetypes::input_iterator<char>;
using input_iterator_sentinel = std_utils::archetypes::input_iterator_sentinel;

using forward_iterator = std_utils::archetypes::forward_iterator<char>;
using forward_iterator_sentinel =
    std_utils::archetypes::forward_iterator_sentinel;

}  // namespace archetypes

}  // namespace abu

#endif