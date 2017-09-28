//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_RECUR_H_
#define ABULAFIA_RECUR_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/pattern.h"
#include "abulafia/support/type_traits.h"

#include <memory>

namespace ABULAFIA_NAMESPACE {

// Recur is broken up in two separate types: Recur and RecurUsage, in order
// to break ref-count loops.
template <typename CHILD_PAT_T, typename ATTR_T = Nil>
class Recur : public Pattern<Recur<CHILD_PAT_T, ATTR_T>> {
  std::shared_ptr<std::unique_ptr<CHILD_PAT_T>> pat_;

 public:
  using operand_pat_t = CHILD_PAT_T;
  using attr_t = ATTR_T;

  Recur() : pat_(std::make_shared<std::unique_ptr<CHILD_PAT_T>>()) {}
  Recur(Recur const& rhs) = default;
  Recur(Recur&& rhs) = default;

  Recur& operator=(CHILD_PAT_T rhs) {
    *pat_ = std::make_unique<CHILD_PAT_T>(std::move(rhs));
    return *this;
  }

  CHILD_PAT_T const& operand() const { return **pat_; }

 private:
  template <typename T, typename U>
  friend class WeakRecur;
};

template <typename CHILD_PAT_T, typename ATTR_T = Nil>
class WeakRecur : public Pattern<WeakRecur<CHILD_PAT_T, ATTR_T>> {
  std::unique_ptr<CHILD_PAT_T>* pat_;

 public:
  using operand_pat_t = CHILD_PAT_T;

  WeakRecur(Recur<CHILD_PAT_T, ATTR_T> const& r) : pat_(r.pat_.get()) {}

  WeakRecur(WeakRecur const& rhs) = default;
  WeakRecur(WeakRecur&& rhs) = default;

  CHILD_PAT_T const& operand() const { return **pat_; }
};

template <typename CHILD_PAT_T, typename ATTR_T>
struct RecurWeakener {
  template <typename T>
  auto operator()(T const& rhs) const {
    return transform(rhs, *this);
  }

  auto operator()(Recur<CHILD_PAT_T, ATTR_T> const& tgt_recur) const {
    return WeakRecur<CHILD_PAT_T, ATTR_T>(tgt_recur);
  }
};

template <typename RECUR_T, typename PAT_T>
auto weaken_recur(PAT_T const& pat) {
  using CHILD_PAT_T = typename RECUR_T::operand_pat_t;
  using ATTR_T = typename RECUR_T::attr_t;
  RecurWeakener<CHILD_PAT_T, ATTR_T> transformation;

  return transform(pat, transformation);
}

template <typename CTX_T, typename DST_T, typename CHILD_PAT_T, typename ATTR_T>
class Parser<CTX_T, DST_T, Recur<CHILD_PAT_T, ATTR_T>>
    : public ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP> {
  using PAT_T = Recur<CHILD_PAT_T, ATTR_T>;
  using operand_pat_t = typename PAT_T::operand_pat_t;
  using operand_parser_t = Parser<CTX_T, DST_T, operand_pat_t>;

  std::unique_ptr<operand_parser_t> child_parser_;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const&)
      : ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP>(ctx, dst) {
    // We do not create the child parser here, since this is a recursive
    // process.
  }

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (!child_parser_) {
      child_parser_ =
          std::make_unique<operand_parser_t>(ctx, dst, pat.operand());
    }
    return child_parser_->consume(ctx, dst, pat.operand());
  }
};

template <typename CTX_T, typename DST_T, typename CHILD_PAT_T, typename ATTR_T>
class Parser<CTX_T, DST_T, WeakRecur<CHILD_PAT_T, ATTR_T>>
    : public ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP> {
  using PAT_T = WeakRecur<CHILD_PAT_T, ATTR_T>;
  using operand_pat_t = typename PAT_T::operand_pat_t;
  using operand_parser_t = Parser<CTX_T, DST_T, operand_pat_t>;

  std::unique_ptr<operand_parser_t> child_parser_;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const&)
      : ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP>(ctx, dst) {
    // We do not create the child parser here, since this is a recursive
    // process.
  }

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (!child_parser_) {
      child_parser_ =
          std::make_unique<operand_parser_t>(ctx, dst, pat.operand());
    }
    return child_parser_->consume(ctx, dst, pat.operand());
  }
};

template <typename PAT_T, typename ATTR_T, typename CTX_T>
struct pat_attr_t<Recur<PAT_T, ATTR_T>, CTX_T> {
  using attr_type = ATTR_T;
};

template <typename PAT_T, typename ATTR_T, typename CTX_T>
struct pat_attr_t<WeakRecur<PAT_T, ATTR_T>, CTX_T> {
  using attr_type = ATTR_T;
};

// Main entry point of attribute resolution for recursive patterns.
// A straight propagation of the recur's assigned traits
template <typename PAT_T, typename ATTR_T>
struct pattern_traits<Recur<PAT_T, ATTR_T>, void>
    : public default_pattern_traits {
  using sub_traits =
      pattern_traits<typename PAT_T::pattern_t, std::tuple<PAT_T>>;

  enum {
    ATOMIC = sub_traits::ATOMIC,
    BACKTRACKS = sub_traits::BACKTRACKS,
    FAILS_CLEANLY = sub_traits::FAILS_CLEANLY,
    MAY_NOT_CONSUME = sub_traits::MAY_NOT_CONSUME,
    PEEKABLE = sub_traits::PEEKABLE,
    APPENDS_DST = sub_traits::APPENDS_DST,
    STABLE_APPENDS = sub_traits::STABLE_APPENDS
  };
};

template <typename PAT_T, typename ATTR_T>
struct pattern_traits<WeakRecur<PAT_T, ATTR_T>, void>
    : public default_pattern_traits {
  using sub_traits =
      pattern_traits<typename PAT_T::pattern_t, std::tuple<PAT_T>>;

  enum {
    ATOMIC = sub_traits::ATOMIC,
    BACKTRACKS = sub_traits::BACKTRACKS,
    FAILS_CLEANLY = sub_traits::FAILS_CLEANLY,
    MAY_NOT_CONSUME = sub_traits::MAY_NOT_CONSUME,
    PEEKABLE = sub_traits::PEEKABLE,
    APPENDS_DST = sub_traits::APPENDS_DST,
    STABLE_APPENDS = sub_traits::STABLE_APPENDS
  };
};

// It gets convoluted. If we are in the process of determining the traits of a
// recursive pattern. We will catch this by the TAG being a std::tuple<>

// This handles "simple" cases, where a recursive rule depends on itself without
// having another recursive rule in between. This is determined by the front of
// the TAG tuple being PAT_T

// In that scenario, we choose optimistic values that may be overriden by the
// parent evaluator
template <typename PAT_T, typename ATTR_T, typename... TYPES>
struct pattern_traits<
    Recur<PAT_T, ATTR_T>, std::tuple<TYPES...>,
    enable_if_t<
        is_same<typename std::tuple_element<0, std::tuple<TYPES...>>::type,
                PAT_T>::value>> : public default_pattern_traits {
  // These are provisional flags, only used for the first pass
  enum {
    ATOMIC = true,
    BACKTRACKS = false,
    FAILS_CLEANLY = true,
    MAY_NOT_CONSUME = false,
    PEEKABLE = true,
    APPENDS_DST = false,
    STABLE_APPENDS = false
  };
};

template <typename PAT_T, typename ATTR_T, typename... TYPES>
struct pattern_traits<
    WeakRecur<PAT_T, ATTR_T>, std::tuple<TYPES...>,
    enable_if_t<
        is_same<typename std::tuple_element<0, std::tuple<TYPES...>>::type,
                PAT_T>::value>> : public default_pattern_traits {
  // These are provisional flags, only used for the first pass
  enum {
    ATOMIC = true,
    BACKTRACKS = false,
    FAILS_CLEANLY = true,
    MAY_NOT_CONSUME = false,
    PEEKABLE = true,
    APPENDS_DST = false,
    STABLE_APPENDS = false
  };
};

// If PAT_T is not in the tag tuple, then we have a recur depending on a recur,
// we treat this as if we initially entered the evaluation, however, we keep the
// existing recurs in the tuple so that we can catch cyclical loops.
template <typename PAT_T, typename ATTR_T, typename... TYPES>
struct pattern_traits<Recur<PAT_T, ATTR_T>, std::tuple<TYPES...>,
                      enable_if_t<!is_one_of<PAT_T, TYPES...>::value>>
    : public default_pattern_traits {
  using sub_traits =
      pattern_traits<typename PAT_T::pattern_t, std::tuple<PAT_T, TYPES...>>;

  enum {
    ATOMIC = sub_traits::ATOMIC,
    BACKTRACKS = sub_traits::BACKTRACKS,
    FAILS_CLEANLY = sub_traits::FAILS_CLEANLY,
    MAY_NOT_CONSUME = sub_traits::MAY_NOT_CONSUME,
    PEEKABLE = sub_traits::PEEKABLE,
    APPENDS_DST = sub_traits::APPENDS_DST,
    STABLE_APPENDS = sub_traits::STABLE_APPENDS
  };
};

template <typename PAT_T, typename ATTR_T, typename... TYPES>
struct pattern_traits<WeakRecur<PAT_T, ATTR_T>, std::tuple<TYPES...>,
                      enable_if_t<!is_one_of<PAT_T, TYPES...>::value>>
    : public default_pattern_traits {
  using sub_traits =
      pattern_traits<typename PAT_T::pattern_t, std::tuple<PAT_T, TYPES...>>;

  enum {
    ATOMIC = sub_traits::ATOMIC,
    BACKTRACKS = sub_traits::BACKTRACKS,
    FAILS_CLEANLY = sub_traits::FAILS_CLEANLY,
    MAY_NOT_CONSUME = sub_traits::MAY_NOT_CONSUME,
    PEEKABLE = sub_traits::PEEKABLE,
    APPENDS_DST = sub_traits::APPENDS_DST,
    STABLE_APPENDS = sub_traits::STABLE_APPENDS
  };
};

// MSVS is having trouble figuring this out directly in the enable_if condition,
// so we do the computation in here instead
template <typename PAT_T, typename... TYPES>
struct is_circular_recur_dep {
  enum {
    value = is_one_of<PAT_T, TYPES...>::value &&
            !std::is_same<
                typename std::tuple_element<0, std::tuple<TYPES...>>::type,
                PAT_T>::value
  };
};

// Finally, this is trouble. If we get here, it means we have a scenario where
// pattern A depends on pattern B that depends on pattern A. In that scenario,
// we can't recurse down, and we have to be pessimistic about our initial guess
// of the traits (unfortunately).
template <typename PAT_T, typename ATTR_T, typename... TYPES>
struct pattern_traits<
    Recur<PAT_T, ATTR_T>, std::tuple<TYPES...>,
    enable_if_t<is_circular_recur_dep<PAT_T, TYPES...>::value>>
    : public default_pattern_traits {
  enum {
    ATOMIC = false,
    BACKTRACKS = true,
    FAILS_CLEANLY = false,
    MAY_NOT_CONSUME =
        false,  // a bit of a leap of fate here... we may have to enforce that
    PEEKABLE = false,
    APPENDS_DST = false,
    STABLE_APPENDS = false
  };
};

template <typename PAT_T, typename ATTR_T, typename... TYPES>
struct pattern_traits<
    WeakRecur<PAT_T, ATTR_T>, std::tuple<TYPES...>,
    enable_if_t<is_circular_recur_dep<PAT_T, TYPES...>::value>>
    : public default_pattern_traits {
  enum {
    ATOMIC = false,
    BACKTRACKS = true,
    FAILS_CLEANLY = false,
    MAY_NOT_CONSUME =
        false,  // a bit of a leap of fate here... we may have to enforce that
    PEEKABLE = false,
    APPENDS_DST = false,
    STABLE_APPENDS = false
  };
};

}  // namespace ABULAFIA_NAMESPACE

#define ABU_Recur_define(var, RECUR_TAG, pattern)                             \
  using abu_##RECUR_TAG##_recur_t = decltype(var);                            \
  using abu_##RECUR_TAG##_weakened_t = decltype(                              \
      ABULAFIA_NAMESPACE ::weaken_recur<abu_##RECUR_TAG##_recur_t>(pattern)); \
  struct RECUR_TAG : public abu_##RECUR_TAG##_weakened_t {                    \
    using pattern_t = abu_##RECUR_TAG##_weakened_t;                           \
    RECUR_TAG(decltype(pattern) const & p)                                    \
        : pattern_t(                                                          \
              ABULAFIA_NAMESPACE ::weaken_recur<abu_##RECUR_TAG##_recur_t>(   \
                  p)) {}                                                      \
  };                                                                          \
  var = RECUR_TAG(pattern);

#endif
