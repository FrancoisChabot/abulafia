//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_UNARY_CONSTRUCT_H_
#define ABULAFIA_PATTERNS_UNARY_CONSTRUCT_H_

#include "abulafia/config.h"

#include "abulafia/patterns/pattern.h"
#include "abulafia/support/function_traits.h"

namespace ABULAFIA_NAMESPACE {

template <typename CHILD_PAT_T, typename... ARGS_T>
class Construct : public Pattern<Construct<CHILD_PAT_T, ARGS_T...>> {
  CHILD_PAT_T pat_;

 public:
  Construct(CHILD_PAT_T pat) : pat_(std::move(pat)) {}

  CHILD_PAT_T const& child_pattern() const { return pat_; }
};

// !pattern
template <typename... ARGS_T, typename CHILD_PAT_T>
auto construct(CHILD_PAT_T pat) {
  return Construct<CHILD_PAT_T, ARGS_T...>(std::move(pat));
}

}  // namespace ABULAFIA_NAMESPACE

#endif
