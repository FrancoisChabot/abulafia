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
#include <deque>
#include <iterator>
#include <memory>
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
  constexpr data_chunk(I begin, S end)
      : next_(std::move(begin)), end_(std::move(end)) {}
  constexpr token_type peek() const { return *next_; }
  constexpr bool empty() const { return next_ == end_; }
  constexpr token_type read() {
    auto result = *next_;
    ++next_;
    return result;
  }
 private:
  I next_;
  [[no_unique_address]] S end_;
};
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
template <typename ChunkData>
struct data_feed_chunk {
  ChunkData data;
  std::shared_ptr<data_feed_chunk> next_chunk;
};
template <std::ranges::input_range ChunkData>
struct input_checkpoint {
  std::shared_ptr<data_feed_chunk<ChunkData>> chunk;
};
template <std::ranges::forward_range ChunkData>
struct forward_checkpoint {
  using pointer_type = std::shared_ptr<data_feed_chunk<ChunkData>>;
  using iterator_type = std::ranges::iterator_t<ChunkData>;
  pointer_type chunk;
  iterator_type next;
};
template <std::ranges::input_range ChunkData>
class basic_data_feed {
  using chunk_type = data_feed_chunk<ChunkData>;
  using data_iterator_type = std::ranges::iterator_t<ChunkData>;
  using data_sentinel_type = std::ranges::sentinel_t<ChunkData>;
 public:
  using token_type = std::ranges::range_value_t<ChunkData>;
  constexpr void add(ChunkData chunk) {
    auto chunk_ptr = std::make_shared<data_feed_chunk<ChunkData>>(
        data_feed_chunk{std::move(chunk), {}});
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
  std::shared_ptr<chunk_type> current_chunk_;
  chunk_type* last_chunk_ = nullptr;
  data_iterator_type current_chunk_next_;
  data_sentinel_type current_chunk_end_;
};
template <std::ranges::input_range ChunkData>
struct data_feed : public basic_data_feed<ChunkData> {
  using token_type = std::ranges::range_value_t<ChunkData>;
 private:
  std::deque<token_type> rollback_buffer_;
};
template <std::ranges::forward_range ChunkData>
struct data_feed<ChunkData> : public basic_data_feed<ChunkData> {
  using checkpoint_type = forward_checkpoint<ChunkData>;
  constexpr checkpoint_type make_checkpoint() const {
    return checkpoint_type{this->current_chunk_, this->current_chunk_next_};
  }
  constexpr void rollback(checkpoint_type cp) {
    this->current_chunk_ = std::move(cp.chunk);
    this->current_chunk_next_ = cp.next;
    this->current_chunk_end_ = std::ranges::end(this->current_chunk_->data);
  }
};
}  // namespace abu

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
concept Policies = requires(T x) {
  { x.repeat.tokens_to_string } -> std::convertible_to<bool>;
};
struct default_policies_type {
  struct {
    bool tokens_to_string = true;
  } repeat;
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
concept Pattern = std::is_same_v<pattern_category_t<T>, real_pattern_tag> &&
    requires(const T v) {
  { v.can_fail() } -> std::convertible_to<bool>;
  { v.consumes_on_success() } -> std::convertible_to<bool>;
  { v.consumes_on_failure() } -> std::convertible_to<bool>;
};
template <typename PatLikeT,
          Token Tok,
          Policies auto policies = default_policies>
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
  constexpr bool can_fail() const { return true; }
  constexpr bool consumes_on_success() const { return true; }
  constexpr bool consumes_on_failure() const { return false; }
};
template <Pattern Op>
struct repeat {
  using pattern_category = real_pattern_tag;
  template <Token Tok, Policies auto policies>
  using value_type = std::conditional_t<
      (std::is_same_v<Tok, parsed_value_t<Op, Tok, policies>> && policies.repeat.tokens_to_string),
      std::basic_string<Tok>,
      std::vector<parsed_value_t<Op, Tok, policies>>>;
  [[no_unique_address]] Op operand;
  std::size_t min;
  std::size_t max;
  constexpr bool can_fail() const { return min > 0 && operand.can_fail(); }
  constexpr bool consumes_on_success() const {
    return operand.consumes_on_success();
  }
  constexpr bool consumes_on_failure() const {
    return can_fail() &&
           (operand.consumes_on_success() || operand.consumes_on_failure());
  }
};
template <Pattern Op>
struct discard {
  using pattern_category = real_pattern_tag;
  template <Token Tok, Policies auto policies>
  using value_type = void;
  [[no_unique_address]] Op operand;
  constexpr bool can_fail() const { return Op::can_fail(); }
  constexpr bool consumes_on_success() const {
    return Op::consumes_on_success();
  }
  constexpr bool consumes_on_failure() const {
    return Op::consumes_on_failure();
  }
};
struct eoi {
  using pattern_category = real_pattern_tag;
  template <Token Tok, Policies auto policies>
  using value_type = void;
  constexpr bool can_fail() const { return true; }
  constexpr bool consumes_on_success() const { return false; }
  constexpr bool consumes_on_failure() const { return false; }
};
struct pass {
  using pattern_category = real_pattern_tag;
  template <Token Tok, Policies auto policies>
  using value_type = void;
  constexpr bool can_fail() const { return false; }
  constexpr bool consumes_on_success() const { return false; }
  constexpr bool consumes_on_failure() const { return false; }
};
struct fail {
  using pattern_category = real_pattern_tag;
  template <Token Tok, Policies auto policies>
  using value_type = void;
  constexpr bool can_fail() const { return true; }
  constexpr bool consumes_on_success() const { return false; }
  constexpr bool consumes_on_failure() const { return false; }
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
  constexpr matcher(const Data& data) : child_(data) {}
  constexpr op_result on_tokens(Data& data) { return child_.on_tokens(data); }
  constexpr op_result on_end(Data& data) { return child_.on_end(data); }
 private:
  matcher<pattern.operand, policies, Data> child_;
};
}  // namespace abu::coro

namespace abu::coro {
template <std::same_as<pat::eoi> auto pattern,
          Policies auto policies,
          DataSource Data>
class matcher<pattern, policies, Data> {
 public:
  constexpr matcher(const Data&) {}
  constexpr op_result on_tokens(Data&) { return partial_result; }
  constexpr op_result on_end(Data& d) {
    if (d.empty()) {
      return success;
    } else {
      return failure_t{};
    }
  }
};
template <std::same_as<pat::pass> auto pattern,
          Policies auto policies,
          DataSource Data>
class matcher<pattern, policies, Data> {
 public:
  constexpr matcher(const Data&) {}
  constexpr op_result on_tokens(Data&) { return success; }
  constexpr op_result on_end(Data&) { return success; }
};
template <std::same_as<pat::fail> auto pattern,
          Policies auto policies,
          DataSource Data>
class matcher<pattern, policies, Data> {
 public:
  constexpr matcher(const Data&) {}
  constexpr op_result on_tokens(Data&) { return failure_t{}; }
  constexpr op_result on_end(Data&) { return failure_t{}; }
};
}  // namespace abu::coro

namespace abu::coro {
namespace details_ {
template <DataSource Data, bool rollback_enabled = true>
class maybe_checkpoint {
 public:
  using checkpoint_type = Data::checkpoint_type;
  maybe_checkpoint(Data& data) : checkpoint_(data.make_checkpoint()) {}
  constexpr void reset(Data& data) { checkpoint_ = data.make_checkpoint(); }
  constexpr void rollback(Data& data) { data.rollback(std::move(checkpoint_)); }
 private:
  checkpoint_type checkpoint_;
};
template <DataSource Data>
class maybe_checkpoint<Data, false> {
 public:
  constexpr maybe_checkpoint(Data& data) {}
  constexpr void reset(Data& data) {}
  constexpr void rollback(Data& data){};
};
template <PatternTemplate<pat::repeat> auto pattern,
          Policies auto policies,
          DataSource Data,
          bool parser>
class basic_repeat {
  using checkpoint_type =
      maybe_checkpoint<Data, pattern.operand.consumes_on_failure()>;
  using child_op_type =
      std::conditional_t<parser,
                         coro::parser<pattern.operand, policies, Data>,
                         coro::matcher<pattern.operand, policies, Data>>;
  using token_type = typename Data::token_type;
  using value_type = parsed_value_t<decltype(pattern), token_type, policies>;
  using result_type =
      std::conditional_t<parser, value_type, abu::details_::null_vector_sink_t>;
 public:
  basic_repeat(Data& data) : checkpoint_(data), child_op(data) {}
  template <typename Cb>
  constexpr bool on_tokens_done(Data& data,
                                op_result child_res,
                                const Cb& final_cb,
                                op_result& out_result) {
    if (child_res.is_partial()) {
      return false;
    }
    if (child_res.is_match_failure()) {
      out_result = finalize_(final_cb);
      if (out_result.is_success()) {
        checkpoint_.rollback(data);
      }
      return false;
    }
    abu_assume(child_res.is_success());
    return handle_child_success(data, final_cb, out_result);
  }
  template <typename Cb>
  constexpr bool on_end_done(Data& data,
                             op_result child_res,
                             const Cb& final_cb,
                             op_result& out_result) {
    if (child_res.is_match_failure()) {
      out_result = finalize_(final_cb);
      if (out_result.is_success()) {
        checkpoint_.rollback(data);
      }
      return false;
    }
    abu_assume(child_res.is_success());
    return handle_child_success(data, final_cb, out_result);
  }
  template <typename Cb>
  constexpr bool handle_child_success(Data& data,
                                      const Cb& cb,
                                      op_result& out_result) {
    if (pattern.max != 0 && result.size() == pattern.max) {
      out_result = finalize_(cb);
      return false;
    }
    child_op = child_op_type{data};
    return true;
  }
  template <typename Cb>
  constexpr op_result finalize_(const Cb& cb) {
    if (result.size() >= pattern.min) {
      cb(std::move(result));
      return success;
    } else {
      return failure_t{};
    }
  }
  [[no_unique_address]] checkpoint_type checkpoint_;
  child_op_type child_op;
  result_type result;
};
}  // namespace details_
template <PatternTemplate<pat::repeat> auto pattern,
          Policies auto policies,
          DataSource Data>
class parser<pattern, policies, Data> {
  using token_type = typename Data::token_type;
  details_::basic_repeat<pattern, policies, Data, true> base_;
 public:
  constexpr parser(Data& data) : base_(data) {}
  template <ParseCallback<pattern, token_type, policies> CbT>
  constexpr op_result on_tokens(Data& data, const CbT& cb) {
    op_result result = partial_result;
    bool keep_going = true;
    while (keep_going) {
      auto child_res = base_.child_op.on_tokens(
          data, [&](auto v) { base_.result.push_back(std::move(v)); });
      keep_going = base_.on_tokens_done(data, child_res, cb, result);
    }
    return result;
  }
  template <ParseCallback<pattern, token_type, policies> CbT>
  constexpr op_result on_end(Data& data, const CbT& cb) {
    op_result result = partial_result;
    bool keep_going = true;
    while (keep_going) {
      auto child_res = base_.child_op.on_end(
          data, [&](auto v) { base_.result.push_back(std::move(v)); });
      keep_going = base_.on_end_done(data, child_res, cb, result);
    }
    return result;
  }
};
template <PatternTemplate<pat::repeat> auto pattern,
          Policies auto policies,
          DataSource Data>
class matcher<pattern, policies, Data> {
  details_::basic_repeat<pattern, policies, Data, false> base_;
 public:
  constexpr matcher(Data& data) : base_(data) {}
  constexpr op_result on_tokens(Data& data) {
    op_result result = partial_result;
    bool keep_going = true;
    while (keep_going) {
      auto child_res = base_.child_op.on_tokens(data);
      if (child_res.is_success()) {
        base_.result.add();
      }
      keep_going =
          base_.on_tokens_done(data, child_res, abu::details_::noop, result);
    }
    return result;
  }
  constexpr op_result on_end(Data& data) {
    op_result result = partial_result;
    bool keep_going = true;
    while (keep_going) {
      auto child_res = base_.child_op.on_end(data);
      if (child_res.is_success()) {
        base_.result.add();
      }
      keep_going =
          base_.on_end_done(data, child_res, abu::details_::noop, result);
    }
    return result;
  }
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


namespace abu::imm {
template <Pattern auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct parser;
template <Pattern auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct matcher {
  static constexpr op_result match(I& i, const S& e) {
    abu::details_::null_sink_t null_sink;
    return parser<pattern, policies, I, S>::parse(null_sink, i, e);
  }
};
template <Pattern auto pattern,
          Policies auto policies = default_policies,
          typename Dst,
          std::input_iterator I,
          std::sentinel_for<I> S>
constexpr op_result parse(Dst& dst, I& i, const S& e) {
  return parser<pattern, policies, I, S>::parse(dst, i, e);
}
template <Pattern auto pattern,
          Policies auto policies = default_policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
constexpr op_result match(I& i, const S& e) {
  return matcher<pattern, policies, I, S>::match(i, e);
}
}  // namespace abu::imm

namespace abu::imm {
template <PatternTemplate<pat::discard> auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct matcher<pattern, policies, I, S> {
  static constexpr op_result match(I& i, const S& e) {
    using op_matcher = matcher<pattern.operand, policies, I, S>;
    return op_matcher::match(i, e);
  }
};
}  // namespace abu::imm

namespace abu::imm {
template <std::same_as<pat::eoi> auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct matcher<pattern, policies, I, S> {
  static constexpr op_result match(I& i, const S& e) {
    if (i == e) {
      return success;
    } else {
      return failure_t{};
    }
  }
};
template <std::same_as<pat::pass> auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct matcher<pattern, policies, I, S> {
  static constexpr op_result match(I& i, const S& e) { return success; }
};
template <std::same_as<pat::fail> auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct matcher<pattern, policies, I, S> {
  static constexpr op_result match(I& i, const S& e) { return failure_t{}; }
};
}  // namespace abu::imm

namespace abu::imm {
namespace details_ {
template <PatternTemplate<pat::repeat> auto pattern,
          Policies auto policies,
          std::input_iterator I>
struct basic_repeat {
  static constexpr bool is_done(std::size_t count) {
    return pattern.max > 0 && count >= pattern.max;
  }
  static constexpr op_result final_result(std::size_t count) {
    if (count >= pattern.min) {
      return success;
    } else {
      return failure_t{};
    }
  }
};
}  // namespace details_
template <PatternTemplate<pat::repeat> auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct parser<pattern, policies, I, S>
    : public details_::basic_repeat<pattern, policies, I> {
  template <typename Dst>
  static constexpr op_result parse(Dst& dst, I& i, const S& e) {
    using basic = details_::basic_repeat<pattern, policies, I>;
    using token_type = std::iter_value_t<I>;
    using value_type = parsed_value_t<decltype(pattern), token_type, policies>;
    using op_value_type =
        parsed_value_t<decltype(pattern.operand), token_type, policies>;
    using op_parser = parser<pattern.operand, policies, I, S>;
    value_type result_val;
    while (1) {
      std::optional<op_value_type> landing;
      auto child_res = op_parser::parse(landing, i, e);
      if (child_res.is_success()) {
        abu_assume(landing);
        result_val.push_back(*landing);
        if (basic::is_done(result_val.size())) {
          break;
        }
      } else {
        break;
      }
    }
    auto result = basic::final_result(result_val.size());
    if(result.is_success()) {
      dst = result_val;
    }
    return result;
  }
};
template <PatternTemplate<pat::repeat> auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct matcher<pattern, policies, I, S>
    : public details_::basic_repeat<pattern, policies, I> {
  static constexpr op_result match(I& i, const S& e) {
    using basic = details_::basic_repeat<pattern, policies, I>;
    using token_type = std::iter_value_t<I>;
    using value_type = parsed_value_t<decltype(pattern), token_type, policies>;
    using op_value_type =
        parsed_value_t<decltype(pattern.operand), token_type, policies>;
    using op_matcher = matcher<pattern.operand, policies, I, S>;
    std::size_t count = 0;
    while (1) {
      auto child_res = op_matcher::match(i, e);
      if (basic::is_done(count)) {
        ++count;
        if (pattern.max > 0 && count >= pattern.max) {
          break;
        }
      } else {
        break;
      }
    }
    return basic::final_result(count);
  }
};
}  // namespace abu::imm

namespace abu::imm {
template <PatternTemplate<pat::tok> auto pattern,
          Policies auto policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
struct parser<pattern, policies, I, S> {
  template <typename Dst>
  static constexpr op_result parse(Dst& dst, I& i, const S& e) {
    if (i != e && pattern.allowed(*i)) {
      dst = *i++;
      return success;
    }
    return failure_t{};
  }
};
}  // namespace abu::imm


namespace abu {
template <PatternLike auto pattern_like,
          Policies auto policies = default_policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
constexpr bool match(I& b, const S& e) {
  constexpr auto pattern = as_pattern(pattern_like);
  abu::data_chunk data{b, e};
  abu::coro::matcher<pattern, policies, decltype(data)> matcher(data);
  op_result status = matcher.on_tokens(data);
  if (status.is_partial()) {
    status = matcher.on_end(data);
  }
  return status.is_success();
}
template <PatternLike auto pattern_like,
          Policies auto policies = default_policies,
          std::input_iterator I,
          std::sentinel_for<I> S>
constexpr auto parse(I& b, const S& e) {
  using token_type = std::iter_value_t<I>;
  constexpr auto pattern = as_pattern(pattern_like);
  using result_type = parsed_value_t<decltype(pattern), token_type, policies>;
  std::optional<result_type> result;
  auto on_success = [&](result_type v) { result = v; };
  abu::data_chunk data{b, e};
  abu::coro::parser<pattern, policies, decltype(data)> matcher(data);
  op_result status = matcher.on_tokens(data, on_success);
  if (status.is_partial()) {
    status = matcher.on_end(data, on_success);
  }
  if (!status.is_success()) {
    throw no_match_error{};
  }
  return std::move(*result);
}
template <PatternLike auto pattern,
          Policies auto policies = default_policies,
          std::ranges::input_range R>
constexpr bool match(const R& range) {
  auto b = std::ranges::begin(range);
  auto e = std::ranges::end(range);
  return match<pattern, policies>(b, e);
}
template <PatternLike auto pattern,
          Policies auto policies = default_policies,
          std::ranges::input_range R>
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
static constexpr pat::eoi eoi;
static constexpr pat::pass pass;
static constexpr pat::fail fail;
}  // namespace abu

#ifdef __clang__
#pragma clang diagnostic pop
#endif

#endif
