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
concept Policy = requires {
  { T::vector_of_tokens_are_strings } -> std::convertible_to<bool>;
};
struct default_policy {
  static constexpr bool vector_of_tokens_are_strings = true;
};
}  // namespace abu

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
concept Token = std::is_copy_constructible_v<T>;
template <typename T>
concept TokenSet = std::predicate<T, archetypes::token>;
struct any_token {
  constexpr bool operator()(const auto&) const { return true; }
};
}  // namespace abu

namespace abu {
enum class op_type {
  parse,
  match,
  // Not an actual op type
  any,
};
template <typename T>
concept DataContext = requires {
  typename T::token_type;
  typename T::policies;
};
template <Token Tok, Policy Pol>
struct data_context {
  using token_type = Tok;
  using policies = Pol;
};
}  // namespace abu

namespace abu {
struct real_pattern_tag {};
struct strong_pattern_tag {};
struct weak_pattern_tag {};
template <typename T>
struct pattern_traits {
  using pattern_category = typename T::pattern_category;
};
template <typename T>
concept Pattern = std::same_as<typename pattern_traits<T>::pattern_category,
                               real_pattern_tag>;
template <typename T>
using pattern_category_t = typename pattern_traits<T>::pattern_category;
template <typename T, DataContext Ctx>
using parsed_value_ctx_t = typename T::template parsed_value_type<Ctx>;
template <typename T, Token Tok, Policy Pol = default_policy>
using parsed_value_t = parsed_value_ctx_t<T, data_context<Tok, Pol>>;
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
template <typename T>
concept OpContext =
    DataContext<T> && Pattern<typename T::pattern_type> && requires {
  { T::operation_type } -> std::same_as<const op_type>;
};
template <typename T>
concept MatchContext = OpContext<T> &&(T::operation_type == op_type::match);
template <typename T>
concept ParseContext =
    OpContext<T> &&(T::operation_type == op_type::parse) && requires {
  typename T::value_type;
};
template <Token Tok, Policy Pol, Pattern Pat, op_type OpType>
struct op_context;
template <Token Tok, Policy Pol, Pattern Pat>
struct op_context<Tok, Pol, Pat, op_type::parse> {
  static constexpr op_type operation_type = op_type::parse;
  using token_type = Tok;
  using policies = Pol;
  using pattern_type = Pat;
  using value_type = parsed_value_t<Pat, token_type, policies>;
};
template <Token Tok, Policy Pol, Pattern Pat>
struct op_context<Tok, Pol, Pat, op_type::match> {
  static constexpr op_type operation_type = op_type::match;
  using token_type = Tok;
  using policies = Pol;
  using pattern_type = Pat;
};
template <typename T, typename PatTag>
concept IsPatternOf =
    Pattern<T> && std::is_same_v<typename T::pattern_tag, PatTag>;
template <typename T, typename PatTag>
concept PatternContext =
    DataContext<T> && IsPatternOf<typename T::pattern_type, PatTag>;
}  // namespace abu

namespace abu::coro {
template <typename T>
class operation;
template <std::input_iterator I,
          std::sentinel_for<I> S,
          Policy Pol,
          Pattern Pat>
struct context
    : public op_context<std::iter_value_t<I>, Pol, Pat, op_type::match> {
  using iterator_type = I;
  using sentinel_type = S;
  constexpr context(I& init_ite, S init_end, const Pat& pat)
      : iterator(init_ite), end(init_end), pattern(pat) {}
  iterator_type& iterator;
  sentinel_type end;
  const Pat& pattern;
};
template <typename T, typename PatTag, op_type OpType = op_type::any>
concept ContextFor = PatternContext<T, PatTag> &&
    (T::operation_type == OpType || OpType == op_type::any);
}  // namespace abu::coro

namespace abu::coro {
template <OpContext ParentCtx,
          auto Mem,
          op_type OpType = ParentCtx::operation_type>
class child_op {
  static_assert(std::is_member_object_pointer_v<decltype(Mem)>);
 public:
  using parent_context_type = ParentCtx;
  using pattern_type =
      std::decay_t<decltype(std::declval<ParentCtx>().pattern.*Mem)>;
  using sub_context_type = context<typename ParentCtx::iterator_type,
                                   typename ParentCtx::sentinel_type,
                                   typename ParentCtx::policies,
                                   pattern_type>;
  constexpr sub_context_type make_sub_context(const ParentCtx& ctx) {
    return sub_context_type(ctx.iterator, ctx.end, ctx.pattern.*Mem);
  }
  constexpr child_op(const ParentCtx& ctx) : op_(make_sub_context(ctx)) {}
  template <typename CbT>
  constexpr op_result on_tokens(const ParentCtx& ctx, const CbT& cb) {
    return op_.on_tokens(make_sub_context(ctx), cb);
  }
  template <typename CbT>
  constexpr op_result on_end(const ParentCtx& ctx, const CbT& cb) {
    return op_.on_end(make_sub_context(ctx), cb);
  }
  void reset(const ParentCtx& ctx) {
    op_ = operation<sub_context_type>{make_sub_context(ctx)};
  }
 private:
  operation<sub_context_type> op_;
};
/*
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
*/
}  // namespace abu::coro

namespace abu::pat {
struct tok_tag {};
struct repeat_tag {};
template <TokenSet TokSetT>
struct tok {
  using pattern_category = real_pattern_tag;
  using pattern_tag = tok_tag;
  using token_set_type = TokSetT;
  template <DataContext Ctx>
  using parsed_value_type = typename Ctx::token_type;
  [[no_unique_address]] token_set_type allowed;
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
  using pattern_category = real_pattern_tag;
  using pattern_tag = repeat_tag;
  using operand_type = Op;
  template <DataContext Ctx>
  using parsed_value_type = std::basic_string<char>;
      // repeat_::value_t<parsed_value_ctx_t<operand_type, Ctx>, Min, Max>;
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
template <Pattern Op>
struct discard {
  using pattern_tag = real_pattern_tag;
  using operand_type = Op;
  template <DataContext>
  using parsed_value_type = void;
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
template <ContextFor<pat::repeat_tag> Ctx>
class operation<Ctx> {
 public:
  using pattern_type = typename Ctx::pattern_type;
  using child_type = child_op<Ctx, &pattern_type::operand>;
  //using checkpoint_type = typename Ctx::checkpoint_type;
  static constexpr std::size_t min_reps = pattern_type::min_reps;
  static constexpr std::size_t max_reps = pattern_type::max_reps;
  using value_type = parsed_value_t<pattern_type, Ctx>;
  constexpr operation(const Ctx& ctx)
      : /*checkpoint_{ctx.checkpoint()},*/ child_op_(ctx) {}
  template <typename CbT>
  constexpr op_result on_tokens(const Ctx& ctx, const CbT& cb) {
    while (true) {
      auto res = child_op_.on_tokens(
          ctx, [this](char v) { result_.push_back(std::move(v));});
      if (res.is_partial()) {
        return partial_result;
      }
      if (res.is_match_failure()) {
        return finish_(ctx, cb);
      }
      abu_assume(res.is_success());
      if (max_reps != 0 && result_.size() == max_reps) {
        return finish_(ctx, cb);
      } else {
        child_op_.reset(ctx);
      }
    }
  }
  template <typename CbT>
  constexpr op_result on_end(const Ctx& ctx, const CbT& cb) {
    while (true) {
      auto res = child_op_.on_end(
          ctx, [this](char v) { result_.push_back(std::move(v));});
      if (res.is_match_failure()) {
        return finish_(ctx, cb);
      }
      abu_assume(res.is_success());
      if (max_reps != 0 && result_.size() == max_reps) {
        return finish_(ctx, cb);
      } else {
        child_op_.reset(ctx);
      }
    }
  }
 private:
   template <typename CbT>
  constexpr op_result finish_(const Ctx&, const CbT& cb) {
    if (result_.size() >= min_reps) {
      cb(std::move(result_));
      return success;
    } else {
     // ctx.rollback(checkpoint_);
      return failure_t{};
    }
  }
 // checkpoint_type checkpoint_;
  value_type result_;
  child_type child_op_;
};
}  // namespace abu::coro

namespace abu::coro {
template <ContextFor<pat::tok_tag> Ctx>
class operation<Ctx> {
 public:
  constexpr operation(const Ctx&) {}
  template <typename CbT>
  constexpr op_result on_tokens(const Ctx& ctx, const CbT& cb) {
    if (ctx.iterator == ctx.end) {
      return partial_result;
    }
    if (!ctx.pattern.allowed(*ctx.iterator)) {
      return failure_t{};
    }
    cb(*ctx.iterator++);
    return success;
  }
  template <typename CbT>
  constexpr op_result on_end(const Ctx&, const CbT&) {
    return failure_t{};
  }
};
}  // namespace abu::coro


namespace abu {
}  // namespace abu

namespace abu {
template <Policy Pol = default_policy,
          std::forward_iterator I,
          std::sentinel_for<I> S,
          Pattern Pat>
constexpr auto parse(I& b, const S& e, const Pat& pat) {
  using result_type = parsed_value_t<Pat, std::iter_value_t<I>>;
  std::optional<result_type> result;
  auto result_assign = [&](result_type r) { result = std::move(r); };
  using root_ctx = coro::context<I, S, Pol, Pat>;
  using root_op = coro::operation<root_ctx>;
  root_ctx ctx{b, e, pat};
  root_op op{ctx};
  auto status = op.on_tokens(ctx, result_assign);
  if (status.is_partial()) {
    status = op.on_end(ctx, result_assign);
  }
  if (status.is_match_failure()) {
    throw no_match_error{};
  }
  return std::move(*result);
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
template <Policy Pol = default_policy,
          std::forward_iterator I,
          std::sentinel_for<I> S,
          Pattern Pat>
constexpr bool match(I& b, const S& e, const Pat& pat) {
  using root_ctx = coro::context<I, S, Pol, Pat>;
  using root_op = coro::operation<root_ctx>;
  root_ctx ctx{b, e, pat};
  root_op op{ctx};
  auto status = op.on_tokens(ctx, [](auto){});
  if (status.is_partial()) {
    status = op.on_end(ctx, [](auto){});
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
  using pattern_category = strong_pattern_tag;
  static constexpr auto to_pattern(const _api::tok_api&) {
    return pat::tok<any_token>{any_token{}};
  }
};
template <TokenSet TokSet>
struct pattern_traits<TokSet> {
  using pattern_category = weak_pattern_tag;
  static constexpr auto to_pattern(TokSet tok_set) {
    return pat::tok<TokSet>{std::move(tok_set)};
  }
};
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
