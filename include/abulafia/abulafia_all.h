//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_SINGLE_INCLUDE_H_
#define ABULAFIA_SINGLE_INCLUDE_H_
#include <bitset>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <exception>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <variant>
#include <vector>

#ifndef ABULAFIA_NAMESPACE
#define ABULAFIA_NAMESPACE abu
#endif

namespace ABULAFIA_NAMESPACE {
namespace char_set {
struct CharacterSet {};
template <typename T>
struct is_char_set : public std::is_base_of<CharacterSet, T> {};
template<typename T, typename Enable=void>
struct to_char_set_impl;
template<typename T>
struct to_char_set_impl<T, std::enable_if_t<is_char_set<T>::value>> {
  static T const& convert(T const& v) {
    return v;
  }
};
template<typename T>
auto to_char_set(T v) {
  return to_char_set_impl<T>::convert(v);
}
}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
namespace char_set {
template <typename CHAR_T>
struct Any : public CharacterSet {
  using char_t = CHAR_T;
  bool is_valid(char_t const &) const {
    return true;
  }
};
template <typename CHAR_T>
Any<CHAR_T> any;
}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename T>
struct function_traits : public function_traits<decltype(&T::operator())> {};
template <typename ClassType, typename ReturnType, typename... Args>
struct function_traits<ReturnType (ClassType::*)(Args...) const>
{
  enum { arity = sizeof...(Args) };
  typedef ReturnType result_type;
  template <size_t i>
  struct arg {
    using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
  };
};
template <typename ReturnType, typename... Args>
struct function_traits<ReturnType (*)(Args...)> {
  // Specialization for function pointers
  typedef ReturnType result_type;
  typedef ReturnType function_type(Args...);
  enum { arity = sizeof...(Args) };
  template <size_t i>
  struct arg {
    typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
  };
};
template <typename CALLABLE_T, std::size_t Index>
using callable_argument_t =
    typename function_traits<CALLABLE_T>::template arg<Index>::type;
template <typename CALLABLE_T>
using callable_result_t = typename function_traits<CALLABLE_T>::result_type;
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
namespace char_set {
template <typename CHAR_T, typename CB_T>
struct DelegatedSet : public CharacterSet {
  using char_t = CHAR_T;
  explicit DelegatedSet(CB_T cb) : cb_(std::move(cb)) {}
  bool is_valid(char_t const& c) const {
    return cb_(c);
  }
 private:
  CB_T cb_;
};
template <typename CHAR_T, typename CB_T>
auto delegated(CB_T const& cb) {
  return DelegatedSet<CHAR_T, CB_T>(cb);
}
template <typename CB_T>
auto delegated(CB_T const& cb) {
  using char_t = callable_argument_t<CB_T, 0>;
  return DelegatedSet<char_t, CB_T>(cb);
}
}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
namespace char_set {
template <typename ARG_T>
struct Not : public CharacterSet {
  using char_t = typename ARG_T::char_t;
  explicit Not(ARG_T arg) : arg_(std::move(arg)) {}
  bool is_valid(char_t const& c) const {
    return !arg_.is_valid(c);
  }
 private:
  ARG_T arg_;
  static_assert(is_char_set<ARG_T>::value);
};
template <typename ARG_T>
struct is_char_set<Not<ARG_T>> : public std::true_type {};
template <typename ARG_T,
          typename Enable = std::enable_if_t<is_char_set<ARG_T>::value>>
Not<ARG_T> operator~(ARG_T arg) {
  return Not<ARG_T>{std::move(arg)};
}
}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
using std::decay_t;
using std::enable_if_t;
using std::forward;
using std::is_same;
using std::tuple_element_t;
template <typename T, typename... ARGS_T>
struct is_one_of : public std::false_type {};
template <typename T, typename U, typename... REST_T>
struct is_one_of<T, U, REST_T...>
    : public std::conditional_t<std::is_same<T, U>::value, std::true_type,
                                is_one_of<T, REST_T...>> {};
template <typename T>
struct is_tuple : public std::false_type {};
template <typename... ARGS_T>
struct is_tuple<std::tuple<ARGS_T...>> : public std::true_type {};
template <class T>
struct blank_type_ {
  typedef void type;
};
template <typename T, typename Enable = void>
struct is_collection : public std::false_type {};
template <typename T>
struct is_collection<T, typename blank_type_<typename T::value_type>::type>
    : public std::true_type {};
template <typename T, typename ENABLE = void>
struct reset_if_collection {
  static void exec(T&) {}
};
template <typename T>
struct reset_if_collection<T, enable_if_t<is_collection<T>::value>> {
  static void exec(T& c) { c.clear(); }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
namespace char_set {
template <typename LHS_T, typename RHS_T>
struct Or : public CharacterSet {
  using char_t = typename LHS_T::char_t;
  Or(LHS_T lhs, RHS_T rhs) : lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}
  bool is_valid(char_t const& c) const {
    return lhs_.is_valid(c) || rhs_.is_valid(c);
  }
 private:
  LHS_T lhs_;
  RHS_T rhs_;
  static_assert(is_char_set<LHS_T>::value);
  static_assert(is_char_set<RHS_T>::value);
  static_assert(is_same<
                  typename LHS_T::char_t,
                  typename RHS_T::char_t>::value);
};
template <typename LHS_T, typename RHS_T>
auto or_impl(LHS_T lhs, RHS_T rhs) {
  auto lhs_cs = to_char_set(std::decay_t<LHS_T>(lhs));
  auto rhs_cs = to_char_set(std::decay_t<RHS_T>(rhs));
  return Or<decltype(lhs_cs), decltype(rhs_cs)>(lhs_cs, rhs_cs) ;
}
template <typename LHS_T, typename RHS_T, typename=enable_if_t<is_char_set<LHS_T>::value ||
  is_char_set<RHS_T>::value>>
auto operator|(LHS_T lhs, RHS_T rhs) {
  return or_impl(lhs, rhs);
}
}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
#if defined(_MSC_VER)
inline __declspec(noreturn) void unreachable() {}
#endif
}
#if defined(__GNUC__) && defined(NDEBUG)
#define abu_assume(condition) \
  if (!(condition)) __builtin_unreachable()
#elif defined(__clang__) && defined(NDEBUG)
#define abu_assume(condition) \
  if (!(condition)) std::terminate();
#else
#define abu_assume(condition) assert(condition)
#endif
#if defined(__GNUC__)
#define abu_unreachable() __builtin_unreachable()
#elif defined(__clang__)
#define abu_unreachable() __builtin_unreachable()
#elif defined(_MSC_VER)
#define abu_unreachable() unreachable()
#endif

namespace ABULAFIA_NAMESPACE {
namespace char_set {
template <typename CHAR_T>
struct Range : public CharacterSet {
  using char_t = CHAR_T;
  Range(CHAR_T b, CHAR_T e) : begin_(b), end_(e) {
    assert(b <= e);
  }
  bool is_valid(char_t const& token) const {
    return token >= begin_ && token <= end_;
  }
 private:
  CHAR_T begin_;
  CHAR_T end_;
};
template <typename CHAR_T>
inline auto range(CHAR_T b, CHAR_T e) {
  return Range<CHAR_T>(b, e);
}
}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
namespace char_set {
template <typename CHAR_T>
struct Set : public CharacterSet {
  using char_t = CHAR_T;
  template <typename ITE_T>
  Set(ITE_T b, ITE_T e) : characters_(b, e) {}
  template <typename T>
  bool is_valid(char_t const& character) const {
    return characters_.find(character) != characters_.end();
  }
  std::set<CHAR_T> characters_;
};
template<typename CHAR_T>
struct IndexedSet : public CharacterSet {
  using char_t = CHAR_T;
  template <typename ITE_T>
  IndexedSet(ITE_T b, ITE_T e) {
    for (; b != e; ++b ) {
      characters_[as_index(*b)] = true;
    }
  }
  bool is_valid(const char_t& c) const {
    return characters_.test(as_index(c));
  }
private:
  using unsigned_t = std::make_unsigned_t<CHAR_T>;
  static constexpr std::size_t as_index(CHAR_T c) {
    return unsigned_t(c);
  }
  std::bitset<std::numeric_limits<unsigned_t>::max()+1> characters_;
};
template <>
struct Set<char> : public IndexedSet<char> {
  using IndexedSet::IndexedSet;
};
template <typename ITE>
auto set(ITE b, ITE e) {
  return Set<typename ITE::value_type>(b, e);
}
inline auto set(char const* v) {
  return Set<char>(v, v + std::strlen(v));
}
template<>
struct to_char_set_impl<const char*, void> {
  static Set<char> convert(char const* v) {
    return set(v);
  }
};
}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
namespace char_set {
template <typename CHAR_T>
struct Single : public CharacterSet {
  using char_t = CHAR_T;
  explicit Single(CHAR_T c) : character_(c) {}
  template <typename T>
  bool is_valid(T const& token) const {
    return token == character_;
  }
 private:
  CHAR_T character_;
};
template <typename CHAR_T>
auto single(CHAR_T c) {
  return Single<CHAR_T>(c);
}
template<>
struct to_char_set_impl<char, void> {
  static Single<char> convert(char const& v) {
    return Single<char>(v);
  }
};
}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
enum class IsFinal { FINAL, NOT_FINAL };
template <typename CONTAINER_T>
class ContainerSequenceContext {
  using iterator = typename CONTAINER_T::const_iterator;
  using buffer_list_t = std::list<std::shared_ptr<CONTAINER_T>>;
  using buffer_iterator = typename buffer_list_t::iterator;
  buffer_list_t buffers_;
  bool final_ = false;
  iterator current_;
  buffer_iterator current_buffer_;
  using rollback_entry_t = std::pair<iterator, buffer_iterator>;
  std::vector<rollback_entry_t> rollback_stack_;
 public:
  using value_type = typename CONTAINER_T::value_type;
  using base_ctx_t = ContainerSequenceContext<CONTAINER_T>;
  base_ctx_t& root_ctx() { return *this; }
  enum { IS_RESUMABLE = true };
  ContainerSequenceContext() : current_buffer_(buffers_.end()) {}
  void add_buffer(std::shared_ptr<CONTAINER_T> b,
                  IsFinal f = IsFinal::NOT_FINAL) {
    assert(!final_);
    // push_back can invalidate our iterator.
    bool is_empty = empty();
    buffers_.push_back(b);
    // if we were empty, bootstrap.
    if (is_empty) {
      current_buffer_ = std::prev(buffers_.end());
      current_ = (*current_buffer_)->begin();
    }
    final_ = f == IsFinal::FINAL;
  }
  void add_buffer(std::unique_ptr<CONTAINER_T> b,
                  IsFinal f = IsFinal::NOT_FINAL) {
    add_buffer(std::shared_ptr<CONTAINER_T>(std::move(b)), f);
  }
  void add_buffer(CONTAINER_T b, IsFinal f = IsFinal::NOT_FINAL) {
    add_buffer(std::make_shared<CONTAINER_T>(std::move(b)), f);
  }
  bool final_buffer() const { return final_; }
  value_type next() const {
    assert(!empty());
    return *current_;
  }
  void advance() {
    current_++;
    if (current_ == (*current_buffer_)->end()) {
      auto next_buffer = std::next(current_buffer_);
      if (next_buffer != buffers_.end()) {
        current_ = (*next_buffer)->begin();
      } else {
        current_ = iterator();
      }
      // iI we are done with the front buffer, and the rollback stack
      // has no hold on it, dump the buffer.
      if (current_buffer_ == buffers_.begin()) {
        bool held_for_rollback =
            !rollback_stack_.empty() &&
            rollback_stack_.front().second == current_buffer_;
        if (!held_for_rollback) {
          buffers_.pop_front();
        }
      }
      current_buffer_ = next_buffer;
    }
  }
  bool empty() const { return current_buffer_ == buffers_.end(); }
  void prepare_rollback() {
    assert(!empty());
    rollback_stack_.emplace_back(current_, current_buffer_);
  }
  void commit_rollback() {
    current_buffer_ = rollback_stack_.back().second;
    current_ = rollback_stack_.back().first;
    rollback_stack_.pop_back();
    cleanup_rollback_();
  }
  void cancel_rollback() {
    rollback_stack_.pop_back();
    cleanup_rollback_();
  }
 private:
  void cleanup_rollback_() {
    // The only hold that matters is the front of the rollback stack.
    // This means cleanup will only happen when the entire stack has unrolled.
    if (rollback_stack_.empty()) {
      while (current_buffer_ != buffers_.begin()) {
        buffers_.pop_front();
      }
    }
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename ITE_T>
class SingleForwardContext {
  using iterator = ITE_T;
  std::stack<iterator> rollback_stack_;
  iterator current_;
  iterator end_;
 public:
  enum {
    HAS_SKIPPER = false,
  };
  using value_type = decltype(*(ITE_T()));
  using base_ctx_t = SingleForwardContext<ITE_T>;
  base_ctx_t& root_ctx() { return *this; }
  enum { IS_RESUMABLE = false };
  SingleForwardContext(iterator b, iterator e) : current_(b), end_(e) {}
  constexpr bool final_buffer() const { return true; }
  void prepare_rollback() { rollback_stack_.push(current_); }
  void commit_rollback() {
    assert(!rollback_stack_.empty());
    current_ = rollback_stack_.top();
    rollback_stack_.pop();
  }
  void cancel_rollback() {
    assert(!rollback_stack_.empty());
    rollback_stack_.pop();
  }
  value_type next() const { return *current_; }
  void advance() {
    assert(!empty());
    current_++;
  }
  bool empty() const { return current_ == end_; }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename SKIP_PAT_T>
class SkipperAdapter {
  CTX_T& parent_ctx_;
  SKIP_PAT_T const& skip_pat_;
 public:
  enum { HAS_SKIPPER = true, IS_RESUMABLE = CTX_T::IS_RESUMABLE };
  using skip_pattern_t = SKIP_PAT_T;
  using value_type = typename CTX_T::value_type;
  using base_ctx_t = typename CTX_T::base_ctx_t;
  base_ctx_t& root_ctx() { return parent_ctx_.root_ctx(); }
  skip_pattern_t const& skipPattern() const { return skip_pat_; }
  SkipperAdapter(CTX_T& parent, SKIP_PAT_T const& skip)
      : parent_ctx_(parent), skip_pat_(skip) {}
  bool final_buffer() const { return parent_ctx_.final_buffer(); }
  void prepare_rollback() { parent_ctx_.prepare_rollback(); }
  void commit_rollback() { parent_ctx_.commit_rollback(); }
  void cancel_rollback() { parent_ctx_.cancel_rollback(); }
  value_type next() const { return parent_ctx_.next(); }
  void advance() { parent_ctx_.advance(); }
  bool empty() const { return parent_ctx_.empty(); }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
enum class result { SUCCESS, FAILURE, PARTIAL };
}

namespace ABULAFIA_NAMESPACE {
struct Nil {
  using value_type = Nil;
  Nil() = default;
  template <typename T>
  Nil(T&) {}
  // Can be assigned anything.
  template <typename T>
  Nil& operator=(T&&) {
    return *this;
  }
  // indirects to itself
  Nil* operator->() { return this; }
  // Used by the integer parser
  template <typename T>
  Nil& operator*=(const T&) {
    return *this;
  }
  template <typename T>
  Nil& operator+=(const T&) {
    return *this;
  }
  // Can pose as anything, as long as it can be default-constructed.
  template <typename T>
  operator T() const {
    return T();
  }
  // Can behave as a basic vector.
  void clear() {}
  template <typename T>
  void push_back(T&&) {}
  template <typename... T>
  void emplace_back(T&&...) {}
  template <typename T, typename U>
  void insert(T const&, U const&, U const&) {}
  Nil const& end() const { return *this; }
};
static Nil nil;
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
enum { PARSER_OPT_NO_SKIP = 1 };
template <typename CTX_T, typename DST_T, typename PAT_T>
class Parser : public Parser<CTX_T, DST_T, typename PAT_T::pattern_t> {
  using Parser<CTX_T, DST_T, typename PAT_T::pattern_t>::Parser;
  using Parser<CTX_T, DST_T, typename PAT_T::pattern_t>::operator=;
};
template <typename CTX_T, typename DST_T, typename PAT_T>
auto make_parser_(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
  return Parser<CTX_T, DST_T, PAT_T>(ctx, dst, pat);
}
template <typename CTX_T, typename DST_T, int OPT_V = 0, typename Enable = void>
class ParserBase;
template <typename CTX_T, typename DST_T, int OPT_V>
class ParserBase<CTX_T, DST_T, OPT_V,
                 std::enable_if_t<!CTX_T::HAS_SKIPPER ||
                                  (OPT_V & PARSER_OPT_NO_SKIP) != 0>> {
 public:
  constexpr ParserBase(CTX_T&, DST_T&) {}
  constexpr result performSkip(CTX_T&) { return result::SUCCESS; }
};
template <typename PARENT_CTX_T>
class SkipperContext {
  PARENT_CTX_T& parent_ctx_;
 public:
  SkipperContext(PARENT_CTX_T& parent) : parent_ctx_(parent) {}
  using value_type = typename PARENT_CTX_T::value_type;
  using base_ctx_t = typename PARENT_CTX_T::base_ctx_t;
  enum { HAS_SKIPPER = false, IS_RESUMABLE = PARENT_CTX_T::IS_RESUMABLE };
  bool final_buffer() const { return parent_ctx_.final_buffer(); }
  void prepare_rollback() { parent_ctx_.prepare_rollback(); }
  void commit_rollback() { parent_ctx_.commit_rollback(); }
  void cancel_rollback() { parent_ctx_.cancel_rollback(); }
  value_type next() const { return parent_ctx_.next(); }
  void advance() { parent_ctx_.advance(); }
  bool empty() const { return parent_ctx_.empty(); }
};
template <typename CTX_T, typename DST_T, int OPT_V>
class ParserBase<
    CTX_T, DST_T, OPT_V,
    std::enable_if_t<CTX_T::HAS_SKIPPER && (OPT_V & PARSER_OPT_NO_SKIP) == 0>> {
 public:
  using skip_pattern_t = typename CTX_T::skip_pattern_t;
  using skip_parser_t = Parser<SkipperContext<CTX_T>, Nil, skip_pattern_t>;
  ParserBase(CTX_T& ctx, DST_T&)
      : skip_parser_(force_lvalue(SkipperContext<CTX_T>(ctx)), nil,
                     ctx.skipPattern()) {
    ctx.prepare_rollback();
  }
  result performSkip(CTX_T& ctx) {
    if (skip_done_) {
      return result::SUCCESS;
    }
    SkipperContext<CTX_T> ctx_wrap(ctx);
    result success = result::SUCCESS;
    do {
      success = skip_parser_.consume(ctx_wrap, nil, ctx.skipPattern());
      switch (success) {
        case result::PARTIAL:
          return result::PARTIAL;
        case result::FAILURE:
          ctx.commit_rollback();
          skip_done_ = true;
          return result::SUCCESS;
        case result::SUCCESS:
          ctx.cancel_rollback();
          skip_parser_ = skip_parser_t(ctx_wrap, nil, ctx.skipPattern());
          ctx.prepare_rollback();
          break;
      }
    } while (success == result::SUCCESS);
    return result::FAILURE;
  }
 private:
  bool skip_done_ = false;
  skip_parser_t skip_parser_;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename T, typename Enable = void>
struct expr_traits {
  enum { is_pattern = false, converts_to_pattern = false };
  static void make_pattern(T) {
    // We use is_same here to ensure the assert does not get evaluated until
    // the template is instantiated.
    static_assert(!is_same<T, T>::value,
                  "type is not a pattern, or convertible to one");
  }
};
template <typename T>
constexpr bool is_pattern() {
  return expr_traits<decay_t<T>>::is_pattern;
}
template <typename T>
constexpr bool converts_to_pattern() {
  return expr_traits<decay_t<T>>::converts_to_pattern;
}
template <typename T>
constexpr bool is_valid_unary_operand() {
  return is_pattern<T>();
}
template <typename LHS_T, typename RHS_T>
constexpr bool are_valid_binary_operands() {
  return (is_pattern<LHS_T>() && converts_to_pattern<RHS_T>()) ||
         (converts_to_pattern<LHS_T>() && is_pattern<RHS_T>()) ||
         (is_pattern<LHS_T>() && is_pattern<RHS_T>());
}
template <typename T>
using pattern_t = decay_t<decltype(
    expr_traits<decay_t<T>>::make_pattern(*reinterpret_cast<decay_t<T>*>(0)))>;
template <typename T>
inline auto make_pattern(T&& p) {
  static_assert(expr_traits<decay_t<T>>::is_pattern ||
                    expr_traits<decay_t<T>>::converts_to_pattern,
                "Cannot create pattern from T");
  return expr_traits<decay_t<T>>::make_pattern(forward<T>(p));
}
template <typename T, typename CB_T>
auto transform(T const& tgt, CB_T const&) {
  return tgt;
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
struct default_pattern_traits {
  enum {
    // Indicates that DST will not be affected should the pattern fail.
    ATOMIC = false,
    // Indicates that the pattern can cause backtracking of the input stream.
    BACKTRACKS = false,
    // Indicates that should the pattern fail, the data stream will be in the
    // same state as before entering it.
    FAILS_CLEANLY = false,
    // Indicates that result can be established without actually consuming data
    // from
    // the data stream
    PEEKABLE = false,
    // Indicates that the pattern could succeed without consuming any data.
    MAY_NOT_CONSUME = true,
    // Indicates that the pattern incrementaly calls push_back(), or invokes
    // insert(end,B,E)
    // on its target
    APPENDS_DST = false,
    // Indicates that the pattern will never append and fail (useful for some
    // optimizations)
    STABLE_APPENDS = false,
  };
};
template <typename T, typename RECUR_TAG, typename Enable = void>
struct pattern_traits;
template <typename RECUR_TAG, typename... CHILD_PATS>
struct any_pat_traits {
  enum {
    ATOMIC = false,
    BACKTRACKS = false,
    FAILS_CLEANLY = false,
    MAY_NOT_CONSUME = false,
    APPENDS_DST = false,
    PEEKABLE = false,
    STABLE_APPENDS = false
  };
};
template <typename RECUR_TAG, typename T, typename... REST_PATS>
struct any_pat_traits<RECUR_TAG, T, REST_PATS...> {
  enum {
    ATOMIC = pattern_traits<T, RECUR_TAG>::ATOMIC ||
             any_pat_traits<RECUR_TAG, REST_PATS...>::ATOMIC,
    BACKTRACKS = pattern_traits<T, RECUR_TAG>::BACKTRACKS ||
                 any_pat_traits<RECUR_TAG, REST_PATS...>::BACKTRACKS,
    FAILS_CLEANLY = pattern_traits<T, RECUR_TAG>::FAILS_CLEANLY ||
                    any_pat_traits<RECUR_TAG, REST_PATS...>::FAILS_CLEANLY,
    MAY_NOT_CONSUME = pattern_traits<T, RECUR_TAG>::MAY_NOT_CONSUME ||
                      any_pat_traits<RECUR_TAG, REST_PATS...>::MAY_NOT_CONSUME,
    PEEKABLE = pattern_traits<T, RECUR_TAG>::PEEKABLE ||
               any_pat_traits<RECUR_TAG, REST_PATS...>::PEEKABLE,
    APPENDS_DST = pattern_traits<T, RECUR_TAG>::APPENDS_DST ||
                  any_pat_traits<RECUR_TAG, REST_PATS...>::APPENDS_DST,
    STABLE_APPENDS = pattern_traits<T, RECUR_TAG>::STABLE_APPENDS ||
                     any_pat_traits<RECUR_TAG, REST_PATS...>::STABLE_APPENDS,
  };
};
template <typename RECUR_TAG, typename... CHILD_PATS>
struct all_pat_traits {
  enum {
    ATOMIC = true,
    BACKTRACKS = true,
    FAILS_CLEANLY = true,
    MAY_NOT_CONSUME = true,
    PEEKABLE = true,
    APPENDS_DST = true,
    STABLE_APPENDS = true,
  };
};
template <typename RECUR_TAG, typename T, typename... REST_PATS>
struct all_pat_traits<RECUR_TAG, T, REST_PATS...> {
  enum {
    ATOMIC = pattern_traits<T, RECUR_TAG>::ATOMIC &&
             all_pat_traits<RECUR_TAG, REST_PATS...>::ATOMIC,
    BACKTRACKS = pattern_traits<T, RECUR_TAG>::BACKTRACKS &&
                 all_pat_traits<RECUR_TAG, REST_PATS...>::BACKTRACKS,
    FAILS_CLEANLY = pattern_traits<T, RECUR_TAG>::FAILS_CLEANLY &&
                    all_pat_traits<RECUR_TAG, REST_PATS...>::FAILS_CLEANLY,
    MAY_NOT_CONSUME = pattern_traits<T, RECUR_TAG>::MAY_NOT_CONSUME &&
                      all_pat_traits<RECUR_TAG, REST_PATS...>::MAY_NOT_CONSUME,
    PEEKABLE = pattern_traits<T, RECUR_TAG>::PEEKABLE &&
               all_pat_traits<RECUR_TAG, REST_PATS...>::PEEKABLE,
    APPENDS_DST = pattern_traits<T, RECUR_TAG>::APPENDS_DST &&
                  all_pat_traits<RECUR_TAG, REST_PATS...>::APPENDS_DST,
    STABLE_APPENDS = pattern_traits<T, RECUR_TAG>::STABLE_APPENDS &&
                     all_pat_traits<RECUR_TAG, REST_PATS...>::STABLE_APPENDS,
  };
};
struct DefaultState {
  template <typename T, typename U>
  DefaultState(T&, U&) {}
};
class PatternBase {};
template <typename T>
class Pattern : public PatternBase {
 public:
  using pattern_t = T;
  /*
    template <typename ACT_T>
    auto operator[](ACT_T act) const {
      return Action<T, ACT_T>(*static_cast<T const*>(this), act);
    }
  */
  /*
  template <typename ATTR_T>
  auto as() const {
    return AttrCast<ATTR_T, T>(*static_cast<T const*>(this));
  }
  */
};
template <typename DST_T, typename CTX_T>
struct StateTraits {
  using dst = DST_T;
  using ctx = CTX_T;
  template <typename NEW_DST>
  using cast = StateTraits<NEW_DST, CTX_T>;
};
template <typename PAT_T, typename STATE_TRAITS>
struct choose_state {
  using type = typename decay_t<PAT_T>::template State<STATE_TRAITS>;
};
template <typename PAT_T, typename STATE_TRAITS>
using state_t = typename choose_state<PAT_T, STATE_TRAITS>::type;
template <typename PAT_T, typename CTX_T>
struct pat_attr_t {};
template <typename PAT_T, typename CTX_T>
using attr_t = typename pat_attr_t<decay_t<PAT_T>, CTX_T>::attr_type;
template <typename LHS_T, typename RHS_T>
void expect_same() {
  static_assert(is_same<LHS_T, RHS_T>::value, "attr_t expectation failure");
}
template <typename T>
struct expr_traits<
    T, enable_if_t<std::is_base_of<PatternBase, decay_t<T>>::value>> {
  enum { is_pattern = true, converts_to_pattern = false };
  static const T& make_pattern(const T& v) { return v; }
};
template <typename T>
typename std::remove_reference<T>::type& force_lvalue(T&& t) {
  return static_cast<typename std::remove_reference<T>::type&>(t);
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename REAL_PAT_T, typename CTX_T, typename DST_T>
struct ParserInterface {
  ParserInterface(REAL_PAT_T const& pat, CTX_T& ctx, DST_T& dst)
      : pat_(pat), ctx_(ctx), dst_(dst), parser_(ctx, dst, pat) {}
  result consume() { return parser_.consume(ctx_, dst_, pat_); }
 private:
  REAL_PAT_T const& pat_;
  CTX_T& ctx_;
  DST_T& dst_;
  Parser<CTX_T, DST_T, REAL_PAT_T> parser_;
};
template <typename PAT_T, typename DATASOURCE_T, typename DST_T>
auto make_parser(PAT_T const& p, DATASOURCE_T& d, DST_T& s) {
  auto real_pat = make_pattern(p);
  return ParserInterface<decltype(real_pat), DATASOURCE_T, DST_T>(p, d, s);
}
template <typename PAT_T, typename DATASOURCE_T>
auto make_parser(PAT_T const& p, DATASOURCE_T& d) {
  return make_parser(p, d, nil);
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename PAT_T, typename DATA_RANGE_T, typename DST_T>
result parse(const PAT_T& pat, const DATA_RANGE_T& data, DST_T& dst) {
  using iterator_t = decltype(std::begin(data));
  auto real_pat = make_pattern(pat);
  SingleForwardContext<iterator_t> ctx(std::begin(data), std::end(data));
  auto parser = make_parser_(ctx, dst, real_pat);
  return parser.consume(ctx, dst, real_pat);
}
template <typename PAT_T, typename ITE_T, typename DST_T>
result parse(const PAT_T& pat, ITE_T b, ITE_T e, DST_T& dst) {
  auto real_pat = make_pattern(pat);
  SingleForwardContext<ITE_T> ctx(b, e);
  auto parser = make_parser_(ctx, dst, real_pat);
  return parser.consume(ctx, dst, real_pat);
}
template <typename PAT_T, typename DATA_RANGE_T>
result parse(const PAT_T& pat, const DATA_RANGE_T& data) {
  Nil fake_dest;
  return parse(pat, data, fake_dest);
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CHILD_PAT_T, typename ATTR_T = Nil>
class Recur : public Pattern<Recur<CHILD_PAT_T, ATTR_T>> {
  std::shared_ptr<std::unique_ptr<CHILD_PAT_T>> pat_;
 public:
  using operand_pat_t = CHILD_PAT_T;
  using attr_t = ATTR_T;
  Recur() : pat_(std::make_shared<std::unique_ptr<CHILD_PAT_T>>()) {}
  Recur(Recur const& rhs) = default;
  Recur(Recur&& rhs) = default;
  Recur& operator=(CHILD_PAT_T rhs) {
    *pat_ = std::make_unique<CHILD_PAT_T>(std::move(rhs));
    return *this;
  }
  CHILD_PAT_T const& operand() const { return **pat_; }
 private:
  template <typename T, typename U>
  friend class WeakRecur;
};
template <typename CHILD_PAT_T, typename ATTR_T = Nil>
class WeakRecur : public Pattern<WeakRecur<CHILD_PAT_T, ATTR_T>> {
  std::unique_ptr<CHILD_PAT_T>* pat_;
 public:
  using operand_pat_t = CHILD_PAT_T;
  WeakRecur(Recur<CHILD_PAT_T, ATTR_T> const& r) : pat_(r.pat_.get()) {}
  WeakRecur(WeakRecur const& rhs) = default;
  WeakRecur(WeakRecur&& rhs) = default;
  CHILD_PAT_T const& operand() const { return **pat_; }
};
template <typename CHILD_PAT_T, typename ATTR_T>
struct RecurWeakener {
  template <typename T>
  auto operator()(T const& rhs) const {
    return transform(rhs, *this);
  }
  auto operator()(Recur<CHILD_PAT_T, ATTR_T> const& tgt_recur) const {
    return WeakRecur<CHILD_PAT_T, ATTR_T>(tgt_recur);
  }
};
template <typename RECUR_T, typename PAT_T>
auto weaken_recur(PAT_T const& pat) {
  using CHILD_PAT_T = typename RECUR_T::operand_pat_t;
  using ATTR_T = typename RECUR_T::attr_t;
  RecurWeakener<CHILD_PAT_T, ATTR_T> transformation;
  return transform(pat, transformation);
}
template <typename PAT_T, typename ATTR_T, typename CTX_T>
struct pat_attr_t<Recur<PAT_T, ATTR_T>, CTX_T> {
  using attr_type = ATTR_T;
};
template <typename PAT_T, typename ATTR_T, typename CTX_T>
struct pat_attr_t<WeakRecur<PAT_T, ATTR_T>, CTX_T> {
  using attr_type = ATTR_T;
};
template <typename PAT_T, typename ATTR_T>
struct pattern_traits<Recur<PAT_T, ATTR_T>, void>
    : public default_pattern_traits {
  using sub_traits =
      pattern_traits<typename PAT_T::pattern_t, std::tuple<PAT_T>>;
  enum {
    ATOMIC = sub_traits::ATOMIC,
    BACKTRACKS = sub_traits::BACKTRACKS,
    FAILS_CLEANLY = sub_traits::FAILS_CLEANLY,
    MAY_NOT_CONSUME = sub_traits::MAY_NOT_CONSUME,
    PEEKABLE = sub_traits::PEEKABLE,
    APPENDS_DST = sub_traits::APPENDS_DST,
    STABLE_APPENDS = sub_traits::STABLE_APPENDS
  };
};
template <typename PAT_T, typename ATTR_T>
struct pattern_traits<WeakRecur<PAT_T, ATTR_T>, void>
    : public default_pattern_traits {
  using sub_traits =
      pattern_traits<typename PAT_T::pattern_t, std::tuple<PAT_T>>;
  enum {
    ATOMIC = sub_traits::ATOMIC,
    BACKTRACKS = sub_traits::BACKTRACKS,
    FAILS_CLEANLY = sub_traits::FAILS_CLEANLY,
    MAY_NOT_CONSUME = sub_traits::MAY_NOT_CONSUME,
    PEEKABLE = sub_traits::PEEKABLE,
    APPENDS_DST = sub_traits::APPENDS_DST,
    STABLE_APPENDS = sub_traits::STABLE_APPENDS
  };
};
template <typename PAT_T, typename ATTR_T, typename... TYPES>
struct pattern_traits<
    Recur<PAT_T, ATTR_T>, std::tuple<TYPES...>,
    enable_if_t<
        is_same<typename std::tuple_element<0, std::tuple<TYPES...>>::type,
                PAT_T>::value>> : public default_pattern_traits {
  // These are provisional flags, only used for the first pass
  enum {
    ATOMIC = true,
    BACKTRACKS = false,
    FAILS_CLEANLY = true,
    MAY_NOT_CONSUME = false,
    PEEKABLE = true,
    APPENDS_DST = false,
    STABLE_APPENDS = false
  };
};
template <typename PAT_T, typename ATTR_T, typename... TYPES>
struct pattern_traits<
    WeakRecur<PAT_T, ATTR_T>, std::tuple<TYPES...>,
    enable_if_t<
        is_same<typename std::tuple_element<0, std::tuple<TYPES...>>::type,
                PAT_T>::value>> : public default_pattern_traits {
  // These are provisional flags, only used for the first pass
  enum {
    ATOMIC = true,
    BACKTRACKS = false,
    FAILS_CLEANLY = true,
    MAY_NOT_CONSUME = false,
    PEEKABLE = true,
    APPENDS_DST = false,
    STABLE_APPENDS = false
  };
};
template <typename PAT_T, typename ATTR_T, typename... TYPES>
struct pattern_traits<Recur<PAT_T, ATTR_T>, std::tuple<TYPES...>,
                      enable_if_t<!is_one_of<PAT_T, TYPES...>::value>>
    : public default_pattern_traits {
  using sub_traits =
      pattern_traits<typename PAT_T::pattern_t, std::tuple<PAT_T, TYPES...>>;
  enum {
    ATOMIC = sub_traits::ATOMIC,
    BACKTRACKS = sub_traits::BACKTRACKS,
    FAILS_CLEANLY = sub_traits::FAILS_CLEANLY,
    MAY_NOT_CONSUME = sub_traits::MAY_NOT_CONSUME,
    PEEKABLE = sub_traits::PEEKABLE,
    APPENDS_DST = sub_traits::APPENDS_DST,
    STABLE_APPENDS = sub_traits::STABLE_APPENDS
  };
};
template <typename PAT_T, typename ATTR_T, typename... TYPES>
struct pattern_traits<WeakRecur<PAT_T, ATTR_T>, std::tuple<TYPES...>,
                      enable_if_t<!is_one_of<PAT_T, TYPES...>::value>>
    : public default_pattern_traits {
  using sub_traits =
      pattern_traits<typename PAT_T::pattern_t, std::tuple<PAT_T, TYPES...>>;
  enum {
    ATOMIC = sub_traits::ATOMIC,
    BACKTRACKS = sub_traits::BACKTRACKS,
    FAILS_CLEANLY = sub_traits::FAILS_CLEANLY,
    MAY_NOT_CONSUME = sub_traits::MAY_NOT_CONSUME,
    PEEKABLE = sub_traits::PEEKABLE,
    APPENDS_DST = sub_traits::APPENDS_DST,
    STABLE_APPENDS = sub_traits::STABLE_APPENDS
  };
};
template <typename PAT_T, typename... TYPES>
struct is_circular_recur_dep {
  enum {
    value = is_one_of<PAT_T, TYPES...>::value &&
            !std::is_same<
                typename std::tuple_element<0, std::tuple<TYPES...>>::type,
                PAT_T>::value
  };
};
template <typename PAT_T, typename ATTR_T, typename... TYPES>
struct pattern_traits<
    Recur<PAT_T, ATTR_T>, std::tuple<TYPES...>,
    enable_if_t<is_circular_recur_dep<PAT_T, TYPES...>::value>>
    : public default_pattern_traits {
  enum {
    ATOMIC = false,
    BACKTRACKS = true,
    FAILS_CLEANLY = false,
    MAY_NOT_CONSUME =
        false,  // a bit of a leap of fate here... we may have to enforce that
    PEEKABLE = false,
    APPENDS_DST = false,
    STABLE_APPENDS = false
  };
};
template <typename PAT_T, typename ATTR_T, typename... TYPES>
struct pattern_traits<
    WeakRecur<PAT_T, ATTR_T>, std::tuple<TYPES...>,
    enable_if_t<is_circular_recur_dep<PAT_T, TYPES...>::value>>
    : public default_pattern_traits {
  enum {
    ATOMIC = false,
    BACKTRACKS = true,
    FAILS_CLEANLY = false,
    MAY_NOT_CONSUME =
        false,  // a bit of a leap of fate here... we may have to enforce that
    PEEKABLE = false,
    APPENDS_DST = false,
    STABLE_APPENDS = false
  };
};
}  // namespace ABULAFIA_NAMESPACE
#define ABU_Recur_define(var, RECUR_TAG, pattern)                             \
  using abu_##RECUR_TAG##_recur_t = decltype(var);                            \
  using abu_##RECUR_TAG##_weakened_t = decltype(                              \
      ABULAFIA_NAMESPACE ::weaken_recur<abu_##RECUR_TAG##_recur_t>(pattern)); \
  struct RECUR_TAG : public abu_##RECUR_TAG##_weakened_t {                    \
    using pattern_t = abu_##RECUR_TAG##_weakened_t;                           \
    RECUR_TAG(decltype(pattern) const & p)                                    \
        : pattern_t(                                                          \
              ABULAFIA_NAMESPACE ::weaken_recur<abu_##RECUR_TAG##_recur_t>(   \
                  p)) {}                                                      \
  };                                                                          \
  var = RECUR_TAG(pattern);

namespace ABULAFIA_NAMESPACE {
template <typename CHILD_PAT_T, typename SKIP_T>
class WithSkipper : public Pattern<WithSkipper<CHILD_PAT_T, SKIP_T>> {
  CHILD_PAT_T child_pat_;
  SKIP_T skip_pat_;
 public:
  CHILD_PAT_T const& getChild() const { return child_pat_; }
  SKIP_T const& getSkip() const { return skip_pat_; }
  WithSkipper(CHILD_PAT_T const& c, SKIP_T const& s)
      : child_pat_(c), skip_pat_(s) {}
};
template <typename CHILD_PAT_T, typename SKIP_T, typename CTX_T>
struct pat_attr_t<WithSkipper<CHILD_PAT_T, SKIP_T>, CTX_T> {
  using attr_type = attr_t<CHILD_PAT_T, CTX_T>;
};
template <typename CHILD_PAT_T, typename SKIP_T, typename RECUR_TAG>
struct pattern_traits<WithSkipper<CHILD_PAT_T, SKIP_T>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    ATOMIC = pattern_traits<CHILD_PAT_T, RECUR_TAG>::ATOMIC,
    BACKTRACKS = true,  // Maybe not...
    FAILS_CLEANLY = pattern_traits<CHILD_PAT_T, RECUR_TAG>::FAILS_CLEANLY,
    PEEKABLE = false,
    MAY_NOT_CONSUME = pattern_traits<CHILD_PAT_T, RECUR_TAG>::MAY_NOT_CONSUME,
  };
};
template <typename PAT_T, typename SKIP_T>
auto apply_skipper(PAT_T&& pat, SKIP_T&& skip) {
  return WithSkipper<std::decay_t<PAT_T>, std::decay_t<SKIP_T>>(
      std::forward<PAT_T>(pat), std::forward<SKIP_T>(skip));
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename OP_T, typename NEG_T>
class Except : public Pattern<Except<OP_T, NEG_T>> {
  OP_T op_;
  NEG_T neg_;
 public:
  using op_t = OP_T;
  using neg_t = NEG_T;
  Except(op_t op_p, neg_t neg_p)
      : op_(std::move(op_p)), neg_(std::move(neg_p)) {}
  op_t const& op() const { return op_; }
  neg_t const& neg() const { return neg_; }
};
template <typename LHS_T, typename RHS_T, typename RECUR_TAG>
struct pattern_traits<Except<LHS_T, RHS_T>, RECUR_TAG> {
  enum {
    ATOMIC = pattern_traits<LHS_T, RECUR_TAG>::ATOMIC,
    BACKTRACKS = pattern_traits<LHS_T, RECUR_TAG>::BACKTRACKS ||
                 pattern_traits<RHS_T, RECUR_TAG>::BACKTRACKS ||
                 !pattern_traits<RHS_T, RECUR_TAG>::FAILS_CLEANLY,
    FAILS_CLEANLY = pattern_traits<LHS_T, RECUR_TAG>::FAILS_CLEANLY,
    PEEKABLE = pattern_traits<LHS_T, RECUR_TAG>::PEEKABLE &&
               pattern_traits<RHS_T, RECUR_TAG>::PEEKABLE,
    MAY_NOT_CONSUME = pattern_traits<LHS_T, RECUR_TAG>::MAY_NOT_CONSUME,
    APPENDS_DST = pattern_traits<LHS_T, RECUR_TAG>::APPENDS_DST,
    STABLE_APPENDS = pattern_traits<LHS_T, RECUR_TAG>::STABLE_APPENDS,
  };
};
template <typename LHS_T, typename RHS_T, typename CTX_T>
struct pat_attr_t<Except<LHS_T, RHS_T>, CTX_T> {
  using attr_type = abu::attr_t<LHS_T, CTX_T>;
};
template <typename LHS_T, typename RHS_T, typename CB_T>
auto transform(Except<LHS_T, RHS_T> const& tgt, CB_T const& cb) {
  auto new_op = cb(tgt.op());
  auto new_neg = cb(tgt.neg());
  using new_op_t = decltype(new_op);
  using new_sep_t = decltype(new_neg);
  return Except<new_op_t, new_sep_t>(std::move(new_op), std::move(new_neg));
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
namespace details {
template <typename T, typename Enable = void>
void reset_if_container(T& dst);
template <typename CONT_T, typename... ARGS>
void append_to_container(CONT_T& container, ARGS&&... args) {
  container.emplace_back(forward<ARGS>(args)...);
}
template <class CharT, class Traits = std::char_traits<CharT>,
          class Allocator = std::allocator<CharT>, typename... ARGS>
void append_to_container(std::basic_string<CharT, Traits, Allocator>& container,
                         ARGS&&... args) {
  container.push_back(forward<ARGS>(args)...);
}
}  // namespace details
template <typename T>
struct DefaultCollectionAssignWrapper {
  using value_type = typename T::value_type;
  using iterator = typename T::iterator;
  DefaultCollectionAssignWrapper(T& dst) : dst_(dst) {}
  // This is the key point:
  // If we try to assign a value to the wrapper, it'll result in a emplace_back
  // on its target.
  template <typename U>
  DefaultCollectionAssignWrapper& operator=(U&& rhs) {
    details::append_to_container(dst_, forward<U>(rhs));
    return *this;
  }
  // The wrapper can also pose as the target itself.
  template <typename U>
  void emplace_back(U&& rhs) {
    details::append_to_container(dst_, forward<U>(rhs));
  }
  template <typename ite>
  void insert(iterator pos, ite b, ite e) {
    dst_.insert(pos, b, e);
  }
  iterator end() { return dst_.end(); }
  // This intentionally does nothing
  void clear() {}
  // Should this ever happen?
  //  void push_back(Nil const&) {}
  T& dst_;
};
template <typename T>
struct ChooseCollectionWrapper {
  using type = DefaultCollectionAssignWrapper<T>;
};
template <typename T>
struct ChooseCollectionWrapper<DefaultCollectionAssignWrapper<T>> {
  using type = DefaultCollectionAssignWrapper<T>;
};
template <>
struct ChooseCollectionWrapper<Nil> {
  using type = Nil;
};
template <typename T>
using CollectionAssignWrapper = typename ChooseCollectionWrapper<T>::type;
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
namespace buf_ {
enum class CollectionAdapter {
  WRAP,
  PUSH_VAL,
  APPEND_COLLECTION,
  BAD_MATCH,
};
template <typename DST_T, typename PAT_T>
constexpr CollectionAdapter choose_collection_adapter() {
  if (is_same<Nil, DST_T>::value) {
    // If we are writing to nil, use the lightest adapter, which is wrap.
    return CollectionAdapter::WRAP;
  } else if (pattern_traits<PAT_T, void>::APPENDS_DST) {
    // If the child pattern is a loop pattern.
    if (is_collection<typename DST_T::value_type>()) {
      // And the destination is a collection of collection
      // Build a collection and append it on success
      return CollectionAdapter::PUSH_VAL;
    } else {
      // The destination is a collection of values
      if (pattern_traits<PAT_T, void>::STABLE_APPENDS) {
        // If the child pattern is stable, just push as we go
        return CollectionAdapter::WRAP;
      } else {
        // If the child pattern is not stable, build the collection
        // first, and concatenate it to dst on sucess.
        return CollectionAdapter::APPEND_COLLECTION;
      }
    }
  } else {
    // The child pattern is a value pattern
    if (pattern_traits<PAT_T, void>::ATOMIC) {
      // If it's atomic, then just write to the destination
      return CollectionAdapter::WRAP;
    } else {
      // If it's not atomic, we need to store the value in a buffer, and push it
      // on success.
      return CollectionAdapter::PUSH_VAL;
    }
  }
  return CollectionAdapter::BAD_MATCH;
}
template <typename DST_T, typename PAT_T>
struct ChosenCollectionAdapter
    : public std::integral_constant<CollectionAdapter,
                                    choose_collection_adapter<DST_T, PAT_T>()> {
};
template <typename CTX_T, typename DST_T, typename PAT_T,
          typename Enable = void>
struct CollectionParserAdapter;
template <typename CTX_T, typename DST_T, typename PAT_T>
struct CollectionParserAdapter<
    CTX_T, DST_T, PAT_T,
    enable_if_t<ChosenCollectionAdapter<DST_T, PAT_T>::value ==
                CollectionAdapter::WRAP>> {
  using assign_wrapper_t = CollectionAssignWrapper<DST_T>;
  using child_parser_t = Parser<CTX_T, assign_wrapper_t, PAT_T>;
  CollectionParserAdapter(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : child_parser_(ctx, force_lvalue(assign_wrapper_t(dst)), pat) {}
  auto consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    auto wrapped = assign_wrapper_t(dst);
    return child_parser_.consume(ctx, wrapped, pat);
  }
  child_parser_t child_parser_;
};
template <typename CTX_T, typename DST_T, typename PAT_T>
struct CollectionParserAdapter<
    CTX_T, DST_T, PAT_T,
    enable_if_t<ChosenCollectionAdapter<DST_T, PAT_T>::value ==
                CollectionAdapter::PUSH_VAL>> {
  using buffer_t = typename DST_T::value_type;
  using child_parser_t = Parser<CTX_T, buffer_t, PAT_T>;
  CollectionParserAdapter(CTX_T& ctx, DST_T&, PAT_T const& pat)
      : child_parser_(ctx, buffer_, pat) {}
  auto consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    auto result = child_parser_.consume(ctx, buffer_, pat);
    if (result == result::SUCCESS) {
      dst.emplace_back(buffer_);
    }
    return result;
  }
  buffer_t buffer_;
  child_parser_t child_parser_;
};
template <typename CTX_T, typename DST_T, typename PAT_T>
struct CollectionParserAdapter<
    CTX_T, DST_T, PAT_T,
    enable_if_t<ChosenCollectionAdapter<DST_T, PAT_T>::value ==
                CollectionAdapter::APPEND_COLLECTION>> {
  using buffer_t = std::vector<typename DST_T::value_type>;
  using child_parser_t = Parser<CTX_T, buffer_t, PAT_T>;
  CollectionParserAdapter(CTX_T& ctx, DST_T&, PAT_T const& pat)
      : child_parser_(ctx, buffer_, pat) {}
  auto consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    auto result = child_parser_.consume(ctx, buffer_, pat);
    if (result == result::SUCCESS) {
      dst.insert(dst.end(), buffer_.begin(), buffer_.end());
    }
    return result;
  }
  buffer_t buffer_;
  child_parser_t child_parser_;
};
}  // namespace buf_
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename PAT_T, std::size_t MIN_REP, std::size_t MAX_REP>
class Repeat : public Pattern<Repeat<PAT_T, MIN_REP, MAX_REP>> {
  PAT_T operand_;
 public:
  Repeat(const PAT_T& op) : operand_(op) {}
  Repeat(PAT_T&& op) : operand_(std::move(op)) {}
  PAT_T const& operand() const { return operand_; }
};
template <std::size_t MIN_REP = 0, std::size_t MAX_REP = 0, typename PAT_T>
inline auto repeat(PAT_T&& pat) {
  return Repeat<pattern_t<PAT_T>, MIN_REP, MAX_REP>(
      make_pattern(forward<PAT_T>(pat)));
}
template <std::size_t MIN_REP = 0, std::size_t MAX_REP = 0, typename PAT_T,
          typename CB_T>
auto transform(Repeat<PAT_T, MIN_REP, MAX_REP> const& tgt, CB_T const& cb) {
  return repeat(cb(tgt.operand()));
}
namespace repeat_ {
template <typename PAT_T, typename CTX_T, typename Enable = void>
struct extract_value_type;
template <typename PAT_T, typename CTX_T>
struct extract_value_type<
    PAT_T, CTX_T, enable_if_t<!pattern_traits<PAT_T, void>::APPENDS_DST>> {
  using type = attr_t<PAT_T, CTX_T>;
};
template <typename PAT_T, typename CTX_T>
struct extract_value_type<
    PAT_T, CTX_T, enable_if_t<pattern_traits<PAT_T, void>::APPENDS_DST>> {
  using attr_t_type = attr_t<PAT_T, CTX_T>;
  using type = typename attr_t_type::value_type;
};
}  // namespace repeat_
template <typename PAT_T, std::size_t MIN_REP, std::size_t MAX_REP,
          typename RECUR_TAG>
struct pattern_traits<Repeat<PAT_T, MIN_REP, MAX_REP>, RECUR_TAG>
    : public default_pattern_traits {
  static_assert(pattern_traits<PAT_T, RECUR_TAG>::MAY_NOT_CONSUME == false ||
                    MAX_REP != 0,
                "invalid repeat pattern, infinite loop");
  enum {
    ATOMIC = false,
    BACKTRACKS = pattern_traits<PAT_T, RECUR_TAG>::BACKTRACKS ||
                 !pattern_traits<PAT_T, RECUR_TAG>::FAILS_CLEANLY,
    FAILS_CLEANLY = true,
    MAY_NOT_CONSUME =
        MIN_REP == 0 || pattern_traits<PAT_T, RECUR_TAG>::MAY_NOT_CONSUME,
    PEEKABLE = MIN_REP == 0 ||
               (MIN_REP == 1 && pattern_traits<PAT_T, RECUR_TAG>::PEEKABLE),
    APPENDS_DST = true,
    STABLE_APPENDS = MIN_REP <= 1
  };
};
template <typename PAT_T, std::size_t MIN_REP, std::size_t MAX_REP,
          typename CTX_T>
struct pat_attr_t<Repeat<PAT_T, MIN_REP, MAX_REP>, CTX_T> {
  using value_type = typename repeat_::extract_value_type<PAT_T, CTX_T>::type;
  using attr_type =
      typename std::conditional<is_same<Nil, value_type>::value, Nil,
                                std::vector<value_type>>::type;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename VAL_PAT_T, typename SEP_PAT_T>
class List : public Pattern<List<VAL_PAT_T, SEP_PAT_T>> {
 public:
  using val_pat_t = VAL_PAT_T;
  using sep_pat_t = SEP_PAT_T;
  List(val_pat_t val_pat, sep_pat_t sep)
      : val_(std::move(val_pat)), sep_(std::move(sep)) {}
  val_pat_t const& op() const { return val_; }
  sep_pat_t const& sep() const { return sep_; }
 private:
  VAL_PAT_T val_;
  SEP_PAT_T sep_;
};
template <typename LHS_T, typename RHS_T, typename RECUR_TAG>
struct pattern_traits<List<LHS_T, RHS_T>, RECUR_TAG> {
  enum {
    BACKTRACKS = pattern_traits<LHS_T, RECUR_TAG>::BACKTRACKS ||
                 pattern_traits<RHS_T, RECUR_TAG>::BACKTRACKS ||
                 !pattern_traits<RHS_T, RECUR_TAG>::FAILS_CLEANLY ||
                 !pattern_traits<LHS_T, RECUR_TAG>::FAILS_CLEANLY,
    FAILS_CLEANLY = false,  // TODO: not 100% correct.
    MAY_NOT_CONSUME = pattern_traits<LHS_T, RECUR_TAG>::MAY_NOT_CONSUME,
    ATOMIC = false,
    APPENDS_DST = true,
    STABLE_APPENDS = true,
  };
};
template <typename LHS_T, typename RHS_T, typename CTX_T>
struct pat_attr_t<List<LHS_T, RHS_T>, CTX_T> {
  using attr_type = typename std::conditional<
      std::is_same<Nil, abu::attr_t<LHS_T, CTX_T>>::value, Nil,
      std::vector<abu::attr_t<LHS_T, CTX_T>>>::type;
};
template <typename LHS_T, typename RHS_T, typename CB_T>
auto transform(List<LHS_T, RHS_T> const& tgt, CB_T const& cb) {
  auto new_op = cb(tgt.operand());
  auto new_sep = cb(tgt.separator());
  using new_op_t = decltype(new_op);
  using new_sep_t = decltype(new_sep);
  return List<new_op_t, new_sep_t>(std::move(new_op), std::move(new_sep));
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CHARSET_T>
class Char : public Pattern<Char<CHARSET_T>> {
  CHARSET_T char_set_;
 public:
  Char(CHARSET_T&& chars) : char_set_(std::move(chars)) {}
  Char(CHARSET_T const& chars) : char_set_(chars) {}
  CHARSET_T const& char_set() const { return char_set_; }
};
template <typename CHARSET_T, typename RECUR_TAG>
struct pattern_traits<Char<CHARSET_T>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    ATOMIC = true,
    BACKTRACKS = false,
    FAILS_CLEANLY = true,
    PEEKABLE = true,
    MAY_NOT_CONSUME = false,
  };
};
template <typename CHARSET_T, typename CTX_T>
struct pat_attr_t<Char<CHARSET_T>, CTX_T> {
  using attr_type = typename CHARSET_T::char_t;
};
template <typename T = char>
auto char_() {
  return Char<char_set::Any<T>>(char_set::Any<T>());
}
template <typename T>
enable_if_t<!char_set::is_char_set<T>::value, Char<char_set::Single<T>>> char_(
    T const& c) {
  return Char<char_set::Single<T>>(char_set::Single<T>(c));
}
template <typename T>
enable_if_t<char_set::is_char_set<decay_t<T>>::value, Char<decay_t<T>>> char_(
    T&& chars) {
  return Char<decay_t<T>>(forward<T>(chars));
}
template <typename T>
auto char_(T begin, T end) {
  return Char<char_set::Range<T>>(char_set::Range<T>(begin, end));
}
inline auto char_(const char * str) {
  return char_(char_set::set(str));
}
template <typename CHAR_SET_T>
struct expr_traits<CHAR_SET_T,
                   enable_if_t<char_set::is_char_set<CHAR_SET_T>::value>> {
  enum { is_pattern = false, converts_to_pattern = true };
  static Char<CHAR_SET_T> make_pattern(CHAR_SET_T const& v) { return char_(v); }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename ATTR_T, typename PAT_T>
class AttrCast : public Pattern<AttrCast<ATTR_T, PAT_T>> {
  PAT_T pat_;
 public:
  AttrCast(PAT_T const& pat) : pat_(pat) {}
  AttrCast(PAT_T&& pat) : pat_(std::move(pat)) {}
  PAT_T const& operand() const { return pat_; }
};
template <typename ATTR_T, typename PAT_T>
inline auto cast(PAT_T&& pat) {
  using real_pat_t = pattern_t<PAT_T>;
  return AttrCast<ATTR_T, real_pat_t>(make_pattern(forward<PAT_T>(pat)));
}
template <typename PAT_T>
inline auto ignore(PAT_T&& pat) {
  using real_pat_t = pattern_t<PAT_T>;
  return AttrCast<Nil, real_pat_t>(make_pattern(forward<PAT_T>(pat)));
}
template <typename ATTR_T, typename PAT_T, typename RECUR_TAG>
struct pattern_traits<AttrCast<ATTR_T, PAT_T>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    BACKTRACKS = pattern_traits<PAT_T, RECUR_TAG>::BACKTRACKS,
    FAILS_CLEANLY = pattern_traits<PAT_T, RECUR_TAG>::FAILS_CLEANLY,
    MAY_NOT_CONSUME = pattern_traits<PAT_T, RECUR_TAG>::MAY_NOT_CONSUME,
    PEEKABLE = pattern_traits<PAT_T, RECUR_TAG>::PEEKABLE,
    ATOMIC = pattern_traits<PAT_T, RECUR_TAG>::ATOMIC,
    APPENDS_DST = pattern_traits<PAT_T, RECUR_TAG>::APPENDS_DST,
    STABLE_APPENDS = pattern_traits<PAT_T, RECUR_TAG>::STABLE_APPENDS,
  };
};
template <typename ATTR_T, typename PAT_T, typename CTX_T>
struct pat_attr_t<AttrCast<ATTR_T, PAT_T>, CTX_T> {
  using attr_type = ATTR_T;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CHAR_T>
inline auto lit(CHAR_T chr) {
  auto res = char_(chr);
  return cast<Nil>(res);
}
template <typename T>
using CharLiteral = decltype(lit(std::declval<T>()));
template <>
struct expr_traits<char> {
  enum { is_pattern = false, converts_to_pattern = true };
  static auto make_pattern(char v) { return lit(v); }
};
template <>
struct expr_traits<char32_t> {
  enum { is_pattern = false, converts_to_pattern = true };
  static auto make_pattern(char32_t v) { return lit(v); }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CHAR_T, typename VAL_T>
class CharSymbol : public Pattern<CharSymbol<CHAR_T, VAL_T>> {
  std::map<CHAR_T, VAL_T> mapping_;
 public:
  CharSymbol(std::map<CHAR_T, VAL_T> const& vals) : mapping_(vals) {}
  std::map<CHAR_T, VAL_T> const& mapping() const { return mapping_; }
};
template <typename CHAR_T, typename VAL_T, typename RECUR_TAG>
struct pattern_traits<CharSymbol<CHAR_T, VAL_T>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    ATOMIC = true,
    BACKTRACKS = true,
    PEEKABLE = true,
    FAILS_CLEANLY = true,
    MAY_NOT_CONSUME = false,
  };
};
template <typename CHAR_T, typename VAL_T, typename CTX_T>
struct pat_attr_t<CharSymbol<CHAR_T, VAL_T>, CTX_T> {
  using attr_type = VAL_T;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
class Eoi : public Pattern<Eoi> {};
template <typename RECUR_TAG>
struct pattern_traits<Eoi, RECUR_TAG> : public default_pattern_traits {
  using attr_type = Nil;
  enum {
    BACKTRACKS = false,
    FAILS_CLEANLY = true,
    MAY_NOT_CONSUME = false,
    PEEKABLE = true,
    ATOMIC = true,
  };
};
template <typename CTX_T>
struct pat_attr_t<Eoi, CTX_T> {
  using attr_type = Nil;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
class Fail : public Pattern<Fail> {};
template <typename RECUR_TAG>
struct pattern_traits<Fail, RECUR_TAG> : public default_pattern_traits {
  enum {
    ATOMIC = true,
    BACKTRACKS = false,
    FAILS_CLEANLY = true,
    PEEKABLE = true,
    // a bit of a white lie. It never succeeds so it does not matter
    MAY_NOT_CONSUME = false,
  };
};
template <typename CTX_T>
struct pat_attr_t<Fail, CTX_T> {
  using attr_type = Nil;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
class Pass : public Pattern<Pass> {};
template <typename RECUR_TAG>
struct pattern_traits<Pass, RECUR_TAG> : public default_pattern_traits {
  enum {
    BACKTRACKS = false,
    FAILS_CLEANLY = true,
    PEEKABLE = true,
    MAY_NOT_CONSUME = true,
    ATOMIC = true,
  };
};
template <typename CTX_T>
struct pat_attr_t<Pass, CTX_T> {
  using attr_type = Nil;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CHAR_T>
class StringLiteral : public Pattern<StringLiteral<CHAR_T>> {
  std::basic_string<CHAR_T> str_;
 public:
  StringLiteral(std::basic_string<CHAR_T> const& str) : str_(str) {
    assert(str_.size() > 0);
  }
  StringLiteral(std::basic_string<CHAR_T>&& str) : str_(std::move(str)) {
    assert(str_.size() > 0);
  }
  auto begin() const { return str_.begin(); }
  auto end() const { return str_.end(); }
};
template <typename CHAR_T>
inline auto lit(CHAR_T const* str) {
  return StringLiteral<decay_t<CHAR_T>>(str);
}
template <>
struct expr_traits<char const*> {
  enum { is_pattern = false, converts_to_pattern = true };
  static StringLiteral<char> make_pattern(char const* v) { return lit(v); }
};
template <>
struct expr_traits<char32_t const*> {
  enum { is_pattern = false, converts_to_pattern = true };
  static StringLiteral<char32_t> make_pattern(char32_t const* v) {
    return lit(v);
  }
};
template <typename T, typename RECUR_TAG>
struct pattern_traits<StringLiteral<T>, RECUR_TAG>
    : public default_pattern_traits {
  using attr_type = Nil;
  enum {
    ATOMIC = true,
    BACKTRACKS = false,
    PEEKABLE = false,
    FAILS_CLEANLY = false,
    MAY_NOT_CONSUME = false,
  };
};
template <typename T, typename CTX_T>
struct pat_attr_t<StringLiteral<T>, CTX_T> {
  using attr_type = Nil;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CHAR_T, typename VAL_T>
class Symbol : public Pattern<Symbol<CHAR_T, VAL_T>> {
  // symbols->value map will be stored as a trie
  struct Node {
    std::map<CHAR_T, Node> child;
    std::optional<VAL_T> val;
  };
  Node root_;
 public:
  using node_t = Node;
  Symbol(std::map<std::basic_string<CHAR_T>, VAL_T> const& vals) {
    for (auto const& entry : vals) {
      node_t* next = &root_;
      for (auto const& chr : entry.first) {
        next = &next->child[chr];
      }
      if (next->val) {
        throw std::runtime_error("cannot have two symbols with the same value");
      }
      next->val = entry.second;
    }
  }
  Node const* root() const { return &root_; }
};
template <typename CHAR_T, typename VAL_T>
auto symbol(std::map<std::basic_string<CHAR_T>, VAL_T> const& vals) {
  return Symbol<CHAR_T, VAL_T>(vals);
}
template <typename CHAR_T, typename VAL_T, typename RECUR_TAG>
struct pattern_traits<Symbol<CHAR_T, VAL_T>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    ATOMIC = true,
    BACKTRACKS = true,
    PEEKABLE = false,
    FAILS_CLEANLY = false,
    MAY_NOT_CONSUME = false,
  };
};
template <typename CHAR_T, typename VAL_T, typename CTX_T>
struct pat_attr_t<Symbol<CHAR_T, VAL_T>, CTX_T> {
  using attr_type = VAL_T;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <int BASE, std::size_t DIGITS_MIN = 1, std::size_t DIGITS_MAX = 0>
class Int : public Pattern<Int<BASE, DIGITS_MIN, DIGITS_MAX>> {
  static_assert(DIGITS_MIN >= 1);
  static_assert(DIGITS_MAX >= DIGITS_MIN || DIGITS_MAX == 0);
};
template <int BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX,
          typename RECUR_TAG>
struct pattern_traits<Int<BASE, DIGITS_MIN, DIGITS_MAX>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    ATOMIC = false,
    BACKTRACKS = false,
    FAILS_CLEANLY = false,
    PEEKABLE = false,  // we cannot peek because "-" is valid
    MAY_NOT_CONSUME = false,
  };
};
template <int BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX,
          typename CTX_T>
struct pat_attr_t<Int<BASE, DIGITS_MIN, DIGITS_MAX>, CTX_T> {
  using attr_type = int;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <int BASE, std::size_t DIGITS_MIN = 1, std::size_t DIGITS_MAX = 0>
class Uint : public Pattern<Uint<BASE, DIGITS_MIN, DIGITS_MAX>> {
  static_assert(DIGITS_MIN >= 1, "Numeric parser must parse at least 1 digit");
  static_assert(DIGITS_MAX >= DIGITS_MIN || DIGITS_MAX == 0,
                "Max < Min? really?");
};
template <int BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX,
          typename RECUR_TAG>
struct pattern_traits<Uint<BASE, DIGITS_MIN, DIGITS_MAX>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    ATOMIC = false,
    BACKTRACKS = false,
    FAILS_CLEANLY = DIGITS_MAX == 1,
    PEEKABLE = DIGITS_MIN == 1,
    MAY_NOT_CONSUME = false,
  };
};
template <int BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX,
          typename CTX_T>
struct pat_attr_t<Uint<BASE, DIGITS_MIN, DIGITS_MAX>, CTX_T> {
  using attr_type = unsigned int;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
namespace detail {
template <typename T, template <typename...> typename PAT_T>
struct is_nary_pattern : public std::false_type {};
template <typename... T, template <typename...> typename PAT_T>
struct is_nary_pattern<PAT_T<T...>, PAT_T> : public std::true_type {};
template <template <typename...> typename PAT_T, typename LHS_T, typename RHS_T,
          typename Enable = void>
struct NaryPatternBuilder {
  using type = PAT_T<decay_t<LHS_T>, decay_t<RHS_T>>;
  template <typename LHS_P_T, typename RHS_P_T>
  static auto build(LHS_P_T&& lhs, RHS_P_T&& rhs) {
    return type(std::make_tuple(forward<LHS_P_T>(lhs), forward<RHS_P_T>(rhs)));
  }
};
template <template <typename...> typename PAT_T, typename RHS_T,
          typename... LHS_T>
struct NaryPatternBuilder<PAT_T, PAT_T<LHS_T...>, RHS_T,
                          enable_if_t<!is_nary_pattern<RHS_T, PAT_T>()>> {
  using type = PAT_T<LHS_T..., decay_t<RHS_T>>;
  template <typename LHS_P_T, typename RHS_P_T>
  static auto build(LHS_P_T&& lhs, RHS_P_T&& rhs) {
    return type(
        std::tuple_cat(lhs.childs(), std::make_tuple(forward<RHS_P_T>(rhs))));
  }
};
template <template <typename...> typename PAT_T, typename LHS_T,
          typename... RHS_T>
struct NaryPatternBuilder<PAT_T, LHS_T, PAT_T<RHS_T...>,
                          enable_if_t<!is_nary_pattern<LHS_T, PAT_T>()>> {
  using type = PAT_T<decay_t<LHS_T>, RHS_T...>;
  template <typename LHS_P_T, typename RHS_P_T>
  static auto build(LHS_P_T&& lhs, RHS_P_T&& rhs) {
    return type(
        std::tuple_cat(std::make_tuple(forward<LHS_P_T>(lhs)), rhs.childs()));
  }
};
template <template <typename...> typename PAT_T, typename... LHS_T,
          typename... RHS_T>
struct NaryPatternBuilder<PAT_T, PAT_T<LHS_T...>, PAT_T<RHS_T...>, void> {
  using type = PAT_T<LHS_T..., RHS_T...>;
  template <typename LHS_P_T, typename RHS_P_T>
  static auto build(LHS_P_T&& lhs, RHS_P_T&& rhs) {
    return type(std::tuple_cat(lhs.childs(), rhs.childs()));
  }
};
}  // namespace detail
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename>
struct is_variant : public std::false_type {};
template <typename... ALL_T>
struct is_variant<std::variant<ALL_T...>> : public std::true_type {};
template <typename LHS_T, typename RHS_T>
struct variant_cat {
  using type =
      typename std::conditional<std::is_same<LHS_T, RHS_T>::value, LHS_T,
                                std::variant<LHS_T, RHS_T>>::type;
};
template <typename LHS_T, typename... RHS_VAR_T>
struct variant_cat<LHS_T, std::variant<RHS_VAR_T...>> {
  using type =
      typename std::conditional<is_one_of<LHS_T, RHS_VAR_T...>::value,
                                std::variant<RHS_VAR_T...>,
                                std::variant<LHS_T, RHS_VAR_T...>>::type;
};
template <typename... LHS_VAR_T, typename RHS_T>
struct variant_cat<std::variant<LHS_VAR_T...>, RHS_T> {
  using type =
      typename std::conditional<is_one_of<RHS_T, LHS_VAR_T...>::value,
                                std::variant<LHS_VAR_T...>,
                                std::variant<LHS_VAR_T..., RHS_T>>::type;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <std::size_t MAX_V, std::size_t N>
struct val_visitor {
  template <typename VISIT_T>
  static decltype(auto) visit(std::size_t val, VISIT_T&& visitor) {
    if (N == val) {
      return visitor(std::integral_constant<std::size_t, N>());
    } else {
      return val_visitor<MAX_V, N + 1>::visit(val,
                                              std::forward<VISIT_T>(visitor));
    }
  }
};
template <std::size_t MAX_V>
struct val_visitor<MAX_V, MAX_V> {
  template <typename VISIT_T>
  static decltype(auto) visit(std::size_t, VISIT_T&& visitor) {
    return visitor(std::integral_constant<std::size_t, MAX_V>());
  }
};
template <std::size_t MAX_V, typename VISITOR_T>
decltype(auto) visit_val(std::size_t v, VISITOR_T&& visit) {
  return val_visitor<MAX_V - 1, 0>::visit(v, std::forward<VISITOR_T>(visit));
}  // namespace ABULAFIA_NAMESPACE
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
namespace alt_ {
template <typename CTX_T, typename T, typename... TYPES_T>
struct get_attr {
  using type = attr_t<T, CTX_T>;
};
template <typename CTX_T, typename T, typename U, typename... TYPES_T>
struct get_attr<CTX_T, T, U, TYPES_T...> {
  using type =
      typename variant_cat<attr_t<T, CTX_T>,
                           typename get_attr<CTX_T, U, TYPES_T...>::type>::type;
};
}  // namespace alt_
template <typename... CHILD_PATS_T>
class Alt : public Pattern<Alt<CHILD_PATS_T...>> {
 public:
  using child_tuple_t = std::tuple<CHILD_PATS_T...>;
  Alt(child_tuple_t const& childs) : childs_(childs) {}
  child_tuple_t const& childs() const { return childs_; }
 private:
  child_tuple_t childs_;
};
template <std::size_t Index, typename... CHILD_PATS_T>
auto const& getChild(Alt<CHILD_PATS_T...> const& pat) {
  return std::get<Index>(pat.childs());
}
template <typename... T>
auto alt(T&&... args) {
  using ret_type = Alt<T...>;
  return ret_type(std::make_tuple(forward<T>(args)...));
}
template <typename RECUR_TAG, typename... CHILD_PATS>
struct pattern_traits<Alt<CHILD_PATS...>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    ATOMIC = all_pat_traits<RECUR_TAG, CHILD_PATS...>::ATOMIC,
    // technically, the last one doesn't matter
    BACKTRACKS = any_pat_traits<RECUR_TAG, CHILD_PATS...>::BACKTRACKS ||
                 !all_pat_traits<RECUR_TAG, CHILD_PATS...>::FAILS_CLEANLY,
    // technically should be just the last one
    FAILS_CLEANLY = all_pat_traits<RECUR_TAG, CHILD_PATS...>::FAILS_CLEANLY,
    MAY_NOT_CONSUME = any_pat_traits<RECUR_TAG, CHILD_PATS...>::MAY_NOT_CONSUME,
    PEEKABLE = all_pat_traits<RECUR_TAG, CHILD_PATS...>::PEEKABLE,
    // Because of this, should any child be a repeat, and the dst be a sequence,
    // any non-repeat childs should be pushing back.
    APPENDS_DST = any_pat_traits<RECUR_TAG, CHILD_PATS...>::APPENDS_DST,
    // TODO: this is wrong:
    // char_('A','Z') | *char_('a', 'z') should be stable
    STABLE_APPENDS = all_pat_traits<RECUR_TAG, CHILD_PATS...>::STABLE_APPENDS,
  };
};
template <typename CTX_T, typename... CHILD_PATS>
struct pat_attr_t<Alt<CHILD_PATS...>, CTX_T> {
  using attr_type = typename alt_::get_attr<CTX_T, CHILD_PATS...>::type;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
namespace seq_ {
template <typename LHS_T, typename RHS_T, typename Enable = void>
struct l_append_t {
  using type = std::tuple<LHS_T, RHS_T>;
};
template <>
struct l_append_t<Nil, Nil, void> {
  using type = Nil;
};
template <typename LHS_T>
struct l_append_t<LHS_T, Nil, void> {
  using type = LHS_T;
};
template <typename RHS_T>
struct l_append_t<Nil, RHS_T, void> {
  using type = RHS_T;
};
template <typename LHS_T, typename... RHS_TPL_T>
struct l_append_t<LHS_T, std::tuple<RHS_TPL_T...>,
                  std::enable_if_t<!std::is_same<LHS_T, Nil>::value &&
                                   !is_tuple<LHS_T>::value>> {
  using type = std::tuple<LHS_T, RHS_TPL_T...>;
};
template <typename... RHS_TPL_T>
struct l_append_t<Nil, std::tuple<RHS_TPL_T...>, void> {
  using type = std::tuple<RHS_TPL_T...>;
};
template <typename... LHS_TPL_T, typename RHS_T>
struct l_append_t<std::tuple<LHS_TPL_T...>, RHS_T, void> {
  using type = std::tuple<LHS_TPL_T..., RHS_T>;
};
template <typename... LHS_TPL_T>
struct l_append_t<std::tuple<LHS_TPL_T...>, Nil, void> {
  using type = std::tuple<LHS_TPL_T...>;
};
template <typename CTX_T, typename T, typename... REST_T>
struct determine_attr_type {
  using type = attr_t<T, CTX_T>;
};
template <typename CTX_T, typename T, typename U, typename... REST_T>
struct determine_attr_type<CTX_T, T, U, REST_T...> {
  using lhs_t_ = attr_t<T, CTX_T>;
  using rhs_t_ = typename determine_attr_type<CTX_T, U, REST_T...>::type;
  using type = l_append_t<lhs_t_, rhs_t_>;
};
template <int PAT_ID, typename CTX_T, typename CHILDS_TUPLE_T,
          typename Enable = void>
struct choose_tuple_index;
template <typename CTX_T, typename CHILDS_TUPLE_T>
struct choose_tuple_index<-1, CTX_T, CHILDS_TUPLE_T> {
  enum { value = -1, next_val = 0 };
};
template <int PAT_ID, typename CTX_T, typename CHILDS_TUPLE_T>
struct choose_tuple_index<
    PAT_ID, CTX_T, CHILDS_TUPLE_T,
    enable_if_t<(
        PAT_ID != -1 &&
        std::is_same<Nil, attr_t<tuple_element_t<PAT_ID == -1 ? 0 : PAT_ID,
                                                 CHILDS_TUPLE_T>,
                                 CTX_T>>::value)>> {
  enum {
    value = -1,
    next_val = choose_tuple_index<PAT_ID - 1, CTX_T, CHILDS_TUPLE_T>::next_val
  };
};
template <int PAT_ID, typename CTX_T, typename CHILDS_TUPLE_T>
struct choose_tuple_index<
    PAT_ID, CTX_T, CHILDS_TUPLE_T,
    enable_if_t<(
        PAT_ID != -1 &&
        !std::is_same<Nil, attr_t<tuple_element_t<PAT_ID == -1 ? 0 : PAT_ID,
                                                  CHILDS_TUPLE_T>,
                                  CTX_T>>::value)>> {
  enum {
    value = choose_tuple_index<PAT_ID - 1, CTX_T, CHILDS_TUPLE_T>::next_val,
    next_val = value + 1
  };
};
enum DstAccessorCategory {
  USE_NIL = 0,
  PASSTHROUGH = 1,
  INDEXED = 2,
  COLLECTION = 3
};
template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
constexpr DstAccessorCategory ChooseAccessorCategory() {
  if (std::is_same<Nil, DST_T>::value ||
      std::is_same<
          Nil, attr_t<tuple_element_t<PAT_ID, CHILDS_TUPLE_T>, CTX_T>>::value) {
    return USE_NIL;
  } else if (is_tuple<DST_T>::value) {
    return INDEXED;
  } else if (is_collection<DST_T>::value) {
    return COLLECTION;
  } else {
    return PASSTHROUGH;
  }
}
template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
struct AccessorCategoryChooser {
  enum {
    use_nil = ChooseAccessorCategory<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>() ==
              USE_NIL,
    passthrough =
        ChooseAccessorCategory<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>() ==
        PASSTHROUGH,
    indexed = ChooseAccessorCategory<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>() ==
              INDEXED,
    collection =
        ChooseAccessorCategory<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>() ==
        COLLECTION,
  };
};
template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T, typename Enable = void>
struct choose_dst_accessor;
template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
struct choose_dst_accessor<
    PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T,
    enable_if_t<AccessorCategoryChooser<PAT_ID, CTX_T, DST_T,
                                        CHILDS_TUPLE_T>::use_nil>> {
  using type = Nil;
  static Nil& access(DST_T&) { return nil; }
};
template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
struct choose_dst_accessor<
    PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T,
    enable_if_t<AccessorCategoryChooser<PAT_ID, CTX_T, DST_T,
                                        CHILDS_TUPLE_T>::collection>> {
  using type = CollectionAssignWrapper<DST_T>;
  static type access(DST_T& dst) { return type(dst); }
};
template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
struct choose_dst_accessor<
    PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T,
    enable_if_t<AccessorCategoryChooser<PAT_ID, CTX_T, DST_T,
                                        CHILDS_TUPLE_T>::passthrough>> {
  using type = DST_T;
  static DST_T& access(DST_T& dst) { return dst; }
};
template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
struct choose_dst_accessor<
    PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T,
    enable_if_t<AccessorCategoryChooser<PAT_ID, CTX_T, DST_T,
                                        CHILDS_TUPLE_T>::indexed>> {
  enum { dst_index = choose_tuple_index<PAT_ID, CTX_T, CHILDS_TUPLE_T>::value };
  using type = tuple_element_t<dst_index, DST_T>;
  static auto& access(DST_T& dst) { return std::get<dst_index>(dst); }
};
template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
struct WrappedParser {
  using dst_t =
      typename choose_dst_accessor<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>::type;
  using type = Parser<
      CTX_T,
      typename choose_dst_accessor<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>::type,
      tuple_element_t<PAT_ID, CHILDS_TUPLE_T>>;
};
template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
using WrappedParser_t =
    typename WrappedParser<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>::type;
template <typename CTX_T, typename DST_T, typename CHILDS_TUPLE_T,
          typename INDEX_SEQ>
struct SeqSubParser;
template <typename CTX_T, typename DST_T, typename CHILDS_TUPLE_T,
          std::size_t... PAT_IDS>
struct SeqSubParser<CTX_T, DST_T, CHILDS_TUPLE_T,
                    std::index_sequence<PAT_IDS...>> {
  //  using type = std::variant<WrappedParser_t<I, CTX_T, DST_T,
  //  CHILDS_TUPLE_T>...>;
  using test_test = std::index_sequence<PAT_IDS...>;
  using type =
      std::variant<WrappedParser_t<PAT_IDS, CTX_T, DST_T, CHILDS_TUPLE_T>...>;
};
}  // namespace seq_
template <typename... CHILD_PATS_T>
class Seq : public Pattern<Seq<CHILD_PATS_T...>> {
 public:
  using child_tuple_t = std::tuple<CHILD_PATS_T...>;
  // The computed type for individual members of the sequence
  Seq(child_tuple_t const& childs) : childs_(childs) {}
  child_tuple_t const& childs() const { return childs_; }
 private:
  child_tuple_t childs_;
};
template <std::size_t Index, typename... CHILD_PATS_T>
auto const& getChild(Seq<CHILD_PATS_T...> const& pat) {
  return std::get<Index>(pat.childs());
}
template <typename... CHILD_PATS_T>
auto seq(CHILD_PATS_T&&... childs) {
  return Seq<CHILD_PATS_T...>(
      std::make_tuple(std::forward<CHILD_PATS_T>(childs)...));
}
template <typename CHILD_TUP_T, typename CB_T, std::size_t... Is>
auto transform_seq_impl(CHILD_TUP_T const& c, CB_T const& cb,
                        std::index_sequence<Is...>) {
  return seq(transform(std::get<Is>(c), cb)...);
}
template <typename... CHILD_PATS_T, typename CB_T>
auto transform(Seq<CHILD_PATS_T...> const& tgt, CB_T const& cb) {
  using indices = std::make_index_sequence<sizeof...(CHILD_PATS_T)>;
  auto const& childs_tuple = tgt.childs();
  return transform_seq_impl(childs_tuple, cb, indices());
}
template <typename RECUR_TAG, typename... CHILD_PATS>
struct pattern_traits<Seq<CHILD_PATS...>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    BACKTRACKS = any_pat_traits<RECUR_TAG, CHILD_PATS...>::BACKTRACKS,
    FAILS_CLEANLY = false,
    MAY_NOT_CONSUME = all_pat_traits<RECUR_TAG, CHILD_PATS...>::MAY_NOT_CONSUME,
    PEEKABLE = false,  // TODO: not quite true, "char_() + -char_()" is peekable
                       // for example
    ATOMIC = false,
    APPENDS_DST = true,
    // TODO: sort this out
    // for a sequence to be stable, there must be no more than 1 child that
    //   is Non-Nil
    // AND
    //   is non-optional
    // AND
    //    is a non-appender
    //  OR
    //    is an unstable appender
    STABLE_APPENDS = false
  };
};
template <typename CTX_T, typename... CHILD_PATS>
struct pat_attr_t<Seq<CHILD_PATS...>, CTX_T> {
  using attr_type =
      typename seq_::determine_attr_type<CTX_T, CHILD_PATS...>::type;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
namespace act_ {
template <typename ACT_T, typename Enable = void>
struct determine_landing_type;
template <typename ACT_T>
struct determine_landing_type<ACT_T,
                              enable_if_t<function_traits<ACT_T>::arity == 0>> {
  using type = Nil;
};
template <typename ACT_T>
struct determine_landing_type<ACT_T,
                              enable_if_t<function_traits<ACT_T>::arity != 0>> {
  using type = std::decay_t<callable_argument_t<ACT_T, 0>>;
};
template <typename ACT_T, typename Enable = void>
struct determine_emmited_type;
template <typename ACT_T>
struct determine_emmited_type<
    ACT_T, enable_if_t<is_same<void, callable_result_t<ACT_T>>::value>> {
  using type = Nil;
};
template <typename ACT_T>
struct determine_emmited_type<
    ACT_T, enable_if_t<!is_same<void, callable_result_t<ACT_T>>::value>> {
  using type = callable_result_t<ACT_T>;
};
template <typename ACT_T, typename Enable = void>
struct Dispatch;
template <typename ACT_T>
struct Dispatch<ACT_T,
                enable_if_t<function_traits<ACT_T>::arity == 0 &&
                            is_same<void, callable_result_t<ACT_T>>::value>> {
  template <typename LAND_T, typename DST_T>
  static void dispatch(ACT_T const& act, LAND_T, DST_T&) {
    act();
  }
};
template <typename ACT_T>
struct Dispatch<ACT_T,
                enable_if_t<function_traits<ACT_T>::arity != 0 &&
                            is_same<void, callable_result_t<ACT_T>>::value>> {
  template <typename LAND_T, typename DST_T>
  static void dispatch(ACT_T const& act, LAND_T land, DST_T&) {
    act(std::move(land));
  }
};
template <typename ACT_T>
struct Dispatch<ACT_T,
                enable_if_t<function_traits<ACT_T>::arity == 0 &&
                            !is_same<void, callable_result_t<ACT_T>>::value>> {
  template <typename LAND_T, typename DST_T>
  static void dispatch(ACT_T const& act, LAND_T, DST_T& dst) {
    dst = act();
  }
};
template <typename ACT_T>
struct Dispatch<ACT_T,
                enable_if_t<function_traits<ACT_T>::arity != 0 &&
                            !is_same<void, callable_result_t<ACT_T>>::value>> {
  template <typename LAND_T, typename DST_T>
  static void dispatch(ACT_T const& act, LAND_T land, DST_T& dst) {
    dst = act(std::move(land));
  }
};
}  // namespace act_
template <typename CHILD_PAT_T, typename ACT_T>
class Action : public Pattern<Action<CHILD_PAT_T, ACT_T>> {
  CHILD_PAT_T pat_;
  ACT_T act_;
 public:
  Action(CHILD_PAT_T pat, ACT_T act)
      : pat_(std::move(pat)), act_(std::move(act)) {}
  CHILD_PAT_T const& child_pattern() const { return pat_; }
  ACT_T const& action() const { return act_; }
};
template <typename CHILD_PAT_T, typename ACT_T, typename RECUR_TAG>
struct pattern_traits<Action<CHILD_PAT_T, ACT_T>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    BACKTRACKS = pattern_traits<CHILD_PAT_T, RECUR_TAG>::BACKTRACKS,
    FAILS_CLEANLY = pattern_traits<CHILD_PAT_T, RECUR_TAG>::FAILS_CLEANLY,
    MAY_NOT_CONSUME = pattern_traits<CHILD_PAT_T, RECUR_TAG>::MAY_NOT_CONSUME,
    PEEKABLE = false,
    ATOMIC = true,
    APPENDS_DST = false,     // yish...
    STABLE_APPENDS = false,  // yish...
  };
};
template <typename CHILD_PAT_T, typename ACT_T, typename CTX_T>
struct pat_attr_t<Action<CHILD_PAT_T, ACT_T>, CTX_T> {
  using attr_type = typename act_::determine_emmited_type<ACT_T>::type;
};
template <typename PAT_T, typename ACT_T>
auto apply_action(PAT_T&& pat, ACT_T&& act) {
  return Action<std::decay_t<PAT_T>, std::decay_t<ACT_T>>(
      std::forward<PAT_T>(pat), std::forward<ACT_T>(act));
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename PAT_T>
class Not : public Pattern<Not<PAT_T>> {
  PAT_T child_;
 public:
  Not(const PAT_T& child) : child_(child) {}
  Not(PAT_T&& child) : child_(std::move(child)) {}
  PAT_T const& operand() const { return child_; }
};
template <typename PAT_T, typename RECUR_TAG>
struct pattern_traits<Not<PAT_T>, RECUR_TAG> : public default_pattern_traits {
  using attr_type = Nil;
  enum {
    // if we can get our info by peeking, then we will not need to backtrack
    BACKTRACKS = !pattern_traits<PAT_T, RECUR_TAG>::PEEKABLE,
    PEEKABLE = pattern_traits<PAT_T, RECUR_TAG>::PEEKABLE,
    FAILS_CLEANLY = true,
    MAY_NOT_CONSUME = true,
    ATOMIC = true,
  };
};
template <typename PAT_T, typename CTX_T>
struct pat_attr_t<Not<PAT_T>, CTX_T> {
  using attr_type = Nil;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename PAT_T>
class Optional : public Pattern<Optional<PAT_T>> {
  PAT_T child_;
 public:
  Optional(PAT_T child) : child_(std::move(child)) {}
  PAT_T const& operand() const { return child_; }
};
template <typename PAT_T, typename RECUR_TAG>
struct pattern_traits<Optional<PAT_T>, RECUR_TAG> : public default_pattern_traits {
  using attr_type = Nil;
  enum {
    BACKTRACKS = pattern_traits<PAT_T, RECUR_TAG>::BACKTRACKS,
    PEEKABLE = true,
    FAILS_CLEANLY = true,
    MAY_NOT_CONSUME = true,
    ATOMIC = true,
    APPENDS_DST = pattern_traits<PAT_T, RECUR_TAG>::APPENDS_DST,
    STABLE_APPENDS = true,
  };
};
template <typename PAT_T, typename CTX_T>
struct pat_attr_t<Optional<PAT_T>, CTX_T> {
  using attr_type = std::optional<attr_t<PAT_T, CTX_T>>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename PAT_T,
          typename Enable = enable_if_t<is_valid_unary_operand<PAT_T>()>>
auto operator*(PAT_T&& pat) {
  return repeat<0, 0>(forward<PAT_T>(pat));
}
template <typename PAT_T,
          typename Enable = enable_if_t<is_valid_unary_operand<PAT_T>()>>
auto operator+(PAT_T&& pat) {
  return repeat<1, 0>(forward<PAT_T>(pat));
}
template <typename PAT_T,
          typename Enable = enable_if_t<is_valid_unary_operand<PAT_T>(),
                                        Not<pattern_t<PAT_T>>>>
auto operator!(PAT_T&& pat) {
  return Not<pattern_t<PAT_T>>(make_pattern(pat));
}
template <typename PAT_T,
          typename Enable = enable_if_t<is_valid_unary_operand<PAT_T>(),
                                        Not<pattern_t<PAT_T>>>>
auto operator-(PAT_T&& pat) {
  return Optional<pattern_t<PAT_T>>(make_pattern(pat));
}
template <
    typename LHS_T, typename RHS_T,
    typename Enable = enable_if_t<are_valid_binary_operands<LHS_T, RHS_T>()>>
auto operator%(LHS_T&& lhs, RHS_T&& rhs) {
  return list(forward<LHS_T>(lhs), forward<RHS_T>(rhs));
}
template <
    typename LHS_T, typename RHS_T,
    typename Enable = enable_if_t<are_valid_binary_operands<LHS_T, RHS_T>()>>
auto operator-(LHS_T&& lhs, RHS_T&& rhs) {
  return except(forward<LHS_T>(lhs), forward<RHS_T>(rhs));
}
template <typename LHS_T, typename RHS_T>
std::enable_if_t<are_valid_binary_operands<LHS_T, RHS_T>(),
                 typename detail::NaryPatternBuilder<Alt, pattern_t<LHS_T>,
                                                     pattern_t<RHS_T>>::type>
operator|(LHS_T&& lhs, RHS_T&& rhs) {
  return detail::NaryPatternBuilder<Alt, pattern_t<LHS_T>, pattern_t<RHS_T>>::
      build(make_pattern(forward<LHS_T>(lhs)),
            make_pattern(forward<RHS_T>(rhs)));
}
template <typename LHS_T, typename RHS_T>
std::enable_if_t<are_valid_binary_operands<LHS_T, RHS_T>(),
                 typename detail::NaryPatternBuilder<Seq, pattern_t<LHS_T>,
                                                     pattern_t<RHS_T>>::type>
operator>>(LHS_T&& lhs, RHS_T&& rhs) {
  return detail::NaryPatternBuilder<Seq, pattern_t<LHS_T>, pattern_t<RHS_T>>::
      build(make_pattern(forward<LHS_T>(lhs)),
            make_pattern(forward<RHS_T>(rhs)));
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
static constexpr Int<10, 1, 0> int_;
static constexpr Uint<10, 1, 0> uint_;
static constexpr Eoi eoi;
static constexpr Fail fail;
static constexpr Pass pass;
template <typename LHS_T, typename RHS_T>
auto except(LHS_T&& lhs, RHS_T&& rhs) {
  return Except<pattern_t<LHS_T>, pattern_t<RHS_T>>(
      make_pattern(forward<LHS_T>(lhs)), make_pattern(forward<RHS_T>(rhs)));
}
template <typename LHS_T, typename RHS_T>
auto list(LHS_T&& lhs, RHS_T&& rhs) {
  return List<pattern_t<LHS_T>, pattern_t<RHS_T>>(
      make_pattern(forward<LHS_T>(lhs)), make_pattern(forward<RHS_T>(rhs)));
}
template <typename CHAR_T, typename VAL_T>
auto symbol(std::map<CHAR_T, VAL_T> const& vals) {
  return CharSymbol<CHAR_T, VAL_T>(vals);
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <int BASE, typename Enabled = void>
struct DigitValues;
template <int BASE>
struct DigitValues<BASE, enable_if_t<(BASE <= 10U)>> {
  static_assert(BASE >= 2, "");
  static_assert(BASE <= 35, "");
  static bool is_valid(char c) { return c >= '0' && c <= ('0' + BASE - 1); }
  static int value(char c) { return c - '0'; }
};
template <int BASE>
struct DigitValues<BASE, enable_if_t<(BASE > 10U) && (BASE <= 35U)>> {
  static_assert(BASE >= 2, "");
  static_assert(BASE <= 35, "");
  static bool is_valid(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= ('a' + BASE - 11)) ||
           (c >= 'A' && c <= ('A' + BASE - 11));
  }
  static int value(char c) {
    if (c >= '0' && c <= '9') {
      return c - '0';
    }
    if (c >= 'a' && c <= 'z') {
      return 10 + c - 'a';
    }
    if (c >= 'A' && c <= 'Z') {
      return 10 + c - 'A';
    }
    return 0;
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, int BASE, std::size_t DIGITS_MIN,
          std::size_t DIGITS_MAX>
class Parser<CTX_T, DST_T, Int<BASE, DIGITS_MIN, DIGITS_MAX>>
    : public ParserBase<CTX_T, DST_T> {
  using digit_vals = DigitValues<BASE>;
  using PAT_T = Int<BASE, DIGITS_MIN, DIGITS_MAX>;
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const&)
      : ParserBase<CTX_T, DST_T>(ctx, dst) {
    dst = 0;
  }
  result consume(CTX_T& ctx, DST_T& dst, PAT_T const&) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }
    while (true) {
      if (ctx.empty()) {
        if (ctx.final_buffer()) {
          return digit_count_ >= DIGITS_MIN ? result::SUCCESS : result::FAILURE;
        } else {
          return result::PARTIAL;
        }
      }
      auto next = ctx.next();
      if (digit_count_ == 0 && look_for_sign_) {
        look_for_sign_ = false;
        if (next == '-') {
          ctx.advance();
          neg_ = true;
          continue;
        }
        if (next == '+') {
          ctx.advance();
          continue;
        }
      }
      if (digit_vals::is_valid(next)) {
        dst *= BASE;
        dst += digit_vals::value(next);
        ++digit_count_;
        ctx.advance();
        if (digit_count_ == DIGITS_MAX) {
          if (neg_) {
            dst *= -1;
          }
          return result::SUCCESS;
        }
      } else {
        if (neg_) {
          dst *= -1;
        }
        return digit_count_ >= DIGITS_MIN ? result::SUCCESS : result::FAILURE;
      }
    }
  }
 private:
  std::size_t digit_count_ = 0;
  bool look_for_sign_ = true;
  bool neg_ = false;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, int BASE, std::size_t DIGITS_MIN,
          std::size_t DIGITS_MAX>
class Parser<CTX_T, DST_T, Uint<BASE, DIGITS_MIN, DIGITS_MAX>>
    : public ParserBase<CTX_T, DST_T> {
  using digit_vals = DigitValues<BASE>;
  using PAT_T = Uint<BASE, DIGITS_MIN, DIGITS_MAX>;
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const&)
      : ParserBase<CTX_T, DST_T>(ctx, dst) {
    dst = 0;
  }
  result peek(CTX_T const& ctx, PAT_T const&) const {
    static_assert(DIGITS_MIN == 1, "we should not be peeking here.");
    if (ctx.empty()) {
      if (ctx.final_buffer()) {
        return result::FAILURE;
      } else {
        return result::PARTIAL;
      }
    }
    auto next = ctx.next();
    if (digit_vals::is_valid(next)) {
      return result::SUCCESS;
    } else {
      return result::FAILURE;
    }
  }
  result consume(CTX_T& ctx, DST_T& dst, PAT_T const&) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }
    while (true) {
      if (ctx.empty()) {
        if (ctx.final_buffer()) {
          return digit_count_ >= DIGITS_MIN ? result::SUCCESS : result::FAILURE;
        } else {
          return result::PARTIAL;
        }
      }
      auto next = ctx.next();
      if (digit_vals::is_valid(next)) {
        dst *= BASE;
        dst += digit_vals::value(next);
        ++digit_count_;
        ctx.advance();
        if (digit_count_ == DIGITS_MAX) {
          return result::SUCCESS;
        }
      } else {
        return digit_count_ >= DIGITS_MIN ? result::SUCCESS : result::FAILURE;
      }
    }
  }
 private:
  std::size_t digit_count_ = 0;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename CHAR_T, typename VAL_T>
class Parser<CTX_T, DST_T, CharSymbol<CHAR_T, VAL_T>>
    : public ParserBase<CTX_T, DST_T> {
  using PAT_T = CharSymbol<CHAR_T, VAL_T>;
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst) {}
  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }
    if (ctx.empty()) {
      return ctx.final_buffer() ? result::FAILURE : result::PARTIAL;
    }
    auto next = ctx.next();
    auto found = pat.mapping().find(next);
    if (found == pat.mapping().end()) {
      return result::FAILURE;
    }
    dst = found->second;
    return result::SUCCESS;
  }
  result peek(CTX_T& ctx, PAT_T const& pat) {
    if (ctx.empty()) {
      return ctx.final_buffer() ? result::FAILURE : result::PARTIAL;
    }
    auto next = ctx.next();
    auto found = pat.mapping().find(next);
    if (found == pat.mapping().end()) {
      return result::FAILURE;
    }
    return result::SUCCESS;
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename CHARSET_T>
class Parser<CTX_T, DST_T, Char<CHARSET_T>> : public ParserBase<CTX_T, DST_T> {
  using PAT_T = Char<CHARSET_T>;
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const&)
      : ParserBase<CTX_T, DST_T>(ctx, dst) {}
  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    // std::cout << "char is writing at: " << std::hex << (uint64_t)&dst <<
    // std::endl;
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }
    if (ctx.empty()) {
      return ctx.final_buffer() ? result::FAILURE : result::PARTIAL;
    }
    auto next = ctx.next();
    if (pat.char_set().is_valid(next)) {
      dst = next;
      ctx.advance();
      return result::SUCCESS;
    }
    return result::FAILURE;
  }
  result peek(CTX_T const& ctx, PAT_T const& pat) const {
    if (ctx.empty()) {
      return ctx.final_buffer() ? result::FAILURE : result::PARTIAL;
    }
    return pat.char_set().is_valid(ctx.next()) ? result::SUCCESS
                                               : result::FAILURE;
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T>
class Parser<CTX_T, Nil, Eoi> : public ParserBase<CTX_T, Nil> {
  using PAT_T = Eoi;
 public:
  Parser(CTX_T& ctx, Nil& dst, PAT_T const&)
      : ParserBase<CTX_T, Nil>(ctx, dst) {}
  result consume(CTX_T& ctx, Nil&, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }
    return peek(ctx, pat);
  }
  result peek(CTX_T& ctx, PAT_T const&) {
    if (ctx.empty()) {
      return ctx.final_buffer() ? result::SUCCESS : result::PARTIAL;
    }
    return result::FAILURE;
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T>
class Parser<CTX_T, Nil, Fail>
    : public ParserBase<CTX_T, Nil, PARSER_OPT_NO_SKIP> {
  using PAT_T = Fail;
 public:
  Parser(CTX_T& ctx, Nil&, PAT_T const&)
      : ParserBase<CTX_T, Nil, PARSER_OPT_NO_SKIP>(ctx, nil) {}
  result consume(CTX_T&, Nil&, PAT_T const&) { return result::FAILURE; }
  result peek(CTX_T&, PAT_T const&) { return result::FAILURE; }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T>
class Parser<CTX_T, Nil, Pass>
    : public ParserBase<CTX_T, Nil, PARSER_OPT_NO_SKIP> {
  using PAT_T = Pass;
 public:
  Parser(CTX_T& ctx, Nil&, PAT_T const&)
      : ParserBase<CTX_T, Nil, PARSER_OPT_NO_SKIP>(ctx, nil) {}
  result consume(CTX_T&, Nil&, PAT_T const&) { return result::SUCCESS; }
  result peek(CTX_T&, PAT_T const&) { return result::SUCCESS; }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename CHAR_T>
class Parser<CTX_T, DST_T, StringLiteral<CHAR_T>>
    : public ParserBase<CTX_T, DST_T> {
  using PAT_T = StringLiteral<CHAR_T>;
  typename std::basic_string<CHAR_T>::const_iterator next_expected_;
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst), next_expected_(pat.begin()) {}
  result consume(CTX_T& ctx, Nil&, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }
    while (1) {
      if (next_expected_ == pat.end()) {
        return result::SUCCESS;
      }
      if (ctx.empty()) {
        return ctx.final_buffer() ? result::FAILURE : result::PARTIAL;
      }
      auto next = ctx.next();
      if (next == *next_expected_) {
        ctx.advance();
        ++next_expected_;
      } else {
        return result::FAILURE;
      }
    }
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename CHAR_T, typename VAL_T>
class Parser<CTX_T, DST_T, Symbol<CHAR_T, VAL_T>>
    : public ParserBase<CTX_T, DST_T> {
  using PAT_T = Symbol<CHAR_T, VAL_T>;
  using node_t = typename PAT_T::node_t;
  node_t const* next_ = nullptr;
  node_t const* current_valid_ = nullptr;
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst), next_(pat.root()) {}
  result consume(CTX_T& ctx, DST_T& dst, PAT_T const&) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }
    while (1) {
      if (ctx.empty()) {
        if (ctx.final_buffer()) {
          if (current_valid_) {
            dst = *current_valid_->val;
            ctx.commit_rollback();
            return result::SUCCESS;
          } else {
            return result::FAILURE;
          }
        } else {
          // If we were conclusively done, we would have returned success
          // before looping.
          return result::PARTIAL;
        }
      }
      auto next = ctx.next();
      auto found = next_->child.find(next);
      if (found == next_->child.end()) {
        // the next character leads nowhere
        if (current_valid_) {
          // we had a match along the way
          dst = *current_valid_->val;
          ctx.commit_rollback();
          return result::SUCCESS;
        }
        return result::FAILURE;
      } else {
        // consume the value
        ctx.advance();
        next_ = &found->second;
        if (next_->val) {
          // we got a hit!
          if (current_valid_) {
            ctx.cancel_rollback();
          }
          if (next_->child.empty()) {
            // nowhere to go from here
            dst = *next_->val;
            return result::SUCCESS;
          }
          current_valid_ = next_;
          ctx.prepare_rollback();
        }
      }
    }
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename OP_T, typename NEG_T>
class Parser<CTX_T, DST_T, Except<OP_T, NEG_T>>
    : public ParserBase<CTX_T, DST_T> {
  using PAT_T = Except<OP_T, NEG_T>;
  using op_parser_t = Parser<CTX_T, DST_T, OP_T>;
  using neg_parser_t = Parser<CTX_T, Nil, NEG_T>;
  using child_parsers_t = std::variant<neg_parser_t, op_parser_t>;
  child_parsers_t child_parsers_;
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst),
        child_parsers_(std::in_place_index_t<0>(), ctx, nil, pat.neg()) {
    constexpr bool backtrack = !pattern_traits<NEG_T, void>::FAILS_CLEANLY;
    if (backtrack) {
      ctx.prepare_rollback();
    }
  }
  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }
    constexpr bool backtrack = !pattern_traits<NEG_T, void>::FAILS_CLEANLY;
    if (child_parsers_.index() == 0) {
      auto res = std::get<0>(child_parsers_).consume(ctx, nil, pat.neg());
      switch (res) {
        case result::PARTIAL:
          return result::PARTIAL;
        case result::SUCCESS:
          if (backtrack) {
            constexpr bool commit = pattern_traits<OP_T, void>::FAILS_CLEANLY;
            if (commit) {
              ctx.commit_rollback();
            } else {
              // we cannot promise FAILS_CLEANLY since LHS_T does not, so we
              // might as well just cancel the rollback.
              ctx.cancel_rollback();
            }
          }
          return result::FAILURE;
        case result::FAILURE:
          if (backtrack) {
            ctx.commit_rollback();
          }
          child_parsers_ =
              child_parsers_t(std::in_place_index_t<1>(), ctx, dst, pat.op());
      }
    }
    abu_assume(child_parsers_.index() == 1);
    return std::get<1>(child_parsers_).consume(ctx, dst, pat.op());
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename VAL_PAT_T,
          typename SEP_PAT_T>
class Parser<CTX_T, DST_T, List<VAL_PAT_T, SEP_PAT_T>>
    : public ParserBase<CTX_T, DST_T> {
  using PAT_T = List<VAL_PAT_T, SEP_PAT_T>;
  using child_adapter_t =
      buf_::CollectionParserAdapter<CTX_T, DST_T, VAL_PAT_T>;
  using sep_pat_parser_t = Parser<CTX_T, Nil, SEP_PAT_T>;
  using child_parser_t = std::variant<child_adapter_t, sep_pat_parser_t>;
  child_parser_t child_parser_;
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst),
        child_parser_(std::in_place_index_t<0>(), ctx, dst, pat.op()) {
    constexpr bool backtrack =
        !pattern_traits<VAL_PAT_T, void>::FAILS_CLEANLY ||
        !pattern_traits<SEP_PAT_T, void>::FAILS_CLEANLY;
    if (backtrack) {
      ctx.prepare_rollback();
    }
    dst.clear();
  }
  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }
    constexpr bool backtrack =
        !pattern_traits<VAL_PAT_T, void>::FAILS_CLEANLY ||
        !pattern_traits<SEP_PAT_T, void>::FAILS_CLEANLY;
    while (1) {
      if (child_parser_.index() == 0) {
        // We are parsing a value.
        auto child_res = std::get<0>(child_parser_).consume(ctx, dst, pat.op());
        switch (child_res) {
          case result::SUCCESS: {
            if (backtrack) {
              ctx.cancel_rollback();
              ctx.prepare_rollback();
            }
            child_parser_ =
                child_parser_t(std::in_place_index_t<1>(), ctx, nil, pat.sep());
          } break;
          case result::FAILURE:
            // this will cancel the consumption of the separator if there was
            // any
            if (backtrack) {
              ctx.commit_rollback();
            }
            return result::SUCCESS;
          case result::PARTIAL:
            return result::PARTIAL;
        }
      } else {
        abu_assume(child_parser_.index() == 1);
        // We are parsing a separator
        auto child_res =
            std::get<1>(child_parser_).consume(ctx, nil, pat.sep());
        switch (child_res) {
          case result::SUCCESS:
            child_parser_ =
                child_parser_t(std::in_place_index_t<0>(), ctx, dst, pat.op());
            break;
          case result::FAILURE:
            // rollback whatever the separator may have eaten
            if (backtrack) {
              ctx.commit_rollback();
            }
            return result::SUCCESS;
          case result::PARTIAL:
            return result::PARTIAL;
        }
      }
    }
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename... CHILD_PATS_T>
class Parser<CTX_T, DST_T, Alt<CHILD_PATS_T...>>
    : public ParserBase<CTX_T, DST_T> {
  using PAT_T = Alt<CHILD_PATS_T...>;
  using child_pat_tuple_t = typename PAT_T::child_tuple_t;
  using child_parsers_t = std::variant<Parser<CTX_T, DST_T, CHILD_PATS_T>...>;
  child_parsers_t child_parsers_;
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst),
        child_parsers_(std::in_place_index_t<0>(), ctx, dst, getChild<0>(pat)) {
    if (rolls_back_at<0>()) {
      ctx.prepare_rollback();
    }
  }
  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }
    if (CTX_T::IS_RESUMABLE) {
      return visit_val<sizeof...(CHILD_PATS_T)>(
          child_parsers_.index(),
          [&](auto N) { return this->consume_from<N()>(ctx, dst, pat); });
    } else {
      // Skip the visitation when using non_resumable parsers.
      return consume_from<0>(ctx, dst, pat);
    }
  }
  template <std::size_t ID>
  result consume_from(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    abu_assume(child_parsers_.index() == ID);
    auto& c_parser = std::get<ID>(child_parsers_);
    auto const& c_pattern = getChild<ID>(pat);
    result child_res = c_parser.consume(ctx, dst, c_pattern);
    if (result::FAILURE == child_res) {
      if (rolls_back_at<ID>()) {
        ctx.commit_rollback();
      }
      constexpr int next_id = ID + 1;
      // if we have reached the end of the child parsers list
      if (sizeof...(CHILD_PATS_T) == next_id) {
        return result::FAILURE;
      } else {
        constexpr int new_id = next_id < sizeof...(CHILD_PATS_T) ? next_id : 0;
        auto const& new_c_pattern = getChild<new_id>(pat);
        child_parsers_ = child_parsers_t(std::in_place_index_t<new_id>(),
                                         make_parser_(ctx, dst, new_c_pattern));
        if (rolls_back_at<new_id>()) {
          ctx.prepare_rollback();
        }
        return consume_from<new_id>(ctx, dst, pat);
      }
    }
    if (child_res == result::SUCCESS) {
      if (rolls_back_at<ID>()) {
        ctx.cancel_rollback();
      }
    }
    return child_res;
  }
 private:
  template <std::size_t N>
  static constexpr bool rolls_back_at() {
    return !pattern_traits<std::tuple_element_t<N, child_pat_tuple_t>,
                           void>::FAILS_CLEANLY;
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename... CHILD_PATS_T>
class Parser<CTX_T, DST_T, Seq<CHILD_PATS_T...>>
    : public ParserBase<CTX_T, DST_T> {
  using PAT_T = Seq<CHILD_PATS_T...>;
  using childs_tuple_t = std::tuple<CHILD_PATS_T...>;
  using child_parsers_t = typename seq_::SeqSubParser<
      CTX_T, DST_T, childs_tuple_t,
      std::index_sequence_for<CHILD_PATS_T...>>::type;
  child_parsers_t child_parsers_;
 public:
  template <std::size_t ID>
  decltype(auto) getDstFor(DST_T& dst) {
    using accessor_t =
        seq_::choose_dst_accessor<ID, CTX_T, DST_T, childs_tuple_t>;
    return accessor_t::access(dst);
  }
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst),
        child_parsers_(
            std::in_place_index_t<0>(),
            std::variant_alternative_t<0, child_parsers_t>(
                ctx, force_lvalue(getDstFor<0>(dst)), getChild<0>(pat))) {
    reset_if_collection<DST_T>::exec(dst);
  }
  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }
    if (CTX_T::IS_RESUMABLE) {
      return visit_val<sizeof...(CHILD_PATS_T)>(
          child_parsers_.index(),
          [&](auto N) { return this->consume_from<N()>(ctx, dst, pat); });
    } else {
      // Skip the visitation when using non_resumable parsers.
      return consume_from<0>(ctx, dst, pat);
    }
  }
  template <std::size_t ID>
  result consume_from(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    abu_assume(child_parsers_.index() == ID);
    auto& c_parser = std::get<ID>(child_parsers_);
    auto const& c_pattern = getChild<ID>(pat);
    result child_res =
        c_parser.consume(ctx, force_lvalue(getDstFor<ID>(dst)), c_pattern);
    if (result::SUCCESS == child_res) {
      constexpr int next_id = ID + 1;
      // if we have reached the end of the child parsers list
      if (sizeof...(CHILD_PATS_T) == next_id) {
        return result::SUCCESS;
      } else {
        // This does not matter nearly that much, as we will never enter
        // here with a saturated value
        // The sole purpose of the max is to prevent reset<N>() from
        // being called with an out of bounds value,
        // which would cause a compile-time error.
        constexpr int new_id = next_id < sizeof...(CHILD_PATS_T) ? next_id : 0;
        auto const& new_c_pattern = getChild<new_id>(pat);
        child_parsers_ = child_parsers_t(
            std::in_place_index_t<new_id>(),
            make_parser_(ctx, force_lvalue(getDstFor<new_id>(dst)),
                         new_c_pattern));
        return consume_from<new_id>(ctx, dst, pat);
      }
    }
    return child_res;
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename CHILD_PAT_T, typename ACT_T>
class Parser<CTX_T, DST_T, Action<CHILD_PAT_T, ACT_T>>
    : public ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP> {
  using PAT_T = Action<CHILD_PAT_T, ACT_T>;
  using child_ctx_t = CTX_T;  // for now, but I suspect this may change.
  using landing_type_t = typename act_::determine_landing_type<ACT_T>::type;
  using child_parser_t = Parser<child_ctx_t, landing_type_t, CHILD_PAT_T>;
  landing_type_t landing;
  child_parser_t child_parser_;
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP>(ctx, dst),
        child_parser_(ctx, landing, pat.child_pattern()) {}
  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    auto status = child_parser_.consume(ctx, landing, pat.child_pattern());
    if (status == result::SUCCESS) {
      act_::Dispatch<ACT_T>::template dispatch(pat.action(), std::move(landing),
                                               dst);
    }
    return status;
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename ATTR_T, typename CHILD_PAT_T>
class Parser<CTX_T, DST_T, AttrCast<ATTR_T, CHILD_PAT_T>>
    : public ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP> {
  using PAT_T = AttrCast<ATTR_T, CHILD_PAT_T>;
  using parser_t = Parser<CTX_T, DST_T, CHILD_PAT_T>;
  parser_t parser_;
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP>(ctx, dst),
        parser_(ctx, dst, pat.operand()) {}
  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    return parser_.consume(ctx, dst, pat.operand());
  }
  result peek(CTX_T& ctx, PAT_T const& pat) {
    return parser_.peek(ctx, pat.operand());
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename CHILD_PAT_T>
class Parser<CTX_T, DST_T, Optional<CHILD_PAT_T>> : public ParserBase<CTX_T, DST_T> {
  using PAT_T = Optional<CHILD_PAT_T>;
  using child_parser_t = Parser<CTX_T, DST_T, CHILD_PAT_T>;
  child_parser_t parser_;
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst), parser_(ctx, dst, pat.operand()) {
    ctx.prepare_rollback();
  }
  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }
    auto status = parser_.consume(ctx, dst, pat.operand());
    switch (status) {
      case result::SUCCESS:
        return result::SUCCESS;
      case result::FAILURE:
        ctx.commit_rollback();
        return result::SUCCESS;
      case result::PARTIAL:
        return result::PARTIAL;
    }
    abu_unreachable();
  }
  result peek(CTX_T& ctx, PAT_T const& pat) {
    return result::SUCCESS;
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename CHILD_PAT_T>
class Parser<CTX_T, Nil, Not<CHILD_PAT_T>> : public ParserBase<CTX_T, Nil> {
  using DST_T = Nil;
  using PAT_T = Not<CHILD_PAT_T>;
  using child_parser_t = Parser<CTX_T, DST_T, CHILD_PAT_T>;
  child_parser_t parser_;
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, Nil>(ctx, dst), parser_(ctx, dst, pat.operand()) {
    constexpr bool backtrack = !pattern_traits<CHILD_PAT_T, void>::PEEKABLE;
    if (backtrack) {
      ctx.prepare_rollback();
    }
  }
  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }
    constexpr bool backtrack = !pattern_traits<CHILD_PAT_T, void>::PEEKABLE;
    if (backtrack) {
      auto status = parser_.consume(ctx, dst, pat.operand());
      switch (status) {
        case result::SUCCESS:
          // just commit the rollback anyways, this allows us to promise
          // FAILS_CLEANLY
          ctx.commit_rollback();
          return result::FAILURE;
        case result::FAILURE:
          ctx.commit_rollback();
          return result::SUCCESS;
        case result::PARTIAL:
          return result::PARTIAL;
      }
      abu_unreachable();
    } else {
      return peek(ctx, pat);
    }
  }
  result peek(CTX_T& ctx, PAT_T const& pat) {
    auto status = parser_.peek(ctx, pat.operand());
    switch (status) {
      case result::SUCCESS:
        return result::FAILURE;
      case result::FAILURE:
        return result::SUCCESS;
      case result::PARTIAL:
        return result::PARTIAL;
    }
    abu_unreachable();
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename CHILD_PAT_T,
          std::size_t MIN_REP, std::size_t MAX_REP>
class Parser<CTX_T, DST_T, Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>>
    : public ParserBase<CTX_T, DST_T> {
  using PAT_T = Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>;
  using child_adapter_t =
      buf_::CollectionParserAdapter<CTX_T, DST_T, CHILD_PAT_T>;
  std::size_t count_ = 0;
  child_adapter_t child_parser_;
  enum { needs_backtrack = !pattern_traits<PAT_T, void>::FAILS_CLEANLY };
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst),
        child_parser_(ctx, dst, pat.operand()) {
    dst.clear();
    if (needs_backtrack) {
      ctx.prepare_rollback();
    }
  }
  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }
    while (1) {
      auto child_res = child_parser_.consume(ctx, dst, pat.operand());
      switch (child_res) {
        case result::FAILURE:
          // cancel the child's data consumption
          // Technically, we could cancel the rollback in the failure branch,
          // but guaranteeing FAILS_CLEANLY is better.
          if (needs_backtrack) {
            ctx.commit_rollback();
          }
          if (count_ >= MIN_REP) {
            return result::SUCCESS;
          } else {
            return result::FAILURE;
          }
        case result::PARTIAL:
          return result::PARTIAL;
        case result::SUCCESS:
          count_++;
          if (needs_backtrack) {
            ctx.cancel_rollback();
          }
          if (MAX_REP != 0 && count_ == MAX_REP) {
            return result::SUCCESS;
          }
          if (needs_backtrack) {
            ctx.prepare_rollback();
          }
          // If we are still going, then we need to reset the child's parser
          child_parser_ = child_adapter_t(ctx, dst, pat.operand());
      }
    }
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename CHILD_PAT_T, typename ATTR_T>
class Parser<CTX_T, DST_T, Recur<CHILD_PAT_T, ATTR_T>>
    : public ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP> {
  using PAT_T = Recur<CHILD_PAT_T, ATTR_T>;
  using operand_pat_t = typename PAT_T::operand_pat_t;
  using operand_parser_t = Parser<CTX_T, DST_T, operand_pat_t>;
  std::unique_ptr<operand_parser_t> child_parser_;
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const&)
      : ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP>(ctx, dst) {
    // We do not create the child parser here, since this is a recursive
    // process.
  }
  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (!child_parser_) {
      child_parser_ =
          std::make_unique<operand_parser_t>(ctx, dst, pat.operand());
    }
    return child_parser_->consume(ctx, dst, pat.operand());
  }
};
template <typename CTX_T, typename DST_T, typename CHILD_PAT_T, typename ATTR_T>
class Parser<CTX_T, DST_T, WeakRecur<CHILD_PAT_T, ATTR_T>>
    : public ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP> {
  using PAT_T = WeakRecur<CHILD_PAT_T, ATTR_T>;
  using operand_pat_t = typename PAT_T::operand_pat_t;
  using operand_parser_t = Parser<CTX_T, DST_T, operand_pat_t>;
  std::unique_ptr<operand_parser_t> child_parser_;
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const&)
      : ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP>(ctx, dst) {
    // We do not create the child parser here, since this is a recursive
    // process.
  }
  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (!child_parser_) {
      child_parser_ =
          std::make_unique<operand_parser_t>(ctx, dst, pat.operand());
    }
    return child_parser_->consume(ctx, dst, pat.operand());
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename CHILD_PAT_T, typename SKIP_T>
class Parser<CTX_T, DST_T, WithSkipper<CHILD_PAT_T, SKIP_T>>
    : public ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP> {
  // PARSER_OPT_NO_SKIP because we need to kill any existing skipper
  using PAT_T = WithSkipper<CHILD_PAT_T, SKIP_T>;
  using sub_ctx_t = SkipperAdapter<CTX_T, SKIP_T>;
  Parser<sub_ctx_t, DST_T, CHILD_PAT_T> child_parser_;
 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP>(ctx, dst),
        child_parser_(force_lvalue(sub_ctx_t(ctx, pat.getSkip())), dst,
                      pat.getChild()) {}
  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    sub_ctx_t sub_ctx(ctx, pat.getSkip());
    return child_parser_.consume(sub_ctx, dst, pat.getChild());
  }
};
}  // namespace ABULAFIA_NAMESPACE



#endif
