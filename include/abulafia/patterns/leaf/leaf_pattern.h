//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_LEAF_LEAF_PATTERN_H_
#define ABULAFIA_PATTERNS_LEAF_LEAF_PATTERN_H_

#include "abulafia/config.h"

#include "abulafia/patterns/pattern.h"
#include "abulafia/support/type_traits.h"

#include <type_traits>
#include <utility>

namespace ABULAFIA_NAMESPACE {
template <typename CRTP>
struct LeafPattern : public Pattern<CRTP> {};

template <
    typename T, typename CB_T,
    typename = std::enable_if_t<is_base_of_template<T, LeafPattern>::value>>
T transform(T const& tgt, CB_T const&) {
  return tgt;
}

}  // namespace ABULAFIA_NAMESPACE

#endif
