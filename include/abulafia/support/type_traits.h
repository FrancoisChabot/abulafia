//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_SUPPORT_TYPE_TRAITS_H_
#define ABULAFIA_SUPPORT_TYPE_TRAITS_H_

#include "abulafia/config.h"

#include <deque>
#include <list>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

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

// Tuple
template <typename T>
struct is_tuple : public std::false_type {};

template <typename... ARGS_T>
struct is_tuple<std::tuple<ARGS_T...>> : public std::true_type {};

// Collection
template <typename T>
struct is_collection : public std::false_type {};

template <typename T, typename ALLOC>
struct is_collection<std::vector<T, ALLOC>> : public std::true_type {};

template <typename T, typename ALLOC>
struct is_collection<std::list<T, ALLOC>> : public std::true_type {};

template <typename T, typename ALLOC>
struct is_collection<std::deque<T, ALLOC>> : public std::true_type {};

template <typename C, typename T, typename A>
struct is_collection<std::basic_string<C, T, A>> : public std::true_type {};

template <typename T, typename ENABLE = void>
struct reset_if_collection {
  static void exec(T&) {}
};

template <typename T>
struct reset_if_collection<T, enable_if_t<is_collection<T>::value>> {
  static void exec(T& c) { c.clear(); }
};

// Credit: https://stackoverflow.com/a/34672753/4442671
template <template <typename...> class C, typename... Ts>
std::true_type is_base_of_template_impl(const C<Ts...>*);

template <template <typename...> class C>
std::false_type is_base_of_template_impl(...);

template <typename T, template <typename...> class C>
using is_base_of_template =
    decltype(is_base_of_template_impl<C>(std::declval<T*>()));

}  // namespace ABULAFIA_NAMESPACE
#endif
