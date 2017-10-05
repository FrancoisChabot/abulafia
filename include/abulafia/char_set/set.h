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
#include <cstring>
#include <limits>
#include <set>
#include <type_traits>

namespace ABULAFIA_NAMESPACE {
namespace char_set {

template <typename CHAR_T>
struct Set : public CharacterSet {
  using char_t = CHAR_T;

  template <typename ITE_T>
  Set(ITE_T b, ITE_T e) : characters_(b, e) {}

  template <typename T>
  bool is_valid(char_t const& character) const {
    return characters_.find(character) != characters_.end();
  }

  std::set<CHAR_T> characters_;
};

// For small types, like char, Character sets are simple enough that they can be
// bitsets.
template <typename CHAR_T>
struct IndexedSet : public CharacterSet {
  using char_t = CHAR_T;

  template <typename ITE_T>
  IndexedSet(ITE_T b, ITE_T e) {
    for (; b != e; ++b) {
      characters_[as_index(*b)] = true;
    }
  }

  bool is_valid(const char_t& c) const { return characters_.test(as_index(c)); }

 private:
  using unsigned_t = std::make_unsigned_t<CHAR_T>;

  static constexpr std::size_t as_index(CHAR_T c) { return unsigned_t(c); }

  std::bitset<std::numeric_limits<unsigned_t>::max() + 1> characters_;
};

template <>
struct Set<char> : public IndexedSet<char> {
  using IndexedSet::IndexedSet;
};

// Create from a pair of iterators
template <typename ITE>
auto set(ITE b, ITE e) {
  return Set<typename ITE::value_type>(b, e);
}

// Create from a pair of iterators
inline auto set(char const* v) { return Set<char>(v, v + std::strlen(v)); }

template <>
struct to_char_set_impl<const char*, void> {
  static Set<char> convert(char const* v) { return set(v); }
};

}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE
#endif