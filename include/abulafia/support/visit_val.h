//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_SUPPORT_VISIT_VAL_H
#define ABULAFIA_SUPPORT_VISIT_VAL_H

#include "abulafia/config.h"

namespace ABULAFIA_NAMESPACE {

template <std::size_t MAX_V, std::size_t N>
struct val_visitor {
  template <typename VISIT_T>
  static decltype(auto) visit(std::size_t val, VISIT_T&& visitor) {
    if (N == val) {
      return visitor(std::integral_constant<std::size_t, N>());
    } else {
      return val_visitor<MAX_V, N + 1>::visit(val,
                                              std::forward<VISIT_T>(visitor));
    }
  }
};

template <std::size_t MAX_V>
struct val_visitor<MAX_V, MAX_V> {
  template <typename VISIT_T>
  static decltype(auto) visit(std::size_t, VISIT_T&& visitor) {
    return visitor(std::integral_constant<std::size_t, MAX_V>());
  }
};

template <std::size_t MAX_V, typename VISITOR_T>
decltype(auto) visit_val(std::size_t v, VISITOR_T&& visit) {
  return val_visitor<MAX_V - 1, 0>::visit(v, std::forward<VISITOR_T>(visit));
}  // namespace ABULAFIA_NAMESPACE

}  // namespace ABULAFIA_NAMESPACE

#endif