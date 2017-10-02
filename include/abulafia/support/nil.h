//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_SUPPORT_NIL_H_
#define ABULAFIA_SUPPORT_NIL_H_

#include "abulafia/config.h"

namespace ABULAFIA_NAMESPACE {

// A "black hole" type. Its role is to be a valid DST_T for every pattern.
// A pattern declaring Nil as its attr_t implies that it does not write to
// its DST.
struct Nil {
  using value_type = Nil;
  using dst_type = Nil;
  Nil() = default;

  template <typename T>
  Nil(T const&) {}

  Nil& get() {
    return *this;
  }
  // Can be assigned anything.
  template <typename T>
  Nil& operator=(T&&) {
    return *this;
  }

  // indirects to itself
  Nil* operator->() { return this; }

  // Used by the integer parser
  template <typename T>
  Nil& operator*=(const T&) {
    return *this;
  }

  template <typename T>
  Nil& operator+=(const T&) {
    return *this;
  }

  // Can pose as anything, as long as it can be default-constructed.
  template <typename T>
  operator T() const {
    return T();
  }

  // Can behave as a basic vector.
  void clear() {}

  template <typename T>
  void push_back(T&&) {}

  template <typename... T>
  void emplace_back(T&&...) {}

  template <typename T, typename U>
  void insert(T const&, U const&, U const&) {}

  Nil const& end() const { return *this; }
};

// There is no harm in providing global instance, since Nil is stateless.
static Nil nil;

}  // namespace ABULAFIA_NAMESPACE

#endif
