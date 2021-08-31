//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_OP_RESULT_H_INCLUDED
#define ABULAFIA_OP_RESULT_H_INCLUDED

#include <optional>
#include <variant>

#include "abulafia/assert.h"

namespace abu {

struct failure_t {
  // TOdo: Populate me!
};

struct success_t {};
struct partial_result_t {};

inline constexpr success_t success;
inline constexpr partial_result_t partial_result;

class error : public std::exception {};
class no_match_error : public error {};

struct op_result {
 public:
  constexpr op_result(success_t) : mode_(mode::success){};
  constexpr op_result(partial_result_t) : mode_(mode::partial){};
  constexpr op_result(failure_t) : mode_(mode::failure){};

  constexpr bool is_success() const { return mode_ == mode::success; }
  constexpr bool is_partial() const { return mode_ == mode::partial; }
  constexpr bool is_match_failure() const { return mode_ == mode::failure; }

 private:
  enum class mode { success, partial, failure };
  mode mode_ = mode::success;
};

//

// class match_failure_error : public std::exception {
//  public:
//   match_failure_error(match_failure_t f) : fail_(std::move(f)) {}

//  private:
//   match_failure_t fail_;
// };

// template <typename T>
// class parse_result {
//  public:
//   using value_type = T;

//   constexpr parse_result(const T& val) : payload_(val) {}
//   constexpr parse_result(T&& val) : payload_(std::move(val)) {}
//   constexpr parse_result(match_failure_t&& err) : payload_(std::move(err)) {}

//   constexpr bool is_success() const { return payload_.index() == 0; }
//   constexpr bool is_match_failure() const { return payload_.index() == 1; }

//   constexpr operator bool() const { return is_success(); }

//   constexpr T& value() {
//     switch (payload_.index()) {
//       case 0:
//         return std::get<0>(payload_);
//         break;
//       case 1:
//         throw match_failure_error{std::get<1>(payload_)};
//     }

//     abu::unreachable();
//   }

//   constexpr const T& value() const {
//     switch (payload_.index()) {
//       case 0:
//         return std::get<0>(payload_);
//         break;
//       case 1:
//         throw match_failure_error{std::get<1>(payload_)};
//     }

//     abu::unreachable();
//   }

//   constexpr match_failure_t& match_failure() {
//     abu_assume(is_match_failure());
//     return std::get<1>(payload_);
//   }

//   constexpr const match_failure_t& match_failure() const {
//     abu_assume(is_match_failure());
//     return std::get<1>(payload_);
//   }

//  private:
//   std::variant<T, match_failure_t> payload_;
// };

// template <>
// class parse_result<void> {
//  public:
//   using value_type = void;

//   constexpr parse_result() = default;
//   constexpr parse_result(match_failure_t&& err)
//       : payload_(std::move(err)), failed_(true) {}

//   constexpr bool is_success() const { return !failed_; }
//   constexpr bool is_match_failure() const { return failed_; }

//   constexpr operator bool() const { return is_success(); }

//   constexpr match_failure_t& match_failure() {
//     abu_assume(is_match_failure());
//     return payload_;
//   }

//   constexpr const match_failure_t& match_failure() const {
//     abu_assume(is_match_failure());
//     return payload_;
//   }

//  private:
//   match_failure_t payload_;
//   bool failed_ = false;
// };

}  // namespace abu
#endif