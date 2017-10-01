//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_COLLECTION_WRAPPER_H_
#define ABULAFIA_COLLECTION_WRAPPER_H_

#include "abulafia/config.h"

namespace ABULAFIA_NAMESPACE {

  namespace details {

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

  template<typename T>
  class CollectionWrapper {
  public:
    using dst_type = typename T::value_type;

    CollectionWrapper(T& v) : v_(v) {}
    CollectionWrapper(CollectionWrapper const&) = default;

    template<typename U>
    CollectionWrapper& operator=(U&& rhs) {
      details::append_to_container(v_, forward<U>(rhs));
      return *this;
    }
    void clear() {
      v_.clear();
    }
  private:
    T& v_;
  };

}  // namespace ABULAFIA_NAMESPACE

#endif
