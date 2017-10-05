//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERN_H_
#define ABULAFIA_PATTERN_H_

#include "abulafia/config.h"

#include "abulafia/patterns/traits.h"
#include "abulafia/support/type_traits.h"

namespace ABULAFIA_NAMESPACE {

class PatternBase {};

template <typename CRTP_T>
class Pattern : public PatternBase {
 public:
  using pat_t = CRTP_T;
};

// Catch-all set of traits for every subclass of pattern.
template <typename T>
struct expr_traits<T, enable_if_t<std::is_base_of<PatternBase, T>::value>> {
  enum { is_pattern = true, converts_to_pattern = false };

  static const T& make_pattern(const T& v) { return v; }
};

}  // namespace ABULAFIA_NAMESPACE

#endif