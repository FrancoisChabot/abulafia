//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_DATA_SOURCE_H_INCLUDED
#define ABULAFIA_DATA_SOURCE_H_INCLUDED

#include <concepts>
#include <deque>
#include <iterator>
#include <memory>

#include "abulafia/token.h"

namespace abu {

template <typename T>
concept DataSource = requires {
  typename T::token_type;
};

template <std::input_iterator I, std::sentinel_for<I> S>
struct data_chunk {
  using token_type = std::iter_value_t<I>;

  constexpr data_chunk(I begin, S end)
      : next_(std::move(begin)), end_(std::move(end)) {}
  constexpr const token_type& peek() const { return *next_; }
  constexpr bool empty() const { return next_ == end_; }
  constexpr token_type read() { return *next_++; }

 private:
  I next_;
  [[no_unique_address]] S end_;
};

template <typename ChunkType>
struct data_feed {
  using chunk_iterator_type = std::ranges::iterator_t<ChunkType>;
  using chunk_sentinel_type = std::ranges::sentinel_t<ChunkType>;
  using token_type = std::ranges::range_value_t<ChunkType>;

  constexpr data_feed() {}

  constexpr void add(std::shared_ptr<ChunkType> chunk) {
    if (std::ranges::begin(*chunk) != std::ranges::end(*chunk)) {
      auto was_empty = empty();
      chunks_.push_back(chunk);
      if (was_empty) {
        current_chunk_next_ = std::ranges::begin(*chunk);
        current_chunk_end_ = std::ranges::end(*chunk);
      }
    }
  }

  constexpr const token_type& peek() const { return *current_chunk_next_; }

  constexpr bool empty() const { return current_chunk_ == chunks_.size(); }

  constexpr token_type read() {
    auto result = *current_chunk_next_++;
    if (current_chunk_next_ == current_chunk_end_) {
      if(current_chunk_ == 0 /*TODO: checkpoint test*/) {
        chunks_.pop_front();
      }
      else{
        ++current_chunk_;
      }

      if (current_chunk_ != chunks_.size()) {
        current_chunk_next_ = std::ranges::begin(*chunks_[current_chunk_]);
        current_chunk_end_ = std::ranges::begin(*chunks_[current_chunk_]);
      } else {
        current_chunk_next_ = {};
        current_chunk_end_ = {};
      }
    }
    return result;
  }

 private:
  std::deque<std::shared_ptr<ChunkType>> chunks_;
  std::size_t current_chunk_ = 0;

  chunk_iterator_type current_chunk_next_;
  chunk_sentinel_type current_chunk_end_;
};
}  // namespace abu

#endif