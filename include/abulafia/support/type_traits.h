//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_SUPPORT_TYPE_TRAITS_H_
#define ABULAFIA_SUPPORT_TYPE_TRAITS_H_

#include "abulafia/config.h"

#include <tuple>
#include <type_traits>
#include <utility>

// Importing standard C++ trait and utilities directly in the abulafia
// namespace.
namespace ABULAFIA_NAMESPACE {
using std::decay_t;
using std::enable_if_t;
using std::forward;
using std::is_same;
using std::tuple_element_t;

template <typename T, typename... ARGS_T>
struct is_one_of : public std::false_type {};

template <typename T, typename U, typename... REST_T>
struct is_one_of<T, U, REST_T...>
    : public std::conditional_t<std::is_same<T, U>::value, std::true_type,
                                is_one_of<T, REST_T...>> {};

template <typename T>
struct is_tuple : public std::false_type {};

template <typename... ARGS_T>
struct is_tuple<std::tuple<ARGS_T...>> : public std::true_type {};

}  // namespace ABULAFIA_NAMESPACE
#endif
