//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERN_H_
#define ABULAFIA_PATTERN_H_

#include "abulafia/config.h"

#include "abulafia/result.h"
#include "abulafia/traits.h"

#include <type_traits>

namespace ABULAFIA_NAMESPACE {

struct default_pattern_traits {
  enum {
    // Indicates that DST will not be affected should the pattern fail.
    ATOMIC = false,

    // Indicates that the pattern can cause backtracking of the input stream.
    BACKTRACKS = false,

    // Indicates that should the pattern fail, the data stream will be in the
    // same state as before entering it.
    FAILS_CLEANLY = false,

    // Indicates that result can be established without actually consuming data
    // from
    // the data stream
    PEEKABLE = false,

    // Indicates that the pattern could succeed without consuming any data.
    MAY_NOT_CONSUME = true,

    // Indicates that the pattern incrementaly calls push_back(), or invokes
    // insert(end,B,E)
    // on its target
    APPENDS_DST = false,

    // Indicates that the pattern will never append and fail (useful for some
    // optimizations)
    STABLE_APPENDS = false,
  };
};

template <typename T, typename RECUR_TAG, typename Enable = void>
struct pattern_traits;

template <typename RECUR_TAG, typename... CHILD_PATS>
struct any_pat_traits {
  enum {
    ATOMIC = false,
    BACKTRACKS = false,
    FAILS_CLEANLY = false,
    MAY_NOT_CONSUME = false,
    APPENDS_DST = false,
    PEEKABLE = false,
    STABLE_APPENDS = false
  };
};

template <typename RECUR_TAG, typename T, typename... REST_PATS>
struct any_pat_traits<RECUR_TAG, T, REST_PATS...> {
  enum {
    ATOMIC = pattern_traits<T, RECUR_TAG>::ATOMIC ||
             any_pat_traits<RECUR_TAG, REST_PATS...>::ATOMIC,
    BACKTRACKS = pattern_traits<T, RECUR_TAG>::BACKTRACKS ||
                 any_pat_traits<RECUR_TAG, REST_PATS...>::BACKTRACKS,
    FAILS_CLEANLY = pattern_traits<T, RECUR_TAG>::FAILS_CLEANLY ||
                    any_pat_traits<RECUR_TAG, REST_PATS...>::FAILS_CLEANLY,
    MAY_NOT_CONSUME = pattern_traits<T, RECUR_TAG>::MAY_NOT_CONSUME ||
                      any_pat_traits<RECUR_TAG, REST_PATS...>::MAY_NOT_CONSUME,
    PEEKABLE = pattern_traits<T, RECUR_TAG>::PEEKABLE ||
               any_pat_traits<RECUR_TAG, REST_PATS...>::PEEKABLE,
    APPENDS_DST = pattern_traits<T, RECUR_TAG>::APPENDS_DST ||
                  any_pat_traits<RECUR_TAG, REST_PATS...>::APPENDS_DST,
    STABLE_APPENDS = pattern_traits<T, RECUR_TAG>::STABLE_APPENDS ||
                     any_pat_traits<RECUR_TAG, REST_PATS...>::STABLE_APPENDS,
  };
};

template <typename RECUR_TAG, typename... CHILD_PATS>
struct all_pat_traits {
  enum {
    ATOMIC = true,
    BACKTRACKS = true,
    FAILS_CLEANLY = true,
    MAY_NOT_CONSUME = true,
    PEEKABLE = true,
    APPENDS_DST = true,
    STABLE_APPENDS = true,
  };
};

template <typename RECUR_TAG, typename T, typename... REST_PATS>
struct all_pat_traits<RECUR_TAG, T, REST_PATS...> {
  enum {
    ATOMIC = pattern_traits<T, RECUR_TAG>::ATOMIC &&
             all_pat_traits<RECUR_TAG, REST_PATS...>::ATOMIC,
    BACKTRACKS = pattern_traits<T, RECUR_TAG>::BACKTRACKS &&
                 all_pat_traits<RECUR_TAG, REST_PATS...>::BACKTRACKS,
    FAILS_CLEANLY = pattern_traits<T, RECUR_TAG>::FAILS_CLEANLY &&
                    all_pat_traits<RECUR_TAG, REST_PATS...>::FAILS_CLEANLY,
    MAY_NOT_CONSUME = pattern_traits<T, RECUR_TAG>::MAY_NOT_CONSUME &&
                      all_pat_traits<RECUR_TAG, REST_PATS...>::MAY_NOT_CONSUME,
    PEEKABLE = pattern_traits<T, RECUR_TAG>::PEEKABLE &&
               all_pat_traits<RECUR_TAG, REST_PATS...>::PEEKABLE,
    APPENDS_DST = pattern_traits<T, RECUR_TAG>::APPENDS_DST &&
                  all_pat_traits<RECUR_TAG, REST_PATS...>::APPENDS_DST,
    STABLE_APPENDS = pattern_traits<T, RECUR_TAG>::STABLE_APPENDS &&
                     all_pat_traits<RECUR_TAG, REST_PATS...>::STABLE_APPENDS,
  };
};

struct DefaultState {
  template <typename T, typename U>
  DefaultState(T&, U&) {}
};

// This is usefull so that we can identify what is a pattern using
// std::is_base_of<>
class PatternBase {};

template <typename T>
class Pattern : public PatternBase {
 public:
  using pattern_t = T;

  /*
    template <typename ACT_T>
    auto operator[](ACT_T act) const {
      return Action<T, ACT_T>(*static_cast<T const*>(this), act);
    }
  */
  /*
  template <typename ATTR_T>
  auto as() const {
    return AttrCast<ATTR_T, T>(*static_cast<T const*>(this));
  }
  */
};

template <typename DST_T, typename CTX_T>
struct StateTraits {
  using dst = DST_T;
  using ctx = CTX_T;

  template <typename NEW_DST>
  using cast = StateTraits<NEW_DST, CTX_T>;
};

template <typename PAT_T, typename STATE_TRAITS>
struct choose_state {
  using type = typename decay_t<PAT_T>::template State<STATE_TRAITS>;
};

template <typename PAT_T, typename STATE_TRAITS>
using state_t = typename choose_state<PAT_T, STATE_TRAITS>::type;

template <typename PAT_T, typename CTX_T>
struct pat_attr_t {};

template <typename PAT_T, typename CTX_T>
using attr_t = typename pat_attr_t<decay_t<PAT_T>, CTX_T>::attr_type;

// This extra indirection is here so that attr expectation failures displays
// the found type in the compile error.
template <typename LHS_T, typename RHS_T>
void expect_same() {
  static_assert(is_same<LHS_T, RHS_T>::value, "attr_t expectation failure");
}

// Catch-all set of traits for every subclass of pattern.
template <typename T>
struct expr_traits<
    T, enable_if_t<std::is_base_of<PatternBase, decay_t<T>>::value>> {
  enum { is_pattern = true, converts_to_pattern = false };

  static const T& make_pattern(const T& v) { return v; }
};

template <typename T>
typename std::remove_reference<T>::type& force_lvalue(T&& t) {
  return static_cast<typename std::remove_reference<T>::type&>(t);
}
}  // namespace ABULAFIA_NAMESPACE

#endif
