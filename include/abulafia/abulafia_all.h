//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
#ifndef MY_PROJECT_HEADER_GUARD_H_
#define MY_PROJECT_HEADER_GUARD_H_
#include <cassert>
#include <concepts>
#include <iterator>
#include <optional>
#include <ranges>
#include <stdexcept>
#include <utility>
#include <variant>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-attributes"
#endif
namespace abu {
#if defined(_MSC_VER)
inline void unreachable [[noreturn]] () { std::terminate(); }
#elif defined(__GNUC__)
inline void unreachable [[noreturn]] () { __builtin_unreachable(); }
#elif defined(__clang__)
inline void unreachable [[noreturn]] () { std::terminate(); }
#endif
}  // namespace abu
#if defined(NDEBUG)
#define abu_assume(condition) \
  if (!(condition)) unreachable()
#else
#define abu_assume(condition) \
  if (!(condition)) assert(condition)
#endif
#define abu_precondition(condition) abu_assume(condition)

namespace abu {
struct error {
  constexpr bool operator==(const error& rhs) const = default;
};
template <typename T>
class result {
 public:
  using success_type = T;
  using failure_type = error;
  result(success_type arg) : storage_(std::move(arg)) {}
  result(error arg) : storage_(std::move(arg)) {}
  operator bool() const { return storage_.index() == 0; }
  constexpr T& value() {
    abu_assume(storage_.index() == 0);
    return std::get<0>(storage_);
  }
  constexpr const T& value() const {
    abu_assume(storage_.index() == 0);
    return std::get<0>(storage_);
  }
  constexpr failure_type& failure() {
    abu_assume(storage_.index() == 1);
    return std::get<1>(storage_);
  }
  constexpr const failure_type& failure() const {
    abu_assume(storage_.index() == 1);
    return std::get<1>(storage_);
  }

 private:
  std::variant<success_type, failure_type> storage_;
};
template <>
class result<void> {
 public:
  using success_type = void;
  using failure_type = error;
  result() noexcept = default;
  result(failure_type arg) : storage_(std::move(arg)) {}
  template <typename T>
  result(const result<T>& other) {
    if (!other) {
      storage_ = other.failure();
    }
  }
  operator bool() const { return !storage_; }
  constexpr failure_type& failure() {
    abu_assume(storage_);
    return *storage_;
  }
  constexpr const failure_type& failure() const {
    abu_assume(storage_);
    return *storage_;
  }

 private:
  std::optional<failure_type> storage_;
};
}  // namespace abu

namespace abu {
template <typename T>
concept Token = std::regular<T>;
template <typename T, typename TokT>
concept TokenSet = requires {
  Token<TokT>;
  std::predicate<T, TokT>;
};
}  // namespace abu

namespace abu {
template <typename CrtpT>
class pattern {
 public:
  using pattern_type = CrtpT;
  template <typename ActT>
  auto operator[](ActT act) const {
    return apply_action(*static_cast<pattern_type const*>(this),
                        std::move(act));
  }
};
namespace details {
template <typename CrtpT>
void pattern_test(pattern<CrtpT>&);
}
template <typename T>
concept Pattern = requires(T x) {
  typename T::value_type;
  ::abu::details::pattern_test(x);
  { T::template can_match<char> } -> std::convertible_to<bool>;
};
template <Pattern T>
using pattern_value_t = typename T::value_type;
template <Pattern PatT, Token TokT>
static constexpr bool pattern_can_match = PatT::template can_match<TokT>;
template <Pattern T>
using parse_result_t = result<pattern_value_t<T>>;
using check_result_t = result<void>;
template <typename T>
struct to_pattern;
template <typename T>
concept PatternConvertible = requires(T x) {
  { ::abu::to_pattern<T>{}(x) } -> Pattern;
};
}  // namespace abu

namespace abu {
template <std::ranges::input_range Data, Pattern PatT>
constexpr auto parse(const Data& data, const PatT& pat) {
  static_assert(pattern_can_match<PatT, std::ranges::range_value_t<Data>>);
  auto beg = std::ranges::begin(data);
  return parse(beg, std::ranges::end(data), pat);
}
template <std::ranges::input_range Data, Pattern PatT>
constexpr auto check(const Data& data, const PatT& pat) {
  static_assert(pattern_can_match<PatT, std::ranges::range_value_t<Data>>);
  auto beg = std::ranges::begin(data);
  return check(beg, std::ranges::end(data), pat);
}
}  // namespace abu

namespace abu {
template <Token TokT, TokenSet<TokT> TokSetT>
class tok_t : public pattern<tok_t<TokT, TokSetT>> {
 public:
  using token_set_type = TokSetT;
  using token_type = TokT;
  using value_type = token_type;
  template <Token U>
  static constexpr bool can_match = std::is_same_v<U, token_type>;
  explicit constexpr tok_t(token_set_type tokens) noexcept
      : tokens_(std::move(tokens)) {}
  constexpr bool matches(const token_type& c) const noexcept {
    return tokens_(c);
  }

 private:
  [[no_unique_address]] token_set_type tokens_;
};
template <typename T>
concept TokPattern = requires(T x) {
  { tok_t(x) } -> std::same_as<T>;
};
template <std::input_iterator I, std::sentinel_for<I> S, TokPattern PatT>
constexpr parse_result_t<PatT> parse(I& begin, S end, const PatT& pat) {
  if (begin != end) {
    auto t = *begin;
    if (pat.matches(t)) {
      ++begin;
      return t;
    }
  }
  return error{};
}
template <std::input_iterator I, std::sentinel_for<I> S, TokPattern PatT>
constexpr check_result_t check(I& begin, S end, const PatT& pat) {
  return parse(begin, end, pat);
}
}  // namespace abu

namespace abu {
template <Token TokT, TokenSet<TokT> TokSetT>
constexpr auto tok(TokSetT tokens) {
  return tok_t<TokT, TokSetT>{std::move(tokens)};
}
}  // namespace abu

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif
