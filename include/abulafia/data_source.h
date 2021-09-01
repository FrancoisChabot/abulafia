//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_DATA_SOURCE_H_INCLUDED
#define ABULAFIA_DATA_SOURCE_H_INCLUDED

#include <concepts>
#include <iterator>

#include "abulafia/token.h"

namespace abu {

template <typename T>
concept DataSource = requires {
  typename T::token_type;
};

template <std::input_iterator I, std::sentinel_for<I> S>
struct data_chunk {
  using token_type = std::iter_value_t<I>;

  constexpr data_chunk(I& begin, S end) : next_(begin), end_(end) {}
  constexpr const token_type& peek() const { return *next_; }
  constexpr bool empty() const { return next_ == end_; }
  constexpr token_type read() { return *next_++; }

 private:
  I& next_;
  [[no_unique_address]] S end_;
};

}  // namespace abu

#endif