//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_UNARY_ACTION_H_
#define ABULAFIA_PATTERNS_UNARY_ACTION_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/pattern.h"
#include "abulafia/support/function_traits.h"

namespace ABULAFIA_NAMESPACE {

template <typename CHILD_PAT_T, typename ACT_T>
class Action : public Pattern<Action<CHILD_PAT_T, ACT_T>> {
  CHILD_PAT_T pat_;
  ACT_T act_;

 public:
  Action(CHILD_PAT_T pat, ACT_T act)
      : pat_(std::move(pat)), act_(std::move(act)) {}

  CHILD_PAT_T const& child_pattern() const { return pat_; }
  ACT_T const& action() const { return act_; }
};

template <typename PAT_T, typename ACT_T>
auto apply_action(PAT_T&& pat, ACT_T&& act) {
  return Action<std::decay_t<PAT_T>, std::decay_t<ACT_T>>(
      std::forward<PAT_T>(pat), std::forward<ACT_T>(act));
}

template <typename CHILD_PAT_T, typename ACT_T, typename CB_T>
auto transform(Action<CHILD_PAT_T, ACT_T> const& tgt, CB_T const& cb) {
  auto new_op = cb(tgt.child_pattern());

  return Action<decltype(new_op), ACT_T>(std::move(new_op), tgt.action());
}

}  // namespace ABULAFIA_NAMESPACE

#endif
