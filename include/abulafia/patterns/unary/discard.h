//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_UNARY_DISCARD_H_
#define ABULAFIA_PATTERNS_UNARY_DISCARD_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/pattern.h"

namespace ABULAFIA_NAMESPACE {

template <typename PAT_T>
class Discard : public Pattern<Discard<PAT_T>> {
  PAT_T operand_;

 public:
  Discard(PAT_T op) : operand_(std::move(op)) {}

  PAT_T const& operand() const { return operand_; }
};

template <typename PAT_T>
auto discard(PAT_T pat) {
  return Discard<pattern_t<PAT_T>>(make_pattern(std::move(pat)));
}

template <typename PAT_T, typename CB_T>
auto transform(Discard<PAT_T> const& tgt, CB_T const& cb) {
  return discard(cb(tgt.operand()));
}

}  // namespace ABULAFIA_NAMESPACE

#endif
