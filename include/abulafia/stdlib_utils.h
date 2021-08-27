#//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_STD_UTILS_H_INCLUDED
#define ABULAFIA_STD_UTILS_H_INCLUDED

// This file contains utilities that introspect and manipulate standard library
// entities in ways that are not specific to abulafia.

#include <iterator>
#include <ranges>

namespace abu {
namespace std_utils {

namespace archetypes {

// ***** std::input_iterator *****
template <typename T>
struct input_iterator : public std::input_iterator_tag {
  using difference_type = std::ptrdiff_t;
  using value_type = T;

  value_type operator*() const;
  input_iterator& operator++();
  void operator++(int);

  bool operator==(const std::default_sentinel_t&) const;
};

using input_iterator_sentinel = std::default_sentinel_t;

static_assert(std::input_iterator<input_iterator<char>>);
static_assert(std::sentinel_for<input_iterator_sentinel, input_iterator<char>>);

// ***** std::forward_iterator *****
template <typename T>
struct forward_iterator : std::forward_iterator_tag {
  using difference_type = std::ptrdiff_t;
  using value_type = T;

  value_type& operator*() const;
  forward_iterator& operator++();
  forward_iterator operator++(int);

  bool operator==(const forward_iterator&) const;
  bool operator==(const std::default_sentinel_t&) const;
};

using forward_iterator_sentinel = std::default_sentinel_t;

static_assert(std::forward_iterator<forward_iterator<char>>);
static_assert(
    std::sentinel_for<forward_iterator_sentinel, forward_iterator<char>>);

}  // namespace archetypes
}  // namespace std_utils

}  // namespace abu

#endif