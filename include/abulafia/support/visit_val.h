//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_SUPPORT_VISIT_VAL_H
#define ABULAFIA_SUPPORT_VISIT_VAL_H

#include "abulafia/config.h"

#include <cstddef>
#include <type_traits>
#include <utility>

namespace ABULAFIA_NAMESPACE {

template <typename T, T MAX_V, T N>
struct val_visitor {
  template <typename VISIT_T>
  static decltype(auto) visit(int val, VISIT_T&& visitor) {
    if (N == val) {
      return visitor(std::integral_constant<int, N>());
    } else {
      return val_visitor<T, MAX_V, N + 1>::visit(
          val, std::forward<VISIT_T>(visitor));
    }
  }
};

template <typename T, T MAX_V>
struct val_visitor<T, MAX_V, MAX_V> {
  template <typename VISIT_T>
  static decltype(auto) visit(int, VISIT_T&& visitor) {
    return visitor(std::integral_constant<int, MAX_V>());
  }
};

template <int MAX_V, typename VISITOR_T>
decltype(auto) visit_val(int v, VISITOR_T&& visit) {
  return val_visitor<int, MAX_V - 1, 0>::visit(v,
                                               std::forward<VISITOR_T>(visit));
}  // namespace ABULAFIA_NAMESPACE

}  // namespace ABULAFIA_NAMESPACE

#endif