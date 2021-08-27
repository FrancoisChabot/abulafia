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
  using success_type = T;
  using failure_type = error;
  result(success_type arg) : storage_(std::move(arg)) {}
  result(error arg) : storage_(std::move(arg)) {}
  operator bool() const { return storage_.index() == 0; }
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
      storage_ = other.get_error();
    }
  }
  operator bool() const { return !storage_; }
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
}  // namespace abu

namespace abu {
template <typename T>
concept Token = std::copyable<T>;
template <typename PredT>
struct token_set {
  using predicate = PredT;
  explicit constexpr token_set(predicate pred) : pred_(std::move(pred)) {}
  template <Token T>
  constexpr bool operator()(const T& t) const {
    return pred_(t);
  }
  constexpr const predicate& pred() const { return pred_; }

 private:
  [[no_unique_address]] predicate pred_;
};
template <typename T>
concept TokenSet = requires(T x) {
  { token_set(x) } -> std::same_as<T>;
};
template <TokenSet T>
constexpr auto operator~(const T& arg) {
  return token_set([p = arg.pred()](const auto& t) { return !p(t); });
}
template <TokenSet LhsT, TokenSet RhsT>
constexpr auto operator-(const LhsT& lhs, const RhsT& rhs) {
  return token_set([pl = lhs.pred(), pr = rhs.pred()](const auto& t) {
    return !pr(t) && pl(t);
  });
}
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
  ::abu::details::pattern_test(x);
};
template <Pattern T>
using pattern_value_t = typename T::value_type;
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
  auto beg = std::ranges::begin(data);
  return parse(beg, std::ranges::end(data), pat);
}
template <std::ranges::input_range Data, PatternConvertible PatT>
constexpr auto parse(const Data& data, const PatT& pat) {
  return parse(data, to_pattern<PatT>{}(pat));
}
template <std::ranges::input_range Data, Pattern PatT>
constexpr auto check(const Data& data, const PatT& pat) {
  auto beg = std::ranges::begin(data);
  return check(beg, std::ranges::end(data), pat);
}
template <std::ranges::input_range Data, PatternConvertible PatT>
constexpr auto check(const Data& data, const PatT& pat) {
  return check(data, to_pattern<PatT>{}(pat));
}
}  // namespace abu

namespace abu {
template <TokenSet TokSetT>
class tok : public pattern<tok<TokSetT>> {
 public:
  using token_set_type = TokSetT;
  explicit constexpr tok(token_set_type tokens) noexcept
      : tokens_(std::move(tokens)) {}
  template <Token TokT>
  constexpr bool matches(const TokT& c) const noexcept {
    return tokens_(c);
  }

 private:
  [[no_unique_address]] token_set_type tokens_;
};
template <typename T>
concept TokPattern = requires(T x) {
  { tok(x) } -> std::same_as<T>;
};
template <std::input_iterator I, std::sentinel_for<I> S, TokPattern PatT>
constexpr result<std::iter_value_t<I>> parse(I& begin, S end, const PatT& pat) {
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
template <TokenSet TokSetT>
struct to_pattern<TokSetT> {
  constexpr auto operator()(TokSetT tokset) const {
    return tok{std::move(tokset)};
  }
};
}  // namespace abu

namespace abu {}  // namespace abu

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif
