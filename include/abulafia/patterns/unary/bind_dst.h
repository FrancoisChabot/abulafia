//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_UNARY_BIND_DST_H_
#define ABULAFIA_PATTERNS_UNARY_BIND_DST_H_

#include "abulafia/config.h"

#include "abulafia/patterns/pattern.h"
#include "abulafia/support/assert.h"

namespace ABULAFIA_NAMESPACE {

template <typename PAT_T>
class BindDst : public Pattern<BindDst<PAT_T>> {
  PAT_T child_;

 public:
  BindDst(PAT_T child) : child_(std::move(child)) {}

  PAT_T const& operand() const { return child_; }
};

template <typename PAT_T>
inline auto bind_dst(PAT_T pat) {
  return BindDst<pattern_t<PAT_T>>(make_pattern(std::move(pat)));
}

template <typename PAT_T, typename CB_T>
auto transform(BindDst<PAT_T> const& tgt, CB_T const& cb) {
  auto new_op = cb(tgt.operand());

  return BindDst<decltype(new_op)>(std::move(new_op));
}

}  // namespace ABULAFIA_NAMESPACE

#endif
