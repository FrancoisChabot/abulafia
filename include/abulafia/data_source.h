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
#include <ranges>

#include "abulafia/assert.h"
#include "abulafia/token.h"

namespace abu {

template <typename T>
concept DataSource = requires {
  typename T::token_type;
};

// ***** DataChunk *****

template <std::input_iterator I, std::sentinel_for<I> S>
struct data_chunk;

template <std::input_iterator I, std::sentinel_for<I> S>
struct input_checkpoint {
  explicit input_checkpoint() = default;
  inline input_checkpoint(std::size_t position, data_chunk<I, S>* handler);
  inline input_checkpoint(input_checkpoint&& rhs);
  inline input_checkpoint& operator=(input_checkpoint&& rhs);
  inline ~input_checkpoint();

  input_checkpoint(const input_checkpoint&) = delete;
  input_checkpoint& operator=(const input_checkpoint&) = delete;

  constexpr std::size_t position() const { return position_; }

 private:
  std::size_t position_;
  data_chunk<I, S>* handler_ = nullptr;
};

template <std::input_iterator I, std::sentinel_for<I> S>
struct data_chunk {
  using token_type = std::iter_value_t<I>;
  using checkpoint_type = input_checkpoint<I, S>;

  constexpr data_chunk(I begin, S end)
      : next_(std::move(begin)), end_(std::move(end)) {}
  constexpr token_type peek() const {
    if (checkpoint_count_ == 0) {
      return *next_;
    }

    abu_assume(rollback_read_pos_ < rollback_buffer_.size());
    return rollback_buffer_[rollback_read_pos_];
  }

  constexpr bool empty() const { return next_ == end_; }

  constexpr token_type read() {
    auto result = peek();
    ++next_;

    if (checkpoint_count_ > 0) {
      rollback_read_pos_ += 1;
      if (rollback_read_pos_ == rollback_buffer_.size()) {
        rollback_buffer_.push_back(*next_);
      }
    }
    return result;
  }

  constexpr checkpoint_type make_checkpoint() {
    if (checkpoint_count_ == 0) {
      abu_assume(rollback_read_pos_ == 0);
      abu_assume(rollback_buffer_.size() == 0);
      rollback_buffer_.push_back(*next_);
    }

    ++checkpoint_count_;
    return checkpoint_type{rollback_read_pos_, this};
  }

  void checkpoint_removed() {
    abu_precondition(checkpoint_count_ > 0);
    --checkpoint_count_;

    if (checkpoint_count_ == 0) {
      // No more checkpoints means no more need for rolling back
      rollback_buffer_ = {};
      rollback_read_pos_ = 0;
    }
  }

  constexpr void rollback(const checkpoint_type& cp) {
    abu_assume(cp.position() < rollback_buffer_.size());
    rollback_read_pos_ = cp.position();
  }

 private:
  I next_;
  [[no_unique_address]] S end_;

  std::size_t checkpoint_count_ = 0;
  std::size_t rollback_read_pos_ = 0;
  std::deque<token_type> rollback_buffer_;
};

template <std::input_iterator I, std::sentinel_for<I> S>
input_checkpoint<I, S>::input_checkpoint(std::size_t position,
                                         data_chunk<I, S>* handler)
    : position_(position), handler_(handler) {
  abu_assume(handler_);
}

template <std::input_iterator I, std::sentinel_for<I> S>
input_checkpoint<I, S>::input_checkpoint(input_checkpoint<I, S>&& rhs)
    : handler_(rhs.handler_) {
  rhs.handler_ = nullptr;
}

template <std::input_iterator I, std::sentinel_for<I> S>
input_checkpoint<I, S>& input_checkpoint<I, S>::operator=(
    input_checkpoint<I, S>&& rhs) {
  if (handler_ && rhs.handler_ != handler_) {
    handler_->checkpoint_removed();
  }

  handler_ = rhs.handler_;
  rhs.handler_ = nullptr;
  return *this;
}

template <std::input_iterator I, std::sentinel_for<I> S>
input_checkpoint<I, S>::~input_checkpoint() {
  if (handler_) {
    handler_->checkpoint_removed();
  }
}

// Data chunks of forward iterators can simply use the iterator itself
// as checkpoints.
template <std::forward_iterator I, std::sentinel_for<I> S>
struct data_chunk<I, S> {
  using token_type = std::iter_value_t<I>;
  using checkpoint_type = I;

  constexpr data_chunk(I begin, S end)
      : next_(std::move(begin)), end_(std::move(end)) {}
  constexpr const token_type& peek() const { return *next_; }
  constexpr bool empty() const { return next_ == end_; }
  constexpr token_type read() { return *next_++; }

  constexpr I make_checkpoint() const { return next_; }
  constexpr void rollback(I cp) { next_ = std::move(cp); }

 private:
  I next_;
  [[no_unique_address]] S end_;
};

// ***** DataFeed *****

// Data feed chunks are organized as a dag of shared pointers, where both the
// feed itself and checkpoints keep branches alive.

struct data_feed_chunk_elem {
  std::shared_ptr<data_feed_chunk_elem> next_chunk;
}

template <typename ChunkData>
struct data_feed_chunk : public data_feed_chunk_elem {
  explicit data_feed_chunk(ChunkData init_data) : data(std::move(init_data) {}
  ChunkData data;
};

// ***** basic_data_feed *****
template <std::ranges::input_range ChunkData>
class basic_data_feed {
  using chunk_type = data_feed_chunk<ChunkData>;
  using data_iterator_type = std::ranges::iterator_t<ChunkData>;
  using data_sentinel_type = std::ranges::sentinel_t<ChunkData>;

 public:
  using token_type = std::ranges::range_value_t<ChunkData>;

  constexpr void add(ChunkData chunk) {
    auto chunk_ptr = std::make_shared<data_feed_chunk<ChunkData>>(
        data_feed_chunk{std::move(chunk)});

    auto new_back = chunk_ptr.get();
    if (empty()) {
      current_chunk_next_ = std::ranges::begin(chunk_ptr->data);
      current_chunk_end_ = std::ranges::end(chunk_ptr->data);
      current_chunk_ = std::move(chunk_ptr);
    } else {
      last_chunk_->next_chunk = std::move(chunk_ptr);
    }
    last_chunk_ = new_back;
  }

  constexpr const token_type& peek() const {
    abu_precondition(!empty());

    return *current_chunk_next_;
  }

  constexpr bool empty() const { return !current_chunk_; }

  constexpr token_type read() {
    auto result = *current_chunk_next_++;
    if (current_chunk_next_ == current_chunk_end_) {
      // Just in case iterators require to be outlived by their container
      current_chunk_next_ = {};
      current_chunk_end_ = {};

      // This is intentionally not be a move!
      // A checkpoint could be holding on to the current_chunk_.
      current_chunk_ = current_chunk_->next_chunk;

      if (current_chunk_) {
        current_chunk_next_ = std::ranges::begin(current_chunk_->data);
        current_chunk_end_ = std::ranges::end(current_chunk_->data);
      } else {
        last_chunk_ = nullptr;
      }
    }
    return result;
  }

 protected:
  std::shared_ptr<data_feed_chunk_elem> current_chunk_;
  chunk_type* last_chunk_ = nullptr;

  data_iterator_type current_chunk_next_;
  data_sentinel_type current_chunk_end_;
};

// ***** Input feed *****
template <std::ranges::input_range ChunkData>
struct feed_input_checkpoint {
  std::shared_ptr<data_feed_chunk<ChunkData>> chunk;
};

template <std::ranges::input_range ChunkData>
struct data_feed : public basic_data_feed<ChunkData> {
  using token_type = std::ranges::range_value_t<ChunkData>;

 private:
  std::deque<token_type> rollback_buffer_;
};

// ***** Forward feed *****
template <std::ranges::forward_range ChunkData>
struct feed_forward_checkpoint {
  using pointer_type = std::shared_ptr<data_feed_chunk<ChunkData>>;
  using iterator_type = std::ranges::iterator_t<ChunkData>;

  pointer_type chunk;
  iterator_type next;
};

template <std::ranges::forward_range ChunkData>
struct data_feed<ChunkData> : public basic_data_feed<ChunkData> {
  using checkpoint_type = feed_forward_checkpoint<ChunkData>;

  constexpr checkpoint_type make_checkpoint() const {
    return checkpoint_type{this->current_chunk_, this->current_chunk_next_};
  }

  constexpr void rollback(checkpoint_type cp) {
    if (cp.chunk) {
      this->current_chunk_ = std::move(cp.chunk);
      this->current_chunk_next_ = std::move(cp.next);
      this->current_chunk_end_ = std::ranges::end(this->current_chunk_->data);
    } else {
      // we are rolling back to before we had any data at
    }
  }
};
}  // namespace abu

#endif