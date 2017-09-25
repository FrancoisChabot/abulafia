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
#include "abulafia/pattern.h"

namespace ABULAFIA_NAMESPACE {

namespace act_ {
  template<typename ACT_T, typename Enable=void>
  struct determine_landing_type;

  // If the action takes no argument, the child will write to nothing
  template<typename ACT_T>
  struct determine_landing_type<ACT_T, 
  enable_if_t<function_traits<ACT_T>::arity == 0>> {
    using type = Nil;
  };

  // The first argument of the action, if present, determines the landing type.
  template<typename ACT_T>
  struct determine_landing_type<ACT_T, 
  enable_if_t<function_traits<ACT_T>::arity != 0>> {
    using type = std::decay_t<callable_argument_t<ACT_T, 0>>;
  };

}
template <typename CHILD_PAT_T, typename ACT_T>
class Action : public Pattern<Action<CHILD_PAT_T, ACT_T>> {
  CHILD_PAT_T pat_;
  ACT_T act_;

 public:
  Action(CHILD_PAT_T pat, ACT_T act) 
    : pat_(std::move(pat))
    , act_(std::move(act)) {}

  PAT_T const& child_pattern() const { return pat_; }
  ACT_T const& action() const { return act_; }
};

template <typename CTX_T, typename DST_T, typename CHILD_PAT_T, typename ACT_T>
class Parser<CTX_T, DST_T, Action<CHILD_PAT_T, ACT_T>>
    : public ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP> {
  using PAT_T = Action<CHILD_PAT_T, ACT_T>;

  using child_ctx_t = CTX_T; //for now, but I suspect this may change.
  using landing_type = typename determine_landing_type<ACT_T>::type;
  using child_parser_t = Parser<child_ctx_t, landing_type, CHILD_PAT_T>;

  child_parser_t child_parser_;
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP>(ctx, dst) 
      {}

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    return parser_.consume(ctx, dst, pat.operand());
  }

};


template <typename CHILD_PAT_T, typename ACT_T, typename RECUR_TAG>
struct pattern_traits<Action<CHILD_PAT_T, ACT_T>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    BACKTRACKS = pattern_traits<CHILD_PAT_T, RECUR_TAG>::BACKTRACKS,
    FAILS_CLEANLY = pattern_traits<CHILD_PAT_T, RECUR_TAG>::FAILS_CLEANLY,
    MAY_NOT_CONSUME = pattern_traits<CHILD_PAT_T, RECUR_TAG>::MAY_NOT_CONSUME,
    PEEKABLE = false,
    ATOMIC = true,
    APPENDS_DST = false, //yish....
    STABLE_APPENDS = false, //yish....
  };
};

template <typename Action<CHILD_PAT_T, ACT_T> typename CTX_T>
struct pat_attr_t<Action<CHILD_PAT_T, ACT_T>, CTX_T> {
  using attr_type = ...;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
