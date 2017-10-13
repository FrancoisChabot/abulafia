//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_NARY_SEQUENCE_H_
#define ABULAFIA_PATTERNS_NARY_SEQUENCE_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/nary/nary_pattern.h"
#include "abulafia/patterns/pattern.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {

// The sequence nary pattern.
template <typename... CHILD_PATS_T>
class Seq : public Pattern<Seq<CHILD_PATS_T...>> {
 public:
  using child_tuple_t = std::tuple<CHILD_PATS_T...>;

  Seq(child_tuple_t childs) : childs_(std::move(childs)) {}

  child_tuple_t const& childs() const { return childs_; }

 private:
  child_tuple_t childs_;
};

template <int Index, typename... CHILD_PATS_T>
auto const& getChild(Seq<CHILD_PATS_T...> const& pat) {
  return std::get<Index>(pat.childs());
}

template <typename... CHILD_PATS_T>
auto seq(CHILD_PATS_T&&... childs) {
  return Seq<CHILD_PATS_T...>(
      std::make_tuple(std::forward<CHILD_PATS_T>(childs)...));
}

template <typename LHS_T, typename RHS_T>
std::enable_if_t<are_valid_binary_operands<LHS_T, RHS_T>(),
                 typename detail::NaryPatternBuilder<Seq, pattern_t<LHS_T>,
                                                     pattern_t<RHS_T>>::type>
operator>>(LHS_T lhs, RHS_T rhs) {
  return detail::NaryPatternBuilder<Seq, pattern_t<LHS_T>, pattern_t<RHS_T>>::
      build(make_pattern(std::move(lhs)), make_pattern(std::move(rhs)));
}

}  // namespace ABULAFIA_NAMESPACE

#endif
