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

namespace act_ {
template <typename ACT_T, typename Enable = void>
struct determine_landing_type;

// If the action takes no argument, the child will write to nothing
template <typename ACT_T>
struct determine_landing_type<ACT_T,
                              enable_if_t<function_traits<ACT_T>::arity == 0>> {
  using type = Nil;
};

// The first argument of the action, if present, determines the landing type.
template <typename ACT_T>
struct determine_landing_type<ACT_T,
                              enable_if_t<function_traits<ACT_T>::arity != 0>> {
  using type = std::decay_t<callable_argument_t<ACT_T, 0>>;
};

template <typename ACT_T, typename Enable = void>
struct determine_emmited_type;

template <typename ACT_T>
struct determine_emmited_type<
    ACT_T, enable_if_t<is_same<void, callable_result_t<ACT_T>>::value>> {
  using type = Nil;
};

template <typename ACT_T>
struct determine_emmited_type<
    ACT_T, enable_if_t<!is_same<void, callable_result_t<ACT_T>>::value>> {
  using type = callable_result_t<ACT_T>;
};

template <typename ACT_T, typename Enable = void>
struct Dispatch;

template <typename ACT_T>
struct Dispatch<ACT_T,
                enable_if_t<function_traits<ACT_T>::arity == 0 &&
                            is_same<void, callable_result_t<ACT_T>>::value>> {
  template <typename LAND_T, typename DST_T>
  static void dispatch(ACT_T const& act, LAND_T, DST_T&) {
    act();
  }
};

template <typename ACT_T>
struct Dispatch<ACT_T,
                enable_if_t<function_traits<ACT_T>::arity != 0 &&
                            is_same<void, callable_result_t<ACT_T>>::value>> {
  template <typename LAND_T, typename DST_T>
  static void dispatch(ACT_T const& act, LAND_T land, DST_T&) {
    act(std::move(land));
  }
};

template <typename ACT_T>
struct Dispatch<ACT_T,
                enable_if_t<function_traits<ACT_T>::arity == 0 &&
                            !is_same<void, callable_result_t<ACT_T>>::value>> {
  template <typename LAND_T, typename DST_T>
  static void dispatch(ACT_T const& act, LAND_T, DST_T& dst) {
    dst = act();
  }
};

template <typename ACT_T>
struct Dispatch<ACT_T,
                enable_if_t<function_traits<ACT_T>::arity != 0 &&
                            !is_same<void, callable_result_t<ACT_T>>::value>> {
  template <typename LAND_T, typename DST_T>
  static void dispatch(ACT_T const& act, LAND_T land, DST_T& dst) {
    dst = act(std::move(land));
  }
};
}  // namespace act_
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

}  // namespace ABULAFIA_NAMESPACE

#endif
