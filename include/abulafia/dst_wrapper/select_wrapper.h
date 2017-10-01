//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_SELECT_WRAPPER_H_
#define ABULAFIA_SELECT_WRAPPER_H_

#include "abulafia/config.h"
#include "abulafia/dst_wrapper/value_wrapper.h"
#include "abulafia/dst_wrapper/collection_wrapper.h"
#include "abulafia/support/type_traits.h"

namespace ABULAFIA_NAMESPACE {
  template <typename T, typename Enable=void>
  struct SelectDstWrapper {
    using type = ValueWrapper<T>;
  };

  template <>
  struct SelectDstWrapper<Nil> {
    using type = Nil;
  };

  template <typename T>
  struct SelectDstWrapper<T, std::enable_if_t<is_collection<T>::value>> {
    using type = CollectionWrapper<T>;
  };

  template<typename T>
  auto wrap_dst(T& dst) {
    return typename SelectDstWrapper<T>::type(dst);
  }

}  // namespace ABULAFIA_NAMESPACE

#endif
