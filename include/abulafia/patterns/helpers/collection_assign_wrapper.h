//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_HELPERS_COLLECTION_ASSIGN_WRAPPER_H_
#define ABULAFIA_PATTERNS_HELPERS_COLLECTION_ASSIGN_WRAPPER_H_

#include "abulafia/config.h"

#include "abulafia/support/nil.h"
#include "abulafia/support/type_traits.h"

#include <vector>

namespace ABULAFIA_NAMESPACE {

namespace details {

template <typename T, typename Enable = void>
void reset_if_container(T& dst);

template <typename CONT_T, typename... ARGS>
void append_to_container(CONT_T& container, ARGS&&... args) {
  container.emplace_back(forward<ARGS>(args)...);
}

// std::basic_string does not have an emplace_back, and is a commonly used type.
template <class CharT, class Traits = std::char_traits<CharT>,
          class Allocator = std::allocator<CharT>, typename... ARGS>
void append_to_container(std::basic_string<CharT, Traits, Allocator>& container,
                         ARGS&&... args) {
  container.push_back(forward<ARGS>(args)...);
}
}  // namespace details
template <typename T>
struct DefaultCollectionAssignWrapper {
  using value_type = typename T::value_type;
  using iterator = typename T::iterator;

  DefaultCollectionAssignWrapper(T& dst) : dst_(dst) {}

  // This is the key point:
  // If we try to assign a value to the wrapper, it'll result in a emplace_back
  // on its target.
  template <typename U>
  DefaultCollectionAssignWrapper& operator=(U&& rhs) {
    details::append_to_container(dst_, forward<U>(rhs));
    return *this;
  }

  // The wrapper can also pose as the target itself.
  template <typename U>
  void emplace_back(U&& rhs) {
    details::append_to_container(dst_, forward<U>(rhs));
  }

  template <typename ite>
  void insert(iterator pos, ite b, ite e) {
    dst_.insert(pos, b, e);
  }

  iterator end() { return dst_.end(); }

  // This intentionally does nothing
  void clear() {}

  // Should this ever happen?
  //  void push_back(Nil const&) {}

  T& dst_;
};

template <typename T>
struct ChooseCollectionWrapper {
  using type = DefaultCollectionAssignWrapper<T>;
};

// TODO: This is not necessarily true
template <typename T>
struct ChooseCollectionWrapper<DefaultCollectionAssignWrapper<T>> {
  using type = DefaultCollectionAssignWrapper<T>;
};

template <>
struct ChooseCollectionWrapper<Nil> {
  using type = Nil;
};

template <typename T>
using CollectionAssignWrapper = typename ChooseCollectionWrapper<T>::type;

}  // namespace ABULAFIA_NAMESPACE
#endif
