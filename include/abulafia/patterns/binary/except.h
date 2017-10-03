//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_BINARY_EXCEPT_H_
#define ABULAFIA_PATTERNS_BINARY_EXCEPT_H_

#include "abulafia/config.h"

#include "abulafia/patterns/pattern.h"

#include <utility>

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

template <typename OP_T, typename NEG_T, typename CB_T>
auto transform(Except<OP_T, NEG_T> const& tgt, CB_T const& cb) {
  auto new_op = cb(tgt.op());
  auto new_neg = cb(tgt.neg());

  return Except<decltype(new_op), decltype(new_neg)>(std::move(new_op), std::move(new_neg));
}

template <typename OP_T, typename NEG_T>
auto except(OP_T lhs, NEG_T rhs) {
  return Except<OP_T, NEG_T>(
      make_pattern(std::move(lhs)), make_pattern(std::move(rhs)));
}

// pattern - pattern
template <typename LHS_T, typename RHS_T,
    typename Enable = enable_if_t<are_valid_binary_operands<LHS_T, RHS_T>()>>
auto operator-(LHS_T lhs, RHS_T rhs) {
  return except(std::move(lhs), std::move(rhs));
}

}  // namespace ABULAFIA_NAMESPACE

#endif
