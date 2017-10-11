//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_WITH_SKIPPER_H_
#define ABULAFIA_WITH_SKIPPER_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/pattern.h"
#include "abulafia/support/type_traits.h"

#include <memory>

namespace ABULAFIA_NAMESPACE {

// Pattern for a signed integer
template <typename CHILD_PAT_T, typename SKIP_T>
class WithSkipper : public Pattern<WithSkipper<CHILD_PAT_T, SKIP_T>> {
  CHILD_PAT_T child_pat_;
  SKIP_T skip_pat_;

 public:
  CHILD_PAT_T const& getChild() const { return child_pat_; }
  SKIP_T const& getSkip() const { return skip_pat_; }

  WithSkipper(CHILD_PAT_T c, SKIP_T s)
      : child_pat_(std::move(c)), skip_pat_(std::move(s)) {}
};

template <typename PAT_T, typename SKIP_T>
auto apply_skipper(PAT_T&& pat, SKIP_T&& skip) {
  return WithSkipper<std::decay_t<PAT_T>, std::decay_t<SKIP_T>>(
      std::forward<PAT_T>(pat), std::forward<SKIP_T>(skip));
}

}  // namespace ABULAFIA_NAMESPACE

#endif
