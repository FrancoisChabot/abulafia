//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_BINARY_EXCEPT_H_
#define ABULAFIA_PATTERNS_BINARY_EXCEPT_H_

#include "abulafia/config.h"

#include "abulafia/pattern.h"

namespace ABULAFIA_NAMESPACE {

template <typename OP_T, typename NEG_T>
class Except : public Pattern<Except<OP_T, NEG_T>> {
  OP_T op_;
  NEG_T neg_;

 public:
  using op_t = OP_T;
  using neg_t = NEG_T;

  Except(op_t op_p, neg_t neg_p)
      : op_(std::move(op_p)), neg_(std::move(neg_p)) {}

  op_t const& op() const { return op_; }
  neg_t const& neg() const { return neg_; }
};

template <typename LHS_T, typename RHS_T, typename RECUR_TAG>
struct pattern_traits<Except<LHS_T, RHS_T>, RECUR_TAG> {
  enum {
    ATOMIC = pattern_traits<LHS_T, RECUR_TAG>::ATOMIC,
    BACKTRACKS = pattern_traits<LHS_T, RECUR_TAG>::BACKTRACKS ||
                 pattern_traits<RHS_T, RECUR_TAG>::BACKTRACKS ||
                 !pattern_traits<RHS_T, RECUR_TAG>::FAILS_CLEANLY,
    FAILS_CLEANLY = pattern_traits<LHS_T, RECUR_TAG>::FAILS_CLEANLY,
    PEEKABLE = pattern_traits<LHS_T, RECUR_TAG>::PEEKABLE &&
               pattern_traits<RHS_T, RECUR_TAG>::PEEKABLE,
    MAY_NOT_CONSUME = pattern_traits<LHS_T, RECUR_TAG>::MAY_NOT_CONSUME,
    APPENDS_DST = pattern_traits<LHS_T, RECUR_TAG>::APPENDS_DST,
    STABLE_APPENDS = pattern_traits<LHS_T, RECUR_TAG>::STABLE_APPENDS,
  };
};

template <typename LHS_T, typename RHS_T, typename CTX_T>
struct pat_attr_t<Except<LHS_T, RHS_T>, CTX_T> {
  using attr_type = abu::attr_t<LHS_T, CTX_T>;
};

template <typename LHS_T, typename RHS_T, typename CB_T>
auto transform(Except<LHS_T, RHS_T> const& tgt, CB_T const& cb) {
  auto new_op = cb(tgt.op());
  auto new_neg = cb(tgt.neg());

  using new_op_t = decltype(new_op);
  using new_sep_t = decltype(new_neg);

  return Except<new_op_t, new_sep_t>(std::move(new_op), std::move(new_neg));
}

}  // namespace ABULAFIA_NAMESPACE

#endif
