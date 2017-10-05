//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_BINARY_LIST_H_
#define ABULAFIA_PATTERNS_BINARY_LIST_H_

#include "abulafia/config.h"

#include "abulafia/patterns/pattern.h"

#include <utility>

namespace ABULAFIA_NAMESPACE {

// List(a, b) -> a >> *( ignore(b) >> a )
template <typename OP_T, typename SEP_PAT_T>
class List : public Pattern<List<OP_T, SEP_PAT_T>> {
 public:
  List(OP_T val_pat, SEP_PAT_T sep)
      : val_(std::move(val_pat)), sep_(std::move(sep)) {}

  OP_T const& op() const { return val_; }
  SEP_PAT_T const& sep() const { return sep_; }

 private:
  OP_T val_;
  SEP_PAT_T sep_;
};

template <typename LHS_T, typename RHS_T, typename CB_T>
auto transform(List<LHS_T, RHS_T> const& tgt, CB_T const& cb) {
  auto new_op = cb(tgt.operand());
  auto new_sep = cb(tgt.sep());

  return List<decltype(new_op), decltype(new_sep)>(std::move(new_op),
                                                   std::move(new_sep));
}

template <typename LHS_T, typename RHS_T>
auto list(LHS_T lhs, RHS_T rhs) {
  return List<pattern_t<LHS_T>, pattern_t<RHS_T>>(make_pattern(std::move(lhs)),
                                                  make_pattern(std::move(rhs)));
}

template <
    typename LHS_T, typename RHS_T,
    typename Enable = enable_if_t<are_valid_binary_operands<LHS_T, RHS_T>()>>
auto operator%(LHS_T&& lhs, RHS_T&& rhs) {
  return list(forward<LHS_T>(lhs), forward<RHS_T>(rhs));
}

}  // namespace ABULAFIA_NAMESPACE

#endif
