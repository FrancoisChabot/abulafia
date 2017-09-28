//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_BINARY_LIST_H_
#define ABULAFIA_PATTERNS_BINARY_LIST_H_

#include "abulafia/config.h"

#include "abulafia/pattern.h"
#include "abulafia/patterns/helpers/buffer.h"
#include "abulafia/patterns/unary/repeat.h"
#include "abulafia/support/assert.h"
#include "abulafia/support/nil.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {

// List(a, b) -> a >> *( ignore(b) >> a )
template <typename VAL_PAT_T, typename SEP_PAT_T>
class List : public Pattern<List<VAL_PAT_T, SEP_PAT_T>> {
 public:
  using val_pat_t = VAL_PAT_T;
  using sep_pat_t = SEP_PAT_T;

  List(val_pat_t val_pat, sep_pat_t sep)
      : val_(std::move(val_pat)), sep_(std::move(sep)) {}

  val_pat_t const& op() const { return val_; }
  sep_pat_t const& sep() const { return sep_; }

 private:
  VAL_PAT_T val_;
  SEP_PAT_T sep_;
};

template <typename LHS_T, typename RHS_T, typename RECUR_TAG>
struct pattern_traits<List<LHS_T, RHS_T>, RECUR_TAG> {
  enum {
    BACKTRACKS = pattern_traits<LHS_T, RECUR_TAG>::BACKTRACKS ||
                 pattern_traits<RHS_T, RECUR_TAG>::BACKTRACKS ||
                 !pattern_traits<RHS_T, RECUR_TAG>::FAILS_CLEANLY ||
                 !pattern_traits<LHS_T, RECUR_TAG>::FAILS_CLEANLY,
    FAILS_CLEANLY = false,  // TODO: not 100% correct.
    MAY_NOT_CONSUME = pattern_traits<LHS_T, RECUR_TAG>::MAY_NOT_CONSUME,
    ATOMIC = false,
    APPENDS_DST = true,
    STABLE_APPENDS = true,
  };
};

template <typename LHS_T, typename RHS_T, typename CTX_T>
struct pat_attr_t<List<LHS_T, RHS_T>, CTX_T> {
  using attr_type = typename std::conditional<
      std::is_same<Nil, abu::attr_t<LHS_T, CTX_T>>::value, Nil,
      std::vector<abu::attr_t<LHS_T, CTX_T>>>::type;
};

template <typename LHS_T, typename RHS_T, typename CB_T>
auto transform(List<LHS_T, RHS_T> const& tgt, CB_T const& cb) {
  auto new_op = cb(tgt.operand());
  auto new_sep = cb(tgt.separator());

  using new_op_t = decltype(new_op);
  using new_sep_t = decltype(new_sep);

  return List<new_op_t, new_sep_t>(std::move(new_op), std::move(new_sep));
}

}  // namespace ABULAFIA_NAMESPACE

#endif
