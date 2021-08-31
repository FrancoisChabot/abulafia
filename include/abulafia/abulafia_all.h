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
#//  Copyright 2017-2021 Francois Chabot
#ifndef ABULAFIA_STD_UTILS_H_INCLUDED
#define ABULAFIA_STD_UTILS_H_INCLUDED
namespace abu {
namespace std_utils {
namespace archetypes {
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
using input_iterator = std_utils::archetypes::input_iterator<char>;
using input_iterator_sentinel = std_utils::archetypes::input_iterator_sentinel;
using forward_iterator = std_utils::archetypes::forward_iterator<char>;
using forward_iterator_sentinel =
    std_utils::archetypes::forward_iterator_sentinel;
}  // namespace archetypes
}  // namespace abu
#endif

namespace abu {
template <typename T>
concept Policy = requires {
  { T::vector_of_tokens_are_strings } -> std::convertible_to<bool>;
};
struct default_policy {
  static constexpr bool vector_of_tokens_are_strings = true;
};
}  // namespace abu

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
struct real_pattern_tag {
  using pattern_tag = real_pattern_tag;
};
struct strong_pattern_tag {
  using pattern_tag = strong_pattern_tag;
};
struct weak_pattern_tag {
  using pattern_tag = weak_pattern_tag;
};
template <typename T>
struct pattern_traits {
  using pattern_tag = typename T::pattern_tag;
};
template <typename T>
using pattern_tag_t = typename pattern_traits<T>::pattern_tag;
template <typename T>
concept Pattern = std::same_as<pattern_tag_t<T>, real_pattern_tag>;
template <typename T>
concept PatternConvertible = requires(T x) {
  { pattern_traits<T>::to_pattern(x) } -> Pattern;
};
template <typename T>
concept PatternLike = Pattern<T> || PatternConvertible<T>;
template <typename T>
concept StrongPattern =
    PatternConvertible<T> && std::same_as<pattern_tag_t<T>, strong_pattern_tag>;
template <typename T>
concept WeakPattern =
    PatternConvertible<T> && std::same_as<pattern_tag_t<T>, strong_pattern_tag>;
template <PatternConvertible T>
constexpr auto as_pattern(const T& p) {
  return pattern_traits<T>::to_pattern(p);
}
template <Pattern T>
constexpr const T& as_pattern(const T& p) {
  return p;
}
enum class op_type {
  parse,
  match,
};
template <typename T>
concept Context = requires {
  typename T::token_type;
  typename T::policies;
  { T::operation_type } -> std::convertible_to<op_type>;
};
template <typename T>
concept ParseContext = Context<T> && (T::operation_type == op_type::parse);
template <typename T>
concept MatchContext = Context<T> && (T::operation_type == op_type::match);
template <Token Tok, op_type OpType, Policy Pol = default_policy>
struct context {
  using token_type = Tok;
  using policies = Pol;
  static constexpr op_type operation_type = OpType;
};
template <typename T, Context Ctx>
using pattern_value_t = typename T::template value_type<Ctx>;
}  // namespace abu

namespace abu::coro {
template <typename T>
concept InputContext = Context<T> && requires(T x) {
  typename T::token_type;
  typename T::iterator_type;
  typename T::sentinel_type;
  std::input_iterator<typename T::iterator_type>;
  { x.iterator } -> std::same_as<typename T::iterator_type&>;
  { x.end } -> std::convertible_to<typename T::sentinel_type>;
};
template <typename T>
concept ForwardContext = InputContext<T> &&
    requires(T x, typename T::checkpoint_type cp) {
  typename T::checkpoint_type;
  x.rollback(cp);
  { x.checkpoint() } -> std::same_as<typename T::checkpoint_type>;
};
template <Context Ctx, Pattern Pat = typename Ctx::pattern_type>
class operation;
template <std::input_iterator I,
          std::sentinel_for<I> S,
          Pattern Pat,
          op_type OpType,
          Policy Pol = default_policy>
struct context : public ::abu::context<std::iter_value_t<I>, OpType, Pol> {
  static constexpr op_type operation_type = OpType;
  using pattern_type = Pat;
  using iterator_type = I;
  using sentinel_type = S;
  using value_type =
      pattern_value_t<Pat, ::abu::context<std::iter_value_t<I>, OpType, Pol>>;
  constexpr context(I& init_ite, S init_end, const Pat& pat)
      : iterator(init_ite), end(init_end), pattern(pat) {}
  iterator_type& iterator;
  sentinel_type end;
  const pattern_type& pattern;
};
template <std::forward_iterator I,
          std::sentinel_for<I> S,
          Pattern Pat,
          op_type OpType,
          Policy Pol>
struct context<I, S, Pat, OpType, Pol>
    : public ::abu::context<std::iter_value_t<I>, OpType, Pol> {
  static constexpr op_type operation_type = OpType;
  using pattern_type = Pat;
  using iterator_type = I;
  using sentinel_type = S;
  using value_type =
      pattern_value_t<Pat, ::abu::context<std::iter_value_t<I>, OpType, Pol>>;
  using checkpoint_type = I;
  constexpr context(I& init_ite, S init_end, const Pat& pat)
      : iterator(init_ite), end(init_end), pattern(pat) {}
  checkpoint_type checkpoint() const { return iterator; }
  void rollback(checkpoint_type cp) const { iterator = cp; }
  iterator_type& iterator;
  sentinel_type end;
  const pattern_type& pattern;
};
template <typename T, template <typename... U> typename P>
concept ContextFor = Context<T> && Pattern<typename T::pattern_type> &&
    requires(typename T::pattern_type x) {
  { P(x) } -> std::same_as<typename T::pattern_type>;
};
template <typename T, typename P>
concept ContextForTag = Context<T> && Pattern<typename T::pattern_type> &&
    std::is_same_v<typename T::pattern_type, P>;
template <typename T, template <typename... U> typename P>
concept MatchContextFor = MatchContext<T> &&
    Pattern<typename T::pattern_type> && requires(typename T::pattern_type x) {
  { P(x) } -> std::same_as<typename T::pattern_type>;
};
template <typename T, template <typename... U> typename P>
concept ParseContextFor = ParseContext<T> &&
    Pattern<typename T::pattern_type> && requires(typename T::pattern_type x) {
  { P(x) } -> std::same_as<typename T::pattern_type>;
};
}  // namespace abu::coro

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
class match_failure_t {};
class match_failure_error : public std::exception {
 public:
  match_failure_error(match_failure_t f) : fail_(std::move(f)) {}
 private:
  match_failure_t fail_;
};
template <typename T>
class parse_result {
 public:
  using value_type = T;
  constexpr parse_result(const T& val) : payload_(val) {}
  constexpr parse_result(T&& val) : payload_(std::move(val)) {}
  constexpr parse_result(match_failure_t&& err) : payload_(std::move(err)) {}
  constexpr bool is_success() const { return payload_.index() == 0; }
  constexpr bool is_match_failure() const { return payload_.index() == 1; }
  constexpr operator bool() const { return is_success(); }
  constexpr T& value() {
    switch (payload_.index()) {
      case 0:
        return std::get<0>(payload_);
        break;
      case 1:
        throw match_failure_error{std::get<1>(payload_)};
    }
    abu::unreachable();
  }
  constexpr const T& value() const {
    switch (payload_.index()) {
      case 0:
        return std::get<0>(payload_);
        break;
      case 1:
        throw match_failure_error{std::get<1>(payload_)};
    }
    abu::unreachable();
  }
  constexpr match_failure_t& match_failure() {
    abu_assume(is_match_failure());
    return std::get<1>(payload_);
  }
  constexpr const match_failure_t& match_failure() const {
    abu_assume(is_match_failure());
    return std::get<1>(payload_);
  }
 private:
  std::variant<T, match_failure_t> payload_;
};
template <>
class parse_result<void> {
 public:
  using value_type = void;
  constexpr parse_result() = default;
  constexpr parse_result(match_failure_t&& err) : payload_(std::move(err)) {}
  constexpr bool is_success() const { return !payload_.has_value(); }
  constexpr bool is_match_failure() const { return payload_.has_value(); }
  constexpr operator bool() const { return is_success(); }
  constexpr match_failure_t& match_failure() {
    abu_assume(is_match_failure());
    return *payload_;
  }
  constexpr const match_failure_t& match_failure() const {
    abu_assume(is_match_failure());
    return *payload_;
  }
 private:
  std::optional<match_failure_t> payload_;
};
}  // namespace abu

namespace abu::coro {
struct partial_result_tag {};
struct void_value_tag {};
class partial_result_error : public std::exception {};
template <typename T>
class parse_result {
 public:
  using value_type = T;
  constexpr parse_result(const T& val) : payload_(val) {}
  constexpr parse_result(T&& val) : payload_(std::move(val)) {}
  constexpr parse_result(match_failure_t&& err) : payload_(std::move(err)) {}
  constexpr parse_result(partial_result_tag val) : payload_(val) {}
  constexpr bool is_success() const { return payload_.index() == 0; }
  constexpr bool is_match_failure() const { return payload_.index() == 1; }
  constexpr bool is_partial() const { return payload_.index() == 2; }
  constexpr operator ::abu::parse_result<T>() {
    if (is_success()) {
      return ::abu::parse_result<T>{std::move(value())};
    }
    if (is_match_failure()) {
      return ::abu::parse_result<T>{std::move(match_failure())};
    }
    throw partial_result_error{};
  }
  constexpr T& value() {
    switch (payload_.index()) {
      case 0:
        return std::get<0>(payload_);
        break;
      case 1:
        throw match_failure_error{std::get<1>(payload_)};
        break;
      case 2:
        throw partial_result_error{};
        break;
    }
    abu::unreachable();
  }
  constexpr const T& value() const {
    switch (payload_.index()) {
      case 0:
        return std::get<0>(payload_);
        break;
      case 1:
        throw match_failure_error{std::get<1>(payload_)};
        break;
      case 2:
        throw partial_result_error{};
        break;
    }
    abu::unreachable();
  }
  constexpr match_failure_t& match_failure() {
    abu_assume(is_match_failure());
    return std::get<1>(payload_);
  }
  constexpr const match_failure_t& match_failure() const {
    abu_assume(is_match_failure());
    return std::get<1>(payload_);
  }
 private:
  std::variant<T, match_failure_t, partial_result_tag> payload_;
};
template <>
class parse_result<void> {
 public:
  using value_type = void;
  constexpr parse_result() : payload_(void_value_tag{}) {}
  constexpr parse_result(match_failure_t&& f) : payload_(std::move(f)) {}
  constexpr parse_result(partial_result_tag val) : payload_(val) {}
  constexpr bool is_success() const { return payload_.index() == 0; }
  constexpr bool is_match_failure() const { return payload_.index() == 1; }
  constexpr bool is_partial() const { return payload_.index() == 2; }
  constexpr operator ::abu::parse_result<void>() && {
    if (is_success()) {
      return {};
    }
    if (is_match_failure()) {
      return ::abu::parse_result<value_type>{std::move(error())};
    }
    throw std::runtime_error("partial results");
  }
  constexpr match_failure_t& error() {
    abu_assume(is_match_failure());
    return std::get<1>(payload_);
  }
  constexpr const match_failure_t& error() const {
    abu_assume(is_match_failure());
    return std::get<1>(payload_);
  }
 private:
  using storage_type =
      std::variant<void_value_tag, match_failure_t, partial_result_tag>;
  storage_type payload_;
};
}  // namespace abu::coro

namespace abu::coro {
template <Context ParentCtx,
          auto Mem,
          op_type OpType = ParentCtx::operation_type>
class child_op {
  static_assert(std::is_member_object_pointer_v<decltype(Mem)>);
 public:
  using parent_context_type = ParentCtx;
  using pattern_type =
      std::decay_t<decltype(std::declval<ParentCtx>().pattern.*Mem)>;
  static constexpr op_type operation_type = OpType;
  using sub_context_type = context<typename ParentCtx::iterator_type,
                                   typename ParentCtx::sentinel_type,
                                   pattern_type,
                                   operation_type,
                                   typename ParentCtx::policies>;
  using value_type =
      std::conditional_t<OpType == op_type::match,
                         void,
                         pattern_value_t<pattern_type, sub_context_type> >;
  constexpr sub_context_type make_sub_context(const ParentCtx& ctx) {
    return sub_context_type(ctx.iterator, ctx.end, ctx.pattern.*Mem);
  }
  constexpr child_op(const ParentCtx& ctx) : op_(make_sub_context(ctx)) {}
  constexpr parse_result<value_type> on_tokens(const ParentCtx& ctx) {
    return op_.on_tokens(make_sub_context(ctx));
  }
  constexpr abu::parse_result<value_type> on_end(const ParentCtx& ctx) {
    return op_.on_end(make_sub_context(ctx));
  }
  void reset(const ParentCtx& ctx) {
    op_ = operation<sub_context_type, pattern_type>{make_sub_context(ctx)};
  }
 private:
  operation<sub_context_type, pattern_type> op_;
};
template <std::size_t>
struct child_index_type {};
template <std::size_t N>
inline constexpr child_index_type<N> child_index;
template <typename First, typename... Rest>
class child_op_set {
  using current_type = std::variant<First, Rest...>;
 public:
  using parent_context_type = typename First::parent_context_type;
  constexpr child_op_set(const parent_context_type& ctx)
      : current_(std::in_place_index_t<0>{}, ctx) {}
  constexpr std::size_t index() const { return current_.index(); }
  template <std::size_t Orig, std::size_t Target>
  constexpr auto reset(child_index_type<Orig>,
                       child_index_type<Target>,
                       const parent_context_type& ctx) {
    abu_assume(current_.index() == Orig);
    current_ = current_type(std::in_place_index_t<Target>{}, ctx);
  }
  template <std::size_t Index>
  constexpr auto on_tokens(child_index_type<Index>,
                           const parent_context_type& ctx) {
    abu_assume(current_.index() == Index);
    return std::get<Index>(current_).on_tokens(ctx);
  }
  template <std::size_t Index>
  constexpr auto on_end(child_index_type<Index>,
                        const parent_context_type& ctx) {
    abu_assume(current_.index() == Index);
    return std::get<Index>(current_).on_end(ctx);
  }
 private:
  current_type current_;
};
}  // namespace abu::coro

namespace abu::pat {
struct eoi {
  using pattern_tag = real_pattern_tag;
  template <Context>
  using value_type = void;
};
struct fail {
  using pattern_tag = real_pattern_tag;
  template <Context>
  using value_type = void;
};
struct pass {
  using pattern_tag = real_pattern_tag;
  template <Context>
  using value_type = void;
};
template <TokenSet TokSetT>
struct tok {
  using pattern_tag = real_pattern_tag;
  using token_set_type = TokSetT;
  template <Context Ctx>
  using value_type = typename Ctx::token_type;
  [[no_unique_address]] token_set_type allowed;
};
template <std::ranges::range SeqT>
struct lit_seq {
  using pattern_tag = real_pattern_tag;
  template <Context>
  using value_type = void;
  [[no_unique_address]] SeqT expected;
};
template <Pattern Op>
struct raw {
  using pattern_tag = real_pattern_tag;
  using operand_type = Op;
  template <Context Ctx>
  using value_type = std::basic_string<typename Ctx::token_type>;
  [[no_unique_address]] operand_type operand;
};
template <Pattern Op>
struct discard {
  using pattern_tag = real_pattern_tag;
  using operand_type = Op;
  template <Context>
  using value_type = void;
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
  template <Context Ctx>
  using value_type = opt_::value_t<pattern_value_t<operand_type, Ctx>>;
  [[no_unique_address]] operand_type operand;
};
namespace repeat_ {
template <typename T, std::size_t Min, std::size_t Max>
struct value : public std::type_identity<std::vector<T>> {};
template <std::size_t Min, std::size_t Max>
struct value<char, Min, Max>
    : public std::type_identity<std::basic_string<char>> {};
template <typename T, std::size_t Min, std::size_t Max>
using value_t = typename value<T, Min, Max>::type;
}  // namespace repeat_
template <Pattern Op, std::size_t Min, std::size_t Max>
struct repeat {
  static_assert(Max == 0 || Max >= Min);
  static constexpr std::size_t min_reps = Min;
  static constexpr std::size_t max_reps = Max;
  using pattern_tag = real_pattern_tag;
  using operand_type = Op;
  template <Context Ctx>
  using value_type =
      repeat_::value_t<pattern_value_t<operand_type, Ctx>, Min, Max>;
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
  template <Context Ctx>
  using value_type = std::vector<pattern_value_t<operand_type, Ctx>>;
  [[no_unique_address]] operand_type operand;
  [[no_unique_address]] delimiter_type modifier;
};
template <Pattern Op, Pattern Except>
struct except {
  using pattern_tag = real_pattern_tag;
  using operand_type = Op;
  using except_type = Except;
  template <Context Ctx>
  using value_type = pattern_value_t<operand_type, Ctx>;
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
}  // namespace abu::pat

namespace abu::coro {
template <ContextFor<pat::discard> Ctx>
class operation<Ctx> {
 public:
  using pattern_type = typename Ctx::pattern_type;
  using child_type = child_op<Ctx, &pattern_type::operand, op_type::match>;
  constexpr operation(const Ctx& ctx) : child_op_(ctx) {}
  constexpr parse_result<void> on_tokens(const Ctx& ctx) {
    return child_op_.on_tokens(ctx);
  }
  constexpr ::abu::parse_result<void> on_end(const Ctx& ctx) {
    return child_op_.on_end(ctx);
  }
 private:
  child_type child_op_;
};
}  // namespace abu::coro

namespace abu::coro {
template <ContextFor<pat::except> Ctx>
class operation<Ctx> {
 public:
  using pattern_type = typename Ctx::pattern_type;
  using except_child_op = child_op<Ctx, &pattern_type::except, op_type::match>;
  using operand_child_op = child_op<Ctx, &pattern_type::operand>;
  using value_type = typename operand_child_op::value_type;
  // ***** constructor ***** //
  constexpr operation(const Ctx& ctx) : child_op_(ctx) {}
  // ***** on_tokens ***** //
  constexpr parse_result<value_type> on_tokens(const Ctx& ctx) {
    // Test for the excluded pattern
    if (child_op_.index() == 0) {
      auto tmp = child_op_.on_tokens(child_index<0>, ctx);
      if (tmp.is_success()) {
        return match_failure_t{};
      } else if (tmp.is_match_failure()) {
        child_op_.reset(child_index<0>, child_index<1>, ctx);
      }
    }
    // Propagate to the operand parser.
    if (child_op_.index() == 1) {
      return child_op_.on_tokens(child_index<1>, ctx);
    }
    return partial_result_tag{};
  }
  // ***** on_end ***** //
  constexpr ::abu::parse_result<value_type> on_end(const Ctx& ctx) {
    if (child_op_.index() == 0) {
      auto tmp = child_op_.on_end(child_index<0>, ctx);
      if (tmp.is_success()) {
        return match_failure_t{};
      }
      child_op_.reset(child_index<0>, child_index<1>, ctx);
    }
    abu_assume(child_op_.index() == 1);
    return child_op_.on_end(child_index<1>, ctx);
  }
 private:
  child_op_set<except_child_op, operand_child_op> child_op_;
};
}  // namespace abu::coro

namespace abu::coro {
template <ContextForTag<pat::eoi> Ctx>
class operation<Ctx> {
 public:
  using pattern_type = typename Ctx::pattern_type;
  using value_type = void;
  constexpr operation(const Ctx&) {}
  constexpr parse_result<void> on_tokens(const Ctx& ctx) {
    if (ctx.iterator != ctx.end) {
      return match_failure_t{};
    }
    return partial_result_tag{};
  }
  constexpr ::abu::parse_result<void> on_end(const Ctx&) { return {}; }
};
template <ContextForTag<pat::fail> Ctx>
class operation<Ctx> {
 public:
  using pattern_type = typename Ctx::pattern_type;
  constexpr operation(const Ctx&) {}
  constexpr parse_result<void> on_tokens(const Ctx&) {
    return match_failure_t{};
  }
  constexpr ::abu::parse_result<void> on_end(const Ctx&) {
    return match_failure_t{};
  }
};
template <ContextForTag<pat::pass> Ctx>
class operation<Ctx> {
 public:
  using pattern_type = typename Ctx::pattern_type;
  constexpr operation(const Ctx&) {}
  constexpr parse_result<void> on_tokens(const Ctx&) { return {}; }
  constexpr ::abu::parse_result<void> on_end(const Ctx&) { return {}; }
};
}  // namespace abu::coro

namespace abu::coro {
template <ParseContext Ctx, Pattern Op, std::size_t Min, std::size_t Max>
class operation<Ctx, pat::repeat<Op, Min, Max>> {
  static_assert(ForwardContext<Ctx>);
 public:
  using pattern_type = pat::repeat<Op, Min, Max>;
  using child_type = child_op<Ctx, &pattern_type::operand>;
  using checkpoint_type = typename Ctx::checkpoint_type;
  using value_type = pattern_value_t<pattern_type, Ctx>;
  constexpr operation(const Ctx& ctx)
      : checkpoint_{ctx.checkpoint()}, child_op_(ctx) {}
  constexpr parse_result<value_type> on_tokens(const Ctx& ctx) {
    while (true) {
      auto res = child_op_.on_tokens(ctx);
      if (res.is_partial()) {
        return partial_result_tag{};
      }
      if (res.is_match_failure()) {
        return finish_(ctx);
      }
      abu_assume(res.is_success());
      result_.push_back(std::move(res.value()));
      if (Max != 0 && result_.size() == Max) {
        return finish_(ctx);
      } else {
        child_op_.reset(ctx);
      }
    }
  }
  constexpr ::abu::parse_result<value_type> on_end(const Ctx& ctx) {
    while (true) {
      auto res = child_op_.on_end(ctx);
      if (res.is_match_failure()) {
        return finish_(ctx);
      }
      abu_assume(res.is_success());
      result_.push_back(std::move(res.value()));
      if (Max != 0 && result_.size() == Max) {
        return finish_(ctx);
      } else {
        child_op_.reset(ctx);
      }
    }
  }
 private:
  constexpr parse_result<value_type> finish_(const Ctx& ctx) {
    if (result_.size() >= Min) {
      return std::move(result_);
    } else {
      ctx.rollback(checkpoint_);
      return match_failure_t{};
    }
  }
  checkpoint_type checkpoint_;
  value_type result_;
  child_type child_op_;
};
}  // namespace abu::coro

namespace abu::coro {
template <ParseContextFor<pat::tok> Ctx>
class operation<Ctx> {
  static_assert(InputContext<Ctx>);
 public:
  using value_type = typename Ctx::value_type;
  constexpr operation(const Ctx&) {}
  constexpr parse_result<value_type> on_tokens(const Ctx& ctx) {
    if (ctx.iterator != ctx.end) {
      if (!ctx.pattern.allowed(*ctx.iterator)) {
        return match_failure_t{};
      }
      return *ctx.iterator++;
    }
    return partial_result_tag{};
  }
  constexpr ::abu::parse_result<value_type> on_end(const Ctx&) {
    return match_failure_t{};
  }
};
template <MatchContextFor<pat::tok> Ctx>
class operation<Ctx> {
  static_assert(InputContext<Ctx>);
 public:
  constexpr operation(const Ctx&) {}
  constexpr parse_result<void> on_tokens(const Ctx& ctx) {
    if (ctx.iterator != ctx.end) {
      if (!ctx.pattern.allowed(*ctx.iterator)) {
        return match_failure_t{};
      }
      return {};
    }
    return partial_result_tag{};
  }
  constexpr ::abu::parse_result<void> on_end(const Ctx&) {
    return match_failure_t{};
  }
};
}  // namespace abu::coro


namespace abu {
template <std::forward_iterator I, std::sentinel_for<I> S, Pattern Pat>
constexpr auto parse(I& b, const S& e, const Pat& pat) {
  using context_type = abu::coro::context<I, S, Pat, op_type::parse>;
  using parse_op_type = abu::coro::operation<context_type>;
  context_type root_ctx{b, e, pat};
  parse_op_type root_parser{root_ctx};
  auto status = root_parser.on_tokens(root_ctx);
  if (status.is_partial()) {
    return std::move(root_parser.on_end(root_ctx).value());
  }
  return std::move(status.value());
}
template <std::forward_iterator I,
          std::sentinel_for<I> S,
          PatternConvertible Pat>
constexpr auto parse(I& b, const S& e, const Pat& pat_like) {
  return parse(b, e, as_pattern(pat_like));
}
template <std::ranges::forward_range R, PatternLike Pat>
constexpr auto parse(const R& range, const Pat& pat) {
  auto b = std::ranges::begin(range);
  auto e = std::ranges::end(range);
  return parse(b, e, pat);
}
template <std::forward_iterator I, std::sentinel_for<I> S, Pattern Pat>
constexpr auto try_parse(I& b, const S& e, const Pat& pat) {
  using context_type = abu::coro::context<I, S, Pat, op_type::parse>;
  using parse_op_type = abu::coro::operation<context_type>;
  context_type root_ctx{b, e, pat};
  parse_op_type root_parser{root_ctx};
  auto status = root_parser.on_tokens(root_ctx);
  if (status.is_partial()) {
    status = std::move(root_parser.on_end(root_ctx));
  }
  return status;
}
template <std::forward_iterator I,
          std::sentinel_for<I> S,
          PatternConvertible Pat>
constexpr auto try_parse(I& b, const S& e, const Pat& pat_like) {
  return try_parse(b, e, as_pattern(pat_like));
}
template <std::ranges::forward_range R, PatternLike Pat>
constexpr auto try_parse(const R& range, const Pat& pat) {
  auto b = std::ranges::begin(range);
  auto e = std::ranges::end(range);
  return try_parse(b, e, pat);
}
template <std::forward_iterator I, std::sentinel_for<I> S, Pattern Pat>
constexpr bool match(I& b, const S& e, const Pat& pat) {
  using context_type = abu::coro::context<I, S, Pat, op_type::match>;
  using match_op_type = abu::coro::operation<context_type>;
  context_type root_ctx{b, e, pat};
  match_op_type root_matcher{root_ctx};
  auto status = root_matcher.on_tokens(root_ctx);
  if (status.is_partial()) {
    return root_matcher.on_end(root_ctx).is_success();
  }
  return status.is_success();
}
template <std::forward_iterator I,
          std::sentinel_for<I> S,
          PatternConvertible Pat>
constexpr auto match(I& b, const S& e, const Pat& pat_like) {
  return match(b, e, as_pattern(pat_like));
}
template <std::ranges::forward_range R, PatternLike Pat>
constexpr auto match(const R& range, const Pat& pat) {
  auto b = std::ranges::begin(range);
  auto e = std::ranges::end(range);
  return match(b, e, pat);
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
static constexpr _api::tok_api tok;
template <>
struct pattern_traits<_api::tok_api> {
  using pattern_tag = strong_pattern_tag;
  static constexpr auto to_pattern(const _api::tok_api&) {
    return pat::tok<any_token>{any_token{}};
  }
};
template <TokenSet TokSet>
struct pattern_traits<TokSet> {
  using pattern_tag = weak_pattern_tag;
  static constexpr auto to_pattern(TokSet tok_set) {
    return pat::tok<TokSet>{std::move(tok_set)};
  }
};
static constexpr pat::eoi eoi;
static constexpr pat::pass pass;
static constexpr pat::fail fail;
inline constexpr auto discard(PatternLike auto pat_like) {
  auto pat = as_pattern(pat_like);
  return pat::discard<decltype(pat)>{std::move(pat)};
}
inline constexpr auto except(PatternLike auto operand, PatternLike auto exception) {
  auto operand_pat = as_pattern(operand);
  auto exception_pat = as_pattern(exception);
  return pat::except<decltype(operand_pat), decltype(exception_pat)>{std::move(operand_pat), std::move(exception_pat)};
}
static constexpr auto opt(PatternLike auto pat_like) {
  return pat::optional{as_pattern(pat_like)};
}
template <std::size_t Min, std::size_t Max>
static constexpr auto repeat(PatternLike auto pat_like) {
  using Op = std::decay_t<decltype(as_pattern(pat_like))>;
  return pat::repeat<Op, Min, Max>{as_pattern(pat_like)};
}
}  // namespace abu

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif
