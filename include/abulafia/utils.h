#//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_UTILS_H_INCLUDED
#define ABULAFIA_UTILS_H_INCLUDED

namespace abu::details_ {
struct noop_t {
  template <typename... Ts>
  constexpr void operator()(const Ts&...) const {}
};
inline constexpr noop_t noop;

struct null_sink_t {
  template <typename T>
  constexpr const null_sink_t& operator=(T&&) const {
    return *this;
  }
};

struct null_vector_sink_t {
  constexpr void add() { size_ += 1; }
  constexpr std::size_t size() const { return size_; }

 private:
  std::size_t size_ = 0;
};

}  // namespace abu::details_

#endif