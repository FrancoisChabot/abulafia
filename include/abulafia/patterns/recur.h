//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_RECUR_H_
#define ABULAFIA_RECUR_H_

#include "abulafia/config.h"

#include "abulafia/patterns/pattern.h"
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

template <typename TGT_RECUR_T>
struct RecurWeakener {
  template <typename T>
  auto operator()(T const& rhs) const {
    return transform(rhs, *this);
  }

  auto operator()(TGT_RECUR_T const& tgt_recur) const {
    using CHILD_PAT_T = typename TGT_RECUR_T::operand_pat_t;
    using ATTR_T = typename TGT_RECUR_T::attr_t;
    return WeakRecur<CHILD_PAT_T, ATTR_T>(tgt_recur);
  }
};

template <typename RECUR_T, typename PAT_T>
auto weaken_recur(PAT_T const& pat) {
  RecurWeakener<RECUR_T> transformation;
  return transform(pat, transformation);
}

}  // namespace ABULAFIA_NAMESPACE

#define ABU_Recur_define(var, RECUR_TAG, pattern)                           \
  struct RECUR_TAG {                                                        \
    using pattern_t = decltype(pattern);                                    \
    using abu_recur_t = decltype(var);                                      \
    using impl_t = decltype(ABULAFIA_NAMESPACE ::weaken_recur<abu_recur_t>( \
        std::declval<pattern_t>()));                                        \
    impl_t impl;                                                            \
                                                                            \
    RECUR_TAG(pattern_t const& p)                                           \
        : impl(ABULAFIA_NAMESPACE ::weaken_recur<abu_recur_t>(p)) {}        \
  };                                                                        \
  var = RECUR_TAG(pattern);

#endif
