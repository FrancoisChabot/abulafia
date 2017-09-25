//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_SUPPORT_FUNCTION_TRAITS_H
#define ABULAFIA_SUPPORT_FUNCTION_TRAITS_H

#include "abulafia/config.h"

#include <tuple>

namespace ABULAFIA_NAMESPACE {

// creadit goes to https://stackoverflow.com/a/7943765/4442671
template <typename T>
struct function_traits : public function_traits<decltype(&T::operator())> {};

template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...) const>
// Specialization for pointers to member function
{
  enum { arity = sizeof...(Args) };

  typedef ReturnType result_type;

  template <size_t i>
  struct arg {
    using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
  };
};

template <typename ReturnType, typename... Args>
struct function_traits<ReturnType (*)(Args...)> {
  // Specialization for function pointers
  typedef ReturnType result_type;
  typedef ReturnType function_type(Args...);
  enum { arity = sizeof...(Args) };

  template <size_t i>
  struct arg {
    typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
  };
};

template <typename CALLABLE_T, std::size_t Index>
using callable_argument_t =
    typename function_traits<CALLABLE_T>::template arg<Index>::type;

template <typename CALLABLE_T>
using callable_result_t = typename function_traits<CALLABLE_T>::result_type;

}  // namespace ABULAFIA_NAMESPACE

#endif