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
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-attributes"
#endif
#//  Copyright 2017-2021 Francois Chabot
#ifndef ABULAFIA_ARCHETYPES_H_INCLUDED
#define ABULAFIA_ARCHETYPES_H_INCLUDED

namespace abu {
namespace archetypes {
struct token {
  template <typename T>
  operator T() const noexcept;
};
struct value {
  template <typename T>
  operator T() const noexcept;
};
}  // namespace archetypes
}  // namespace abu
#endif

namespace abu {
template <typename T>
concept Token = std::is_copy_constructible_v<T>;
template <typename T>
concept TokenSet = std::predicate<T, archetypes::token>;
struct any_token {
  constexpr bool operator()(const auto&) const { return true; }
};
}  // namespace abu

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

namespace abu {
#if defined(_MSC_VER)
inline void unreachable [[noreturn]] () { std::terminate(); }
#elif defined(__GNUC__)
inline void unreachable [[noreturn]] () { __builtin_unreachable(); }
#elif defined(__clang__)
inline void unreachable [[noreturn]] () { std::terminate(); }
#endif
}  // namespace abu
#define abu_assert assert
#if defined(NDEBUG)
#define abu_assume(condition) \
  if (!(condition)) unreachable()
#else
#define abu_assume(condition) \
  if (!(condition)) abu_assert(condition)
#endif
#define abu_precondition(condition) abu_assume(condition)

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
}  // namespace abu

namespace abu {
template <typename T>
concept Policies = requires {
  { T::vector_of_tokens_are_strings } -> std::convertible_to<bool>;
};
struct default_policies_type {
  static constexpr bool vector_of_tokens_are_strings = true;
};
static constexpr default_policies_type default_policies{};
}  // namespace abu

namespace abu {
struct real_pattern_tag {};
struct strong_pattern_tag {};
struct weak_pattern_tag {};
template <typename T>
struct pattern_traits {
  using pattern_category = typename T::pattern_category;
  template <Token Tok, Policies auto policies>
  using value_type = typename T::template value_type<Tok, policies>;
};
template <typename T>
using pattern_category_t = typename pattern_traits<T>::pattern_category;
template <typename T>
concept Pattern = std::is_same_v<pattern_category_t<T>, real_pattern_tag>;
template <typename PatLikeT, Token Tok, Policies auto policies>
using parsed_value_t =
    typename pattern_traits<PatLikeT>::template value_type<Tok, policies>;
template <typename T>
concept PatternConvertible = requires(T x) {
  { pattern_traits<T>::to_pattern(x) } -> Pattern;
};
template <typename T>
concept StrongPattern = PatternConvertible<T> &&
    std::same_as<pattern_category_t<T>, strong_pattern_tag>;
template <typename T>
concept WeakPattern = PatternConvertible<T> &&
    std::same_as<pattern_category_t<T>, strong_pattern_tag>;
template <typename T>
concept PatternLike = Pattern<T> || PatternConvertible<T>;
template <PatternConvertible T>
constexpr auto as_pattern(const T& p) {
  return pattern_traits<T>::to_pattern(p);
}
template <Pattern T>
constexpr const T& as_pattern(const T& p) {
  return p;
}
template <typename T, template <typename...> typename U>
concept PatternTemplate = requires(T x) {
  { U(x) } -> std::same_as<T>;
};
}  // namespace abu

#//  Copyright 2017-2021 Francois Chabot
#ifndef ABULAFIA_UTILS_H_INCLUDED
#define ABULAFIA_UTILS_H_INCLUDED
namespace abu::details_ {
struct noop_t {
  template <typename... Ts>
  constexpr void operator()(const Ts&...) const {}
};
inline constexpr noop_t noop;
}  // namespace abu::details_
#endif

namespace abu::coro {
template <Pattern auto pattern, Policies auto policies, DataSource Data>
class parser;
template <Pattern auto pattern, Policies auto policies, DataSource Data>
class matcher : public parser<pattern, policies, Data> {
  using parser<pattern, policies, Data>::parser;
 public:
  constexpr op_result on_tokens(Data& data) {
    return parser<pattern, policies, Data>::on_tokens(data, details_::noop);
  }
  constexpr op_result on_end(Data& data) {
    return parser<pattern, policies, Data>::on_end(data, details_::noop);
  }
};
namespace details_ {
template <typename T>
struct op_pattern_lookup;
template <Pattern auto pattern, Policies auto policies, DataSource Data>
struct op_pattern_lookup<parser<pattern, policies, Data>> {
  static constexpr auto value = pattern;
};
template <Pattern auto pattern, Policies auto policies, DataSource Data>
struct op_pattern_lookup<matcher<pattern, policies, Data>> {
  static constexpr auto value = pattern;
};
}  // namespace details_
template <typename T>
inline constexpr auto op_pattern = details_::op_pattern_lookup<T>::value;
template <typename T, auto pattern, typename Tok, auto policies>
concept ParseCallback = Token<Tok> &&
    requires(T x, parsed_value_t<decltype(pattern), Tok, policies>&& val) {
  { x(std::move(val)) } -> std::same_as<void>;
};
namespace details_ {
  template <typename Parser, Token Tok>
  struct parse_callback_archetype {
    static constexpr default_policies_type policies = default_policies;
    using value_t = parsed_value_t<decltype(op_pattern<Parser>), Tok, policies>;
    void operator()(value_t&&);
  };
}  // namespace details_
template <typename T, typename Data>
concept Parser =
    requires(T x,
             Data& d,
             const Data& cd,
             const details_::parse_callback_archetype<T, Data>& cb) {
  T(cd);
  { x.on_tokens(d, cb) } -> std::same_as<op_result>;
  { x.on_end(cb) } -> std::same_as<op_result>;
};
template <typename T, typename Data>
concept Matcher = requires(T x, Data& d, const Data& cd) {
  T(cd);
  { x.on_tokens(d) } -> std::same_as<op_result>;
  { x.on_end(d) } -> std::same_as<op_result>;
};
}  // namespace abu::coro

namespace abu::pat {
template <TokenSet TokSetT>
struct tok {
  using pattern_category = real_pattern_tag;
  using token_set_type = TokSetT;
  template <Token Tok, Policies auto policies>
  using value_type = Tok;
  [[no_unique_address]] token_set_type allowed;
};
template <Pattern Op>
struct repeat {
  using pattern_category = real_pattern_tag;
  template <Token Tok, Policies auto policies>
  using value_type = std::basic_string<char>;
  [[no_unique_address]] Op operand;
  std::size_t min;
  std::size_t max;
};
template <Pattern Op>
struct discard {
  using pattern_category = real_pattern_tag;
  using operand_type = Op;
  template <Token Tok, Policies auto policies>
  using value_type = void;
  [[no_unique_address]] operand_type operand;
};
/*
struct eoi_tag {};
struct fail_tag {};
struct pass_tag {};
struct fail {
  using pattern_tag = fail;
};
struct pass {
  using pattern_tag = pass;
};
template <TokenSet TokSetT>
struct tok {
  using pattern_tag = real_pattern_tag;
  using token_set_type = TokSetT;
  template <DataContext Ctx>
  using parsed_value_type = typename Ctx::token_type;
  [[no_unique_address]] token_set_type allowed;
};
template <std::ranges::range SeqT>
struct lit_seq {
  using pattern_tag = real_pattern_tag;
  template <DataContext>
  using parsed_value_type = void;
  [[no_unique_address]] SeqT expected;
};
template <Pattern Op>
struct raw {
  using pattern_tag = real_pattern_tag;
  using operand_type = Op;
  template <DataContext Ctx>
  using parsed_value_type = std::basic_string<typename Ctx::token_type>;
  [[no_unique_address]] operand_type operand;
};
namespace opt_ {
template <typename T>
struct value : public std::type_identity<std::optional<T>> {};
template <typename T>
struct value<std::optional<T>> : public std::type_identity<std::optional<T>> {};
template <>
struct value<void> : public std::type_identity<void> {};
template <typename T>
using value_t = typename value<T>::type;
}  // namespace opt_
template <Pattern Op>
struct optional {
  using pattern_tag = real_pattern_tag;
  using operand_type = Op;
  template <DataContext Ctx>
  using parsed_value_type = opt_::value_t<parsed_value_ctx_t<operand_type,
Ctx>>;
  [[no_unique_address]] operand_type operand;
};
template <Pattern Op, typename Act>
struct action {
  using pattern_tag = real_pattern_tag;
  using operand_type = Op;
  using action_type = Act;
  [[no_unique_address]] operand_type operand;
  [[no_unique_address]] action_type action;
};
template <Pattern Op, Pattern Delim>
struct list {
  using pattern_tag = real_pattern_tag;
  using operand_type = Op;
  using delimiter_type = Delim;
  template <DataContext Ctx>
  using parsed_value_type = std::vector<parsed_value_ctx_t<operand_type, Ctx>>;
  [[no_unique_address]] operand_type operand;
  [[no_unique_address]] delimiter_type modifier;
};
template <Pattern Op, Pattern Except>
struct except {
  using pattern_tag = real_pattern_tag;
  using operand_type = Op;
  using except_type = Except;
  template <DataContext Ctx>
  using parsed_value_type = parsed_value_ctx_t<operand_type, Ctx>;
  [[no_unique_address]] operand_type operand;
  [[no_unique_address]] except_type except;
};
template <Pattern... Ops>
struct seq {
  using pattern_tag = real_pattern_tag;
  using operands_type = std::tuple<Ops...>;
  [[no_unique_address]] operands_type operands;
};
template <Pattern... Ops>
struct alt {
  using pattern_tag = real_pattern_tag;
  using operands_type = std::tuple<Ops...>;
  [[no_unique_address]] operands_type operands;
};
*/
}  // namespace abu::pat

namespace abu::coro {
template <PatternTemplate<pat::discard> auto pattern,
          Policies auto policies,
          DataSource Data>
class matcher<pattern, policies, Data> {
 public:
  using operand_paraser = matcher<pattern.operand, policies, Data>;
  constexpr matcher(const Data& data) : child_(data) {}
  constexpr op_result on_tokens(Data& data) { return child_.on_tokens(data); }
  constexpr op_result on_end(Data& data) { return child_.on_end(data); }
 private:
  operand_paraser child_;
};
}  // namespace abu::coro

namespace abu::coro {
template <PatternTemplate<pat::repeat> auto pattern,
          Policies auto policies,
          DataSource Data>
class parser<pattern, policies, Data> {
 public:
  using token_type = typename Data::token_type;
  using value_type = parsed_value_t<decltype(pattern), token_type, policies>;
  using operand_parser = parser<pattern.operand, policies, Data>;
  using op_value_type =
      parsed_value_t<decltype(pattern.operand), token_type, policies>;
  constexpr parser(Data& data) : child_op_(data) {}
  template <ParseCallback<pattern, token_type, policies> CbT>
  constexpr op_result on_tokens(Data& data, const CbT& cb) {
    while (true) {
      auto res = child_op_.on_tokens(
          data, [this](op_value_type&& v) { result_.push_back(std::move(v)); });
      if (res.is_partial()) {
        return partial_result;
      }
      if (res.is_match_failure()) {
        return finish_(cb);
      }
      abu_assume(res.is_success());
      if (pattern.max != 0 && result_.size() == pattern.max) {
        return finish_(cb);
      } else {
        child_op_ = operand_parser{data};
      }
    }
  }
  template <ParseCallback<pattern, token_type, policies> CbT>
  constexpr op_result on_end(Data& data, const CbT& cb) {
    while (true) {
      auto res = child_op_.on_end(
          data, [this](op_value_type&& v) { result_.push_back(std::move(v)); });
      if (res.is_match_failure()) {
        return finish_(cb);
      }
      abu_assume(res.is_success());
      if (pattern.max != 0 && result_.size() == pattern.max) {
        return finish_(cb);
      } else {
        child_op_ = operand_parser{data};
      }
    }
  }
 private:
  template <ParseCallback<pattern, token_type, policies> CbT>
  constexpr op_result finish_(const CbT& cb) {
    if (result_.size() >= pattern.min) {
      cb(std::move(result_));
      return success;
    } else {
      return failure_t{};
    }
  }
  value_type result_;
  operand_parser child_op_;
};
}  // namespace abu::coro

namespace abu::coro {
template <PatternTemplate<pat::tok> auto pattern,
          Policies auto policies,
          DataSource Data>
class parser<pattern, policies, Data> {
 public:
  using token_type = typename Data::token_type;
  constexpr parser(const Data&) {}
  template <ParseCallback<pattern, token_type, policies> CbT>
  constexpr op_result on_tokens(Data& data, const CbT& cb) {
    if (data.empty()) {
      return partial_result;
    }
    if (!pattern.allowed(data.peek())) {
      return failure_t{};
    }
    cb(data.read());
    return success;
  }
  template <ParseCallback<pattern, token_type, policies> CbT>
  constexpr op_result on_end(Data&, const CbT&) {
    return failure_t{};
  }
};
}  // namespace abu::coro


namespace abu {
template <PatternLike auto pattern_like,
          Policies auto policies = default_policies,
          std::forward_iterator I,
          std::sentinel_for<I> S>
constexpr bool match(I& b, const S& e) {
  abu::data_chunk data{b, e};
  constexpr auto pattern = as_pattern(pattern_like);
  coro::matcher<pattern, policies, decltype(data)> matcher(data);
  auto status = matcher.on_tokens(data);
  if (status.is_partial()) {
    status = matcher.on_end(data);
  }
  return status.is_success();
}
template <PatternLike auto pattern,
          Policies auto policies = default_policies,
          std::ranges::forward_range R>
constexpr bool match(const R& range) {
  auto b = std::ranges::begin(range);
  auto e = std::ranges::end(range);
  return match<pattern, policies>(b, e);
}
template <PatternLike auto pattern_like,
          Policies auto policies = default_policies,
          std::forward_iterator I,
          std::sentinel_for<I> S>
constexpr auto parse(I& b, const S& e) {
  using token_type = std::iter_value_t<I>;
  abu::data_chunk data{b, e};
  constexpr auto pattern = as_pattern(pattern_like);
  coro::parser<pattern, policies, decltype(data)> parser(data);
  using result_type = parsed_value_t<decltype(pattern), token_type, policies>;
  std::optional<result_type> result;
  auto assign = [&](auto&& v) { result = std::move(v); };
  auto status = parser.on_tokens(data, assign);
  if (status.is_partial()) {
    status = parser.on_end(data, assign);
  }
  if (!status.is_success()) {
    throw no_match_error{};
  }
  return std::move(*result);
}
template <PatternLike auto pattern,
          Policies auto policies = default_policies,
          std::ranges::forward_range R>
constexpr auto parse(const R& range) {
  auto b = std::ranges::begin(range);
  auto e = std::ranges::end(range);
  return parse<pattern, policies>(b, e);
}
}  // namespace abu

namespace abu {
namespace _api {
struct tok_api {
  template <TokenSet T>
  constexpr auto operator()(T tokset) const {
    return pat::tok{std::move(tokset)};
  }
};
}  // namespace _api
template <>
struct pattern_traits<_api::tok_api> {
  using pattern_category = strong_pattern_tag;
  static constexpr auto to_pattern(const _api::tok_api&) {
    return pat::tok<any_token>{any_token{}};
  }
};
static constexpr _api::tok_api tok;
template <TokenSet TokSet>
struct pattern_traits<TokSet> {
  using pattern_category = weak_pattern_tag;
  static constexpr auto to_pattern(TokSet tok_set) {
    return pat::tok<TokSet>{std::move(tok_set)};
  }
};
template <PatternLike OpT>
inline constexpr auto repeat(const OpT& pat_like, std::size_t min, std::size_t max) {
  return pat::repeat{as_pattern(pat_like), min, max};
}
inline constexpr auto discard(PatternLike auto pat_like) {
  return pat::discard{as_pattern(pat_like)};
}
}  // namespace abu

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif
