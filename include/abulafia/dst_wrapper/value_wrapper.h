//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_VALUE_WRAPPER_H_
#define ABULAFIA_VALUE_WRAPPER_H_

#include "abulafia/config.h"

#include "abulafia/support/nil.h"

#include <utility>

namespace ABULAFIA_NAMESPACE {
  
  template<typename T>
  class ValueWrapper {
  public:
    using dst_type = T;

    ValueWrapper(T& v) : v_(v) {}
    ValueWrapper(ValueWrapper const&) = default;

    template<typename U>
    ValueWrapper& operator=(U&& v) {
      v_ = std::forward<U>(v);
      return *this;
    }

    // using this implies that we are NOT atomic in nature.
    T& get() {
      return v_;
    }

  private:
    T& v_;
  };

}  // namespace ABULAFIA_NAMESPACE

#endif
