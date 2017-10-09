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

template <typename ACT_T>
constexpr bool has_incoming_val() {
  return (std::is_invocable<ACT_T, arbitrary>::value &&
          !std::is_invocable<ACT_T, ActionParam<Nil>>::value) ||
         std::is_invocable<ACT_T, arbitrary, ActionParam<Nil>>::value;
}

template <typename ACT_T>
constexpr bool has_incoming_param() {
  return std::is_invocable<ACT_T, ActionParam<Nil>>::value ||
         std::is_invocable<ACT_T, arbitrary, ActionParam<Nil>>::value;
}

/*Used primarily to determine if the return type is void or not*/

template <typename ACT_T, typename Enable = void>
struct determine_result_type {
  using type = typename function_traits<ACT_T>::result_type;
};

template <typename ACT_T>
struct determine_result_type<ACT_T,
                             std::enable_if_t<has_incoming_param<ACT_T>() &&
                                              has_incoming_val<ACT_T>()>> {
  using type = decltype(std::declval<ACT_T>()(arbitrary{}, ActionParam<Nil>()));
};

template <typename ACT_T>
struct determine_result_type<ACT_T,
                             std::enable_if_t<has_incoming_param<ACT_T>() &&
                                              !has_incoming_val<ACT_T>()>> {
  using type = decltype(std::declval<ACT_T>()(ActionParam<Nil>()));
};

template <typename ACT_T>
constexpr bool returns_void() {
  return std::is_same<typename determine_result_type<ACT_T>::type, void>::value;
}

template <typename ACT_T, typename Enable = void>
struct determine_landing_type {
  using type = Nil;
};

// The first argument of the action, if present, determines the landing type.
template <typename ACT_T>
struct determine_landing_type<ACT_T,
                              std::enable_if_t<has_incoming_val<ACT_T>() &&
                                               !has_incoming_param<ACT_T>()>> {
  using type = typename function_traits<ACT_T>::template arg<0>::type;
};

// The first argument of the action, if present, determines the landing type.
template <typename ACT_T>
struct determine_landing_type<ACT_T,
                              std::enable_if_t<has_incoming_val<ACT_T>() &&
                                               has_incoming_param<ACT_T>()>> {
  using oper = decltype(&ACT_T::template operator()<ActionParam<Nil>>);

  using type = typename function_traits<oper>::template arg<0>::type;
};

template <typename ACT_T, typename Enable = void>
struct Dispatch;

template <typename ACT_T>
struct Dispatch<
    ACT_T, enable_if_t<!returns_void<ACT_T>() && !has_incoming_val<ACT_T>() &&
                       !has_incoming_param<ACT_T>()>> {
  template <typename LAND_T, typename DST_T, typename CTX_T>
  static void dispatch(ACT_T const& act, LAND_T, DST_T dst, CTX_T) {
    dst = act();
  }
};

template <typename ACT_T>
struct Dispatch<
    ACT_T, enable_if_t<returns_void<ACT_T>() && !has_incoming_val<ACT_T>() &&
                       !has_incoming_param<ACT_T>()>> {
  template <typename LAND_T, typename DST_T, typename CTX_T>
  static void dispatch(ACT_T const& act, LAND_T, DST_T, CTX_T) {
    act();
  }
};

template <typename ACT_T>
struct Dispatch<
    ACT_T, enable_if_t<!returns_void<ACT_T>() && has_incoming_val<ACT_T>() &&
                       !has_incoming_param<ACT_T>()>> {
  template <typename LAND_T, typename DST_T, typename CTX_T>
  static void dispatch(ACT_T const& act, LAND_T land, DST_T dst, CTX_T) {
    dst = act(std::move(land));
  }
};

template <typename ACT_T>
struct Dispatch<
    ACT_T, enable_if_t<returns_void<ACT_T>() && has_incoming_val<ACT_T>() &&
                       !has_incoming_param<ACT_T>()>> {
  template <typename LAND_T, typename DST_T, typename CTX_T>
  static void dispatch(ACT_T const& act, LAND_T land, DST_T, CTX_T) {
    act(std::move(land));
  }
};

template <typename ACT_T>
struct Dispatch<
    ACT_T, enable_if_t<!returns_void<ACT_T>() && !has_incoming_val<ACT_T>() &&
                       has_incoming_param<ACT_T>()>> {
  template <typename LAND_T, typename DST_T, typename CTX_T>
  static void dispatch(ACT_T const& act, LAND_T, DST_T dst, CTX_T ctx) {
    ActionParam<typename CTX_T::bound_dst_t> p{ctx.bound_dst()};
    dst = act(p);
  }
};

template <typename ACT_T>
struct Dispatch<
    ACT_T, enable_if_t<returns_void<ACT_T>() && !has_incoming_val<ACT_T>() &&
                       has_incoming_param<ACT_T>()>> {
  template <typename LAND_T, typename DST_T, typename CTX_T>
  static void dispatch(ACT_T const& act, LAND_T, DST_T dst, CTX_T ctx) {
    ActionParam<typename CTX_T::bound_dst_t> p{ctx.bound_dst()};
    act(p);
  }
};

template <typename ACT_T>
struct Dispatch<
    ACT_T, enable_if_t<!returns_void<ACT_T>() && has_incoming_val<ACT_T>() &&
                       has_incoming_param<ACT_T>()>> {
  template <typename LAND_T, typename DST_T, typename CTX_T>
  static void dispatch(ACT_T const& act, LAND_T land, DST_T dst, CTX_T ctx) {
    ActionParam<typename CTX_T::bound_dst_t> p{ctx.bound_dst()};
    dst = act(std::move(land), p);
  }
};

template <typename ACT_T>
struct Dispatch<
    ACT_T, enable_if_t<returns_void<ACT_T>() && has_incoming_val<ACT_T>() &&
                       has_incoming_param<ACT_T>()>> {
  template <typename LAND_T, typename DST_T, typename CTX_T>
  static void dispatch(ACT_T const& act, LAND_T land, DST_T, CTX_T ctx) {
    ActionParam<typename CTX_T::bound_dst_t> p{ctx.bound_dst()};
    act(std::move(land), p);
  }
};

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
  using landing_type_t = typename act_::determine_landing_type<ACT_T>::type;
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
      act_::Dispatch<ACT_T>::template dispatch(pat.action(), std::move(landing),
                                               dst, ctx);
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
