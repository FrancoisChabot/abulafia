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

struct RecurPayload {
  virtual ~RecurPayload() {}
};

struct RecurMemoryPool {
  std::unique_ptr<RecurPayload>* alloc() {
    handles_.emplace_back();
    return &handles_.back();
  }

 private:
  std::deque<std::unique_ptr<RecurPayload>> handles_;
};

// Recur is broken up in two separate types: Recur and RecurUsage, in order
// to break ref-count loops.
template <typename CHILD_PAT_T, typename ATTR_T = Nil>
class Recur : public Pattern<Recur<CHILD_PAT_T, ATTR_T>> {
  std::unique_ptr<RecurPayload>* pat_;

 public:
  using operand_pat_t = CHILD_PAT_T;
  using attr_t = ATTR_T;

  Recur(RecurMemoryPool& pool) : pat_(pool.alloc()) {}
  Recur(Recur const& rhs) = default;
  Recur(Recur&& rhs) = default;

  Recur& operator=(CHILD_PAT_T rhs) {
    *pat_ = std::make_unique<CHILD_PAT_T>(std::move(rhs));
    return *this;
  }

  CHILD_PAT_T const& operand() const {
    return static_cast<CHILD_PAT_T&>(**pat_);
  }

 private:
  template <typename T, typename U>
  friend class WeakRecur;
};

}  // namespace ABULAFIA_NAMESPACE

#define ABU_Recur_define(var, RECUR_TAG, pattern)               \
  struct RECUR_TAG : public ABULAFIA_NAMESPACE ::RecurPayload { \
    using pattern_t = std::decay_t<decltype(pattern)>;          \
    using impl_t = pattern_t;                                   \
    impl_t impl;                                                \
                                                                \
    RECUR_TAG(pattern_t const& p) : impl(p) {}                  \
  };                                                            \
  var = RECUR_TAG(pattern);

#endif
