//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_UNARY_OPTIONAL_H_
#define ABULAFIA_PATTERNS_UNARY_OPTIONAL_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/pattern.h"
#include "abulafia/support/assert.h"

#include <optional>

namespace ABULAFIA_NAMESPACE {

template <typename PAT_T>
class Optional : public Pattern<Optional<PAT_T>> {
  PAT_T child_;

 public:
  Optional(PAT_T child) : child_(std::move(child)) {}

  PAT_T const& operand() const { return child_; }
};


// !pattern
template <typename PAT_T,
          typename Enable = enable_if_t<is_valid_unary_operand<PAT_T>()>>
auto operator-(PAT_T&& pat) {
  return Optional<pattern_t<PAT_T>>(make_pattern(pat));
}

}  // namespace ABULAFIA_NAMESPACE

#endif
