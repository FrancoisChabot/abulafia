//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_ACTION_H_
#define ABULAFIA_PARSERS_COROUTINE_ACTION_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/unary/action.h"

namespace ABULAFIA_NAMESPACE {
namespace act_ {

struct arbitrary {
  template <class T>
  operator T();
};

template <typename BOUND_DST_T>
struct ActionParam {
  BOUND_DST_T bound_dst;
};

template <typename ACT_T, typename DST_T = Nil>
constexpr bool has_incoming_val() {
  return (std::is_invocable<ACT_T, arbitrary>::value &&
          !std::is_invocable<ACT_T, ActionParam<DST_T>>::value) ||
         std::is_invocable<ACT_T, arbitrary, ActionParam<DST_T>>::value;
}

template <typename ACT_T, typename DST_T = Nil>
constexpr bool has_incoming_param() {
  return std::is_invocable<ACT_T, ActionParam<DST_T>>::value ||
         std::is_invocable<ACT_T, arbitrary, ActionParam<DST_T>>::value;
}

// This is a workaround for MSVC, it does not resolve usage of constexpr bool
// functions in enable_if<> correctly.

template <typename ACT_T, typename DST_T = Nil>
struct act_arg_traits {
  enum {
    VAL_ARG = has_incoming_val<ACT_T, DST_T>(),
    PARAM_ARG = has_incoming_param<ACT_T, DST_T>()
  };
};
/*Used primarily to determine if the return type is void or not*/

template <typename ACT_T, typename Enable = void>
struct determine_result_type {
  using type = typename function_traits<ACT_T>::result_type;
};

template <typename ACT_T>
struct determine_result_type<
    ACT_T, std::enable_if_t<act_arg_traits<ACT_T>::PARAM_ARG &&
                            act_arg_traits<ACT_T>::VAL_ARG>> {
  using type = decltype(std::declval<ACT_T>()(arbitrary{}, ActionParam<Nil>()));
};

template <typename ACT_T>
struct determine_result_type<
    ACT_T, std::enable_if_t<act_arg_traits<ACT_T>::PARAM_ARG &&
                            !act_arg_traits<ACT_T>::VAL_ARG>> {
  using type = decltype(std::declval<ACT_T>()(ActionParam<Nil>()));
};

template <typename ACT_T>
constexpr bool returns_void() {
  return std::is_same<typename determine_result_type<ACT_T>::type, void>::value;
}

template <typename ACT_T, typename DST_T, typename Enable = void>
struct determine_landing_type {
  using type = Nil;
};

// The first argument of the action, if present, determines the landing type.
template <typename ACT_T, typename DST_T>
struct determine_landing_type<
    ACT_T, DST_T,
    std::enable_if_t<act_arg_traits<ACT_T>::VAL_ARG &&
                     !act_arg_traits<ACT_T>::PARAM_ARG>> {
  using type = typename function_traits<ACT_T>::template arg<0>::type;
};

// The first argument of the action, if present, determines the landing type.
template <typename ACT_T, typename DST_T>
struct determine_landing_type<
    ACT_T, DST_T,
    std::enable_if_t<act_arg_traits<ACT_T>::VAL_ARG &&
                     act_arg_traits<ACT_T>::PARAM_ARG>> {
  using oper = decltype(&ACT_T::template operator()<ActionParam<DST_T>>);

  using type = typename function_traits<oper>::template arg<0>::type;
};

template <typename ACT_T, typename LAND_T, typename DST_T, typename CTX_T>
void act_dispatch(ACT_T const& act, LAND_T land, DST_T dst, CTX_T ctx) {
  constexpr bool writes = !returns_void<ACT_T>();
  constexpr bool has_val = has_incoming_val<ACT_T>();
  constexpr bool has_param = has_incoming_param<ACT_T>();

  ActionParam<typename CTX_T::bound_dst_t> p{ctx.bound_dst()};

  (void)p;
  (void)land;
  (void)dst;

  if constexpr (writes) {
    if constexpr (has_val) {
      if constexpr (has_param) {
        dst = act(std::move(land), p);
      } else {
        dst = act(std::move(land));
      }
    } else {
      if constexpr (has_param) {
        dst = act(p);
      } else {
        dst = act();
      }
    }
  } else {
    if constexpr (has_val) {
      if constexpr (has_param) {
        act(std::move(land), p);
      } else {
        act(std::move(land));
      }
    } else {
      if constexpr (has_param) {
        act(p);
      } else {
        act();
      }
    }
  }
}

}  // namespace act_

template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T,
          typename ACT_T>
class ActionImpl {
  using PAT_T = Action<CHILD_PAT_T, ACT_T>;

  struct child_req_t {
    enum {
      ATOMIC = false,

      // This is extremely important, since we can succeed even if the child
      // parser fails. The exception to this is if MIN_REP == MAX_REP (except
      // for 0). In which case, failure of the child guarantees failure of the
      // parent.
      FAILS_CLEANLY = REQ_T::FAILS_CLEANLY,

      // Propagate
      CONSUMES_ON_SUCCESS = REQ_T::CONSUMES_ON_SUCCESS
    };
  };

  using child_ctx_t = CTX_T;
  using landing_type_t =
      typename act_::determine_landing_type<ACT_T,
                                            typename CTX_T::bound_dst_t>::type;
  using child_parser_t = Parser<child_ctx_t, wrapped_dst_t<landing_type_t>,
                                child_req_t, CHILD_PAT_T>;

  landing_type_t landing;
  child_parser_t child_parser_;

 public:
  ActionImpl(CTX_T ctx, DST_T, PAT_T const& pat)
      : child_parser_(ctx, wrap_dst(landing), pat.child_pattern()) {}

  Result consume(CTX_T ctx, DST_T dst, PAT_T const& pat) {
    auto status =
        child_parser_.consume(ctx, wrap_dst(landing), pat.child_pattern());
    if (status == Result::SUCCESS) {
      act_::act_dispatch(pat.action(), std::move(landing), dst, ctx);
    }
    return status;
  }
};

template <typename CHILD_PAT_T, typename ACT_T>
struct ParserFactory<Action<CHILD_PAT_T, ACT_T>> {
  using pat_t = Action<CHILD_PAT_T, ACT_T>;

  static constexpr DstBehavior dst_behavior() {
    return act_::returns_void<ACT_T>() ? DstBehavior::IGNORE
                                       : DstBehavior::VALUE;
  }

  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = ActionImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T, ACT_T>;
};
}  // namespace ABULAFIA_NAMESPACE

#endif
