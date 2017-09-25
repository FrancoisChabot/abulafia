//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_CHAR_SET_SET_H_
#define ABULAFIA_CHAR_SET_SET_H_

#include "abulafia/config.h"

#include "abulafia/char_set/char_set.h"

#include <bitset>
#include <set>

namespace ABULAFIA_NAMESPACE {
namespace char_set {

template <typename CHAR_T>
struct Set {
  using char_t = CHAR_T;
  template <typename CONTAINER_T>
  Set(CONTAINER_T const& c) : characters_(std::begin(c), std::end(c)) {}

  template <typename T>
  bool is_valid(const T& character) const {
    return characters_.find(character) != characters_.end();
  }

  std::set<CHAR_T, std::less<void>> characters_;
};

// Character sets of chars are simple enough that they can be bitsets
// TODO: Operation on bitset sets (such as oring two together) should yield
// other bitsets.
template <>
struct Set<char> {
  using char_t = char;

  template <typename CONTAINER_T>
  Set(CONTAINER_T const& list) {
    for (auto const& c : list) {
      unsigned char pos = static_cast<unsigned char>(c);
      characters_[pos] = true;
    }
  }

  template <typename T>
  bool is_valid(const T& character) const {
    unsigned char pos = static_cast<unsigned char>(character);
    return characters_.test(pos);
  }

  std::bitset<256> characters_;
};

template <typename T>
struct is_char_set<Set<T>> : public std::true_type {};

// Create from some stl-like container
template <typename CONTAINER_T>
inline auto set(CONTAINER_T const& l) {
  return Set<typename CONTAINER_T::value_type>(l);
}

// Create from string literal
template <typename CHAR_T, std::size_t LEN>
inline auto set(const CHAR_T (&l)[LEN]) {
  // The cast to std::string is important to avoid treating the trailing null as
  // a member of the set.
  return Set<CHAR_T>(std::string(l));
}

}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE
#endif