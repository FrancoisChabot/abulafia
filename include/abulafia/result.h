//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_RESULT_H_INCLUDED
#define ABULAFIA_RESULT_H_INCLUDED

#include <concepts>
#include <optional>
#include <stdexcept>
#include <type_traits>
#include <variant>

#include "abulafia/assert.h"

namespace abu {

struct error {};

struct bad_result_access : public std::exception {
  bad_result_access(error err) : err_(err) {}

  const error& get_error() const { return err_; }

 private:
  error err_;
};

template <typename T>
class result {
 public:
  using value_type = T;
  using failure_type = error;

  constexpr result(value_type arg) : storage_(std::move(arg)) {}
  constexpr result(error arg) noexcept : storage_(std::move(arg)) {}

  constexpr operator bool() const { return storage_.index() == 0; }

  constexpr T& operator*() {
    if (storage_.index() != 0) {
      throw bad_result_access{std::get<1>(storage_)};
    }
    return std::get<0>(storage_);
  }

  constexpr const T& operator*() const {
    if (storage_.index() != 0) {
      throw bad_result_access{std::get<1>(storage_)};
    }
    return std::get<0>(storage_);
  }

  constexpr failure_type& get_error() {
    abu_assume(storage_.index() == 1);
    return std::get<1>(storage_);
  }

  constexpr const failure_type& get_error() const {
    abu_assume(storage_.index() == 1);
    return std::get<1>(storage_);
  }

 private:
  std::variant<value_type, failure_type> storage_;
};

template <>
class result<void> {
 public:
  using value_type = void;
  using failure_type = error;

  constexpr result() noexcept = default;
  constexpr result(failure_type arg) : storage_(std::move(arg)) {}

  template <typename T>
  result(const result<T>& other) {
    if (!other) {
      storage_ = other.get_error();
    }
  }

  constexpr operator bool() const { return !storage_; }

  constexpr failure_type& get_error() {
    abu_assume(storage_);
    return *storage_;
  }

  constexpr const failure_type& get_error() const {
    abu_assume(storage_);
    return *storage_;
  }

 private:
  std::optional<failure_type> storage_;
};

template <typename T>
concept Result = requires(T x) {
  { ::abu::result(std::move(x)) } -> std::same_as<T>;
};

template <typename T>
concept TrivialResult = Result<T> &&
    (std::same_as<typename T::value_type, void> ||
     std::is_trivial_v<typename T::value_type>);

}  // namespace abu
#endif