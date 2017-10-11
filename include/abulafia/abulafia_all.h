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
#include <deque>
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
template <typename T, typename Enable = void>
struct to_char_set_impl;
template <typename T>
struct to_char_set_impl<T, std::enable_if_t<is_char_set<T>::value>> {
  static T const& convert(T const& v) { return v; }
};
template <typename T>
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
  bool is_valid(char_t const &) const { return true; }
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
  bool is_valid(char_t const& c) const { return cb_(c); }
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
  bool is_valid(char_t const& c) const { return !arg_.is_valid(c); }
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
template <typename T>
struct is_collection : public std::false_type {};
template <typename T, typename ALLOC>
struct is_collection<std::vector<T, ALLOC>> : public std::true_type {};
template <typename T, typename ALLOC>
struct is_collection<std::list<T, ALLOC>> : public std::true_type {};
template <typename T, typename ALLOC>
struct is_collection<std::deque<T, ALLOC>> : public std::true_type {};
template <typename C, typename T, typename A>
struct is_collection<std::basic_string<C, T, A>> : public std::true_type {};
template <typename T, typename ENABLE = void>
struct reset_if_collection {
  static void exec(T&) {}
};
template <typename T>
struct reset_if_collection<T, enable_if_t<is_collection<T>::value>> {
  static void exec(T& c) { c.clear(); }
};
template <template <typename...> class C, typename... Ts>
std::true_type is_base_of_template_impl(const C<Ts...>*);
template <template <typename...> class C>
std::false_type is_base_of_template_impl(...);
template <typename T, template <typename...> class C>
using is_base_of_template =
    decltype(is_base_of_template_impl<C>(std::declval<T*>()));
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
  static_assert(is_same<typename LHS_T::char_t, typename RHS_T::char_t>::value);
};
template <typename LHS_T, typename RHS_T>
auto or_impl(LHS_T lhs, RHS_T rhs) {
  auto lhs_cs = to_char_set(std::decay_t<LHS_T>(lhs));
  auto rhs_cs = to_char_set(std::decay_t<RHS_T>(rhs));
  return Or<decltype(lhs_cs), decltype(rhs_cs)>(lhs_cs, rhs_cs);
}
template <typename LHS_T, typename RHS_T,
          typename = enable_if_t<is_char_set<LHS_T>::value ||
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
  Range(CHAR_T b, CHAR_T e) : begin_(b), end_(e) { assert(b <= e); }
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
template <typename CHAR_T>
struct IndexedSet : public CharacterSet {
  using char_t = CHAR_T;
  template <typename ITE_T>
  IndexedSet(ITE_T b, ITE_T e) {
    for (; b != e; ++b) {
      characters_[as_index(*b)] = true;
    }
  }
  bool is_valid(const char_t& c) const { return characters_.test(as_index(c)); }
 private:
  using unsigned_t = std::make_unsigned_t<CHAR_T>;
  static constexpr std::size_t as_index(CHAR_T c) { return unsigned_t(c); }
  std::bitset<std::numeric_limits<unsigned_t>::max() + 1> characters_;
};
template <>
struct Set<char> : public IndexedSet<char> {
  using IndexedSet::IndexedSet;
};
template <typename ITE>
auto set(ITE b, ITE e) {
  return Set<typename ITE::value_type>(b, e);
}
inline auto set(char const* v) { return Set<char>(v, v + std::strlen(v)); }
template <>
struct to_char_set_impl<const char*, void> {
  static Set<char> convert(char const* v) { return set(v); }
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
template <>
struct to_char_set_impl<char, void> {
  static Single<char> convert(char const& v) { return Single<char>(v); }
};
}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
enum class IsFinal { FINAL, NOT_FINAL };
template <typename CONTAINER_T>
class ContainerSequenceDataSource {
  using iterator = typename CONTAINER_T::const_iterator;
  using buffer_list_t = std::list<std::shared_ptr<CONTAINER_T>>;
  using buffer_iterator = typename buffer_list_t::iterator;
  buffer_list_t buffers_;
  bool final_ = false;
  iterator current_;
  buffer_iterator current_buffer_;
  using rollback_entry_t = std::pair<iterator, buffer_iterator>;
  std::vector<rollback_entry_t> rollback_stack_;
  unsigned int empty_rollbacks_ = 0;
 public:
  using value_type = typename CONTAINER_T::value_type;
  enum { IS_RESUMABLE = true };
  ContainerSequenceDataSource() : current_buffer_(buffers_.end()) {}
  void add_buffer(std::shared_ptr<CONTAINER_T> b,
                  IsFinal f = IsFinal::NOT_FINAL) {
    assert(!final_);
    if (b->begin() != b->end()) {
      bool is_empty = empty();
      buffers_.push_back(b);
      // if we were empty, bootstrap.
      if (is_empty) {
        current_buffer_ = std::prev(buffers_.end());
        current_ = (*current_buffer_)->begin();
        for (unsigned int i = 0; i < empty_rollbacks_; ++i) {
          rollback_stack_.emplace_back(current_, current_buffer_);
        }
        empty_rollbacks_ = 0;
      }
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
    if (empty()) {
      ++empty_rollbacks_;
    } else {
      rollback_stack_.emplace_back(current_, current_buffer_);
    }
  }
  void commit_rollback() {
    if (empty_rollbacks_) {
      --empty_rollbacks_;
    } else {
      current_buffer_ = rollback_stack_.back().second;
      current_ = rollback_stack_.back().first;
      rollback_stack_.pop_back();
      cleanup_rollback_();
    }
  }
  void cancel_rollback() {
    if (empty_rollbacks_) {
      --empty_rollbacks_;
    } else {
      rollback_stack_.pop_back();
      cleanup_rollback_();
    }
  }
  static constexpr bool isResumable() { return true; }
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
class SingleForwardDataSource {
  using iterator = ITE_T;
  std::stack<iterator> rollback_stack_;
  iterator current_;
  iterator end_;
 public:
  enum {
    HAS_SKIPPER = false,
  };
  using value_type = decltype(*(ITE_T()));
  enum { IS_RESUMABLE = false };
  SingleForwardDataSource(iterator b, iterator e) : current_(b), end_(e) {}
  SingleForwardDataSource(SingleForwardDataSource const&) = delete;
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
  static constexpr bool isResumable() { return false; }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
namespace details {
template <typename CONT_T, typename... ARGS>
void append_to_container(CONT_T& container, ARGS&&... args) {
  container.emplace_back(std::forward<ARGS>(args)...);
}
template <class CharT, class Traits = std::char_traits<CharT>,
          class Allocator = std::allocator<CharT>, typename... ARGS>
void append_to_container(std::basic_string<CharT, Traits, Allocator>& container,
                         ARGS&&... args) {
  container.push_back(std::forward<ARGS>(args)...);
}
}  // namespace details
template <typename T>
class CollectionWrapper {
 public:
  using dst_type = T;
  using dst_value_type = typename T::value_type;
  CollectionWrapper(T& v) : v_(&v) {}
  CollectionWrapper(CollectionWrapper const& rhs) : v_(rhs.v_) {}
  template <typename U>
  CollectionWrapper& operator=(U&& rhs) {
    details::append_to_container(*v_, std::forward<U>(rhs));
    return *this;
  }
  T& get() { return *v_; }
 private:
  T* v_;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
struct Nil {
  using value_type = Nil;
  using dst_type = Nil;
  using dst_value_type = Nil;
  Nil() = default;
  template <typename T>
  Nil(T const&) {}
  Nil& get() { return *this; }
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
  bool operator==(Nil const&) const { return true; }
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
template <typename T>
class ValueWrapper {
 public:
  using dst_type = T;
  using dst_value_type = T;
  ValueWrapper(T& v) : v_(&v) {}
  ValueWrapper(ValueWrapper const& rhs) : v_(rhs.v_) {}
  template <typename U>
  ValueWrapper& operator=(U&& v) {
    *v_ = std::forward<U>(v);
    return *this;
  }
  // using this implies that we are NOT atomic in nature.
  T& get() { return *v_; }
 private:
  T* v_;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename T, typename Enable = void>
struct SelectDstWrapper {
  using type = ValueWrapper<T>;
};
template <>
struct SelectDstWrapper<Nil> {
  using type = Nil;
};
template <typename T>
struct SelectDstWrapper<T, std::enable_if_t<is_collection<T>::value>> {
  using type = CollectionWrapper<T>;
};
template <typename T>
using wrapped_dst_t = typename SelectDstWrapper<T>::type;
template <typename T>
auto wrap_dst(T& dst) {
  return typename SelectDstWrapper<T>::type(dst);
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
template <typename T, typename Enable = void>
struct expr_traits {
  enum { is_pattern = false, converts_to_pattern = false };
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
using pattern_t =
    decay_t<decltype(expr_traits<decay_t<T>>::make_pattern(std::declval<T>()))>;
template <typename T>
inline auto make_pattern(T&& p) {
  static_assert(expr_traits<decay_t<T>>::is_pattern ||
                    expr_traits<decay_t<T>>::converts_to_pattern,
                "Cannot create pattern from T");
  return expr_traits<decay_t<T>>::make_pattern(forward<T>(p));
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
class PatternBase {};
template <typename CHILD_PAT_T, typename... ARGS_T>
class Construct;
template <typename CRTP_T>
class Pattern : public PatternBase {
 public:
  using pat_t = CRTP_T;
  template <typename ACT_T>
  auto operator[](ACT_T act) const {
    return apply_action(*static_cast<pat_t const*>(this), std::move(act));
  }
  template <typename... ARGS_T>
  auto as() const {
    return Construct<pat_t, ARGS_T...>(*static_cast<pat_t const*>(this));
  }
};
template <typename T>
struct expr_traits<T, enable_if_t<std::is_base_of<PatternBase, T>::value>> {
  enum { is_pattern = true, converts_to_pattern = false };
  static const T& make_pattern(const T& v) { return v; }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <std::size_t BASE, std::size_t DIGITS_MIN = 1,
          std::size_t DIGITS_MAX = 0>
class UInt : public Pattern<UInt<BASE, DIGITS_MIN, DIGITS_MAX>> {
 public:
  static_assert(DIGITS_MIN >= 1);
  static_assert(DIGITS_MAX >= DIGITS_MIN || DIGITS_MAX == 0);
};
static constexpr UInt<10, 1, 0> uint_;
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
enum class DstBehavior {
  IGNORE,
  VALUE,
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
enum class Result { SUCCESS, FAILURE, PARTIAL };
}

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T,
          typename PARSER_FACTORY_T>
class AtomicAdapter {
 public:
  using pat_t = typename PARSER_FACTORY_T::pat_t;
  using buffer_t = typename DST_T::dst_value_type;
  struct adapted_reqs_t : public REQ_T {
    enum { ATOMIC = false };
  };
  using adapted_dst_t = typename SelectDstWrapper<buffer_t>::type;
  using child_parser_t =
      typename PARSER_FACTORY_T::template type<CTX_T, adapted_dst_t,
                                               adapted_reqs_t>;
  AtomicAdapter(CTX_T ctx, DST_T, pat_t const& pat)
      : adapted_parser_(ctx, adapted_dst_t(buffer_), pat) {}
  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    auto status = adapted_parser_.consume(ctx, adapted_dst_t(buffer_), pat);
    if (status == Result::SUCCESS) {
      dst = buffer_;
    }
    return status;
  }
 private:
  buffer_t buffer_;
  child_parser_t adapted_parser_;
};
template <typename FACTORY_T>
struct AtomicFactoryAdapter {
  static_assert(!FACTORY_T::ATOMIC);
  static constexpr DstBehavior dst_behavior() {
    return FACTORY_T::dst_behavior();
  }
  using pat_t = typename FACTORY_T::pat_t;
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = FACTORY_T::FAILS_CLEANLY,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = AtomicAdapter<CTX_T, DST_T, REQ_T, FACTORY_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T,
          typename PARSER_FACTORY_T>
class CleanFailureAdapter {
 public:
  using pat_t = typename PARSER_FACTORY_T::pat_t;
  struct adapted_reqs_t : public REQ_T {
    enum { FAILS_CLEANLY = false };
  };
  using child_parser_t =
      typename PARSER_FACTORY_T::template type<CTX_T, DST_T, adapted_reqs_t>;
  CleanFailureAdapter(CTX_T ctx, DST_T dst, pat_t const& pat)
      : adapted_parser_(ctx, dst, pat) {
    ctx.data().prepare_rollback();
  }
  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    auto status = adapted_parser_.consume(ctx, dst, pat);
    switch (status) {
      case Result::SUCCESS:
        ctx.data().cancel_rollback();
        break;
      case Result::FAILURE:
        ctx.data().commit_rollback();
        break;
      case Result::PARTIAL:
        break;
    }
    return status;
  }
 private:
  child_parser_t adapted_parser_;
};
template <typename FACTORY_T>
struct CleanFailureFactoryAdapter {
  static_assert(!FACTORY_T::FAILS_CLEANLY);
  static constexpr DstBehavior dst_behavior() {
    return FACTORY_T::dst_behavior();
  }
  using pat_t = typename FACTORY_T::pat_t;
  enum {
    ATOMIC = FACTORY_T::ATOMIC,
    FAILS_CLEANLY = true,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = CleanFailureAdapter<CTX_T, DST_T, REQ_T, FACTORY_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
class Fail : public Pattern<Fail> {};
static constexpr Fail fail;
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename DATASOURCE_T, typename SKIPPER_T, typename BOUND_DST_T = Nil>
struct Context {
  using datasource_t = DATASOURCE_T;
  using skip_pattern_t = SKIPPER_T;
  using bound_dst_t = BOUND_DST_T;
  Context(datasource_t& ds, skip_pattern_t const& skip, BOUND_DST_T bound_dst)
      : data_(ds), skipper_(skip), bound_dst_(bound_dst) {}
  template <typename T>
  using set_skipper_t = Context<datasource_t, T, bound_dst_t>;
  template <typename DST_T>
  using bind_dst = Context<datasource_t, skip_pattern_t, DST_T>;
  enum {
    IS_RESUMABLE = DATASOURCE_T::IS_RESUMABLE,
    HAS_SKIPPER = !std::is_same<Fail, skip_pattern_t>::value,
  };
  DATASOURCE_T& data() { return data_; }
  SKIPPER_T const& skipper() { return skipper_; }
  BOUND_DST_T const& bound_dst() { return bound_dst_; }
 private:
  DATASOURCE_T& data_;
  SKIPPER_T const& skipper_;
  BOUND_DST_T bound_dst_;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
struct Parser_t;
template <typename CTX_T, typename DST_T, typename REQ_T,
          typename PARSER_FACTORY_T>
class SkipAdapter {
 public:
  using pat_t = typename PARSER_FACTORY_T::pat_t;
  using child_parser_t =
      typename PARSER_FACTORY_T::template type<CTX_T, DST_T, REQ_T>;
  using skip_context_t = Context<typename CTX_T::datasource_t, Fail, Nil>;
  struct skip_req_t {
    enum { ATOMIC = false, FAILS_CLEANLY = true, CONSUMES_ON_SUCCESS = true };
  };
  using skip_parser_t =
      Parser_t<skip_context_t, Nil, skip_req_t, typename CTX_T::skip_pattern_t>;
  SkipAdapter(CTX_T ctx, DST_T dst, pat_t const& pat)
      : skip_parser_(skip_context_t(ctx.data(), fail, nil), nil, ctx.skipper()),
        adapted_parser_(ctx, dst, pat) {}
  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    while (!skipping_done_) {
      auto status = skip_parser_.consume(skip_context_t(ctx.data(), fail, nil),
                                         nil, ctx.skipper());
      switch (status) {
        case Result::SUCCESS:
          skip_parser_ = skip_parser_t(skip_context_t(ctx.data(), fail, nil),
                                       nil, ctx.skipper());
          break;
        case Result::FAILURE:
          skipping_done_ = true;
          break;
        case Result::PARTIAL:
          return Result::PARTIAL;
          break;
      }
    }
    return adapted_parser_.consume(ctx, dst, pat);
  }
 private:
  bool skipping_done_ = false;
  skip_parser_t skip_parser_;
  child_parser_t adapted_parser_;
};
template <typename FACTORY_T>
struct SkipFactoryAdapter {
  using pat_t = typename FACTORY_T::pat_t;
  static constexpr DstBehavior dst_behavior() {
    return FACTORY_T::dst_behavior();
  }
  enum {
    ATOMIC = FACTORY_T::ATOMIC,
    FAILS_CLEANLY = FACTORY_T::FAILS_CLEANLY,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = SkipAdapter<CTX_T, DST_T, REQ_T, FACTORY_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
struct DefaultReqs {
  enum {
    ATOMIC = false,
    FAILS_CLEANLY = false,
    CONSUMES_ON_SUCCESS = false,
  };
};
template <typename PAT_T>
struct ParserFactory;
template <typename CTX_T, typename DST_T, typename REQ_T, typename REF_PAT_T>
struct RecurParserChildImpl
    : public ParserFactory<REF_PAT_T>::template type<CTX_T, DST_T, REQ_T> {
  using parent_t =
      typename ParserFactory<REF_PAT_T>::template type<CTX_T, DST_T, REQ_T>;
  using parent_t::parent_t;
};
template <typename PAT_T>
struct ParserFactory {
  // Is this not a recur or a rename?
  using pat_t = typename PAT_T::pattern_t;
  static constexpr DstBehavior dst_behavior() {
    return ParserFactory<pat_t>::dst_behavior();
  }
  enum {
    ATOMIC = ParserFactory<pat_t>::ATOMIC,
    FAILS_CLEANLY = ParserFactory<pat_t>::FAILS_CLEANLY,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = RecurParserChildImpl<CTX_T, DST_T, REQ_T, pat_t>;
};
template <typename T, template <typename> typename ADAPT_T, bool enable>
struct ConditionalAdapter {
  using type = T;
};
template <typename T, template <typename> typename ADAPT_T>
struct ConditionalAdapter<T, ADAPT_T, true> {
  using type = ADAPT_T<T>;
};
template <typename T, template <typename> typename ADAPT_T, bool enable>
using ConditionalAdapter_t =
    typename ConditionalAdapter<T, ADAPT_T, enable>::type;
template <typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
struct AdaptedParserFactory {
  static auto create(CTX_T ctx, DST_T dst, PAT_T const& pat) {
    using raw_factory = ParserFactory<PAT_T>;
    using skip_t = typename CTX_T::skip_pattern_t;
    constexpr bool apply_atomic_adapter = REQ_T::ATOMIC && !raw_factory::ATOMIC;
    constexpr bool apply_clean_failure_adapter =
        REQ_T::FAILS_CLEANLY && !raw_factory::FAILS_CLEANLY;
    constexpr bool apply_skipper_adapter = !std::is_same<skip_t, Fail>::value;
    using a = raw_factory;
    using b =
        ConditionalAdapter_t<a, AtomicFactoryAdapter, apply_atomic_adapter>;
    using c = ConditionalAdapter_t<b, CleanFailureFactoryAdapter,
                                   apply_clean_failure_adapter>;
    using d =
        ConditionalAdapter_t<c, SkipFactoryAdapter, apply_skipper_adapter>;
    // TODO: Apply skipper here.
    using parser_type = typename d::template type<CTX_T, DST_T, REQ_T>;
    return parser_type(ctx, dst, pat);
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename ITE_T, typename PAT_T, typename DST_T>
Result parse(ITE_T b, ITE_T e, const PAT_T& pat, DST_T& dst) {
  SingleForwardDataSource<ITE_T> data(b, e);
  auto real_pat = make_pattern(pat);
  auto real_dst = wrap_dst(dst);
  Context<SingleForwardDataSource<ITE_T>, Fail, decltype(real_dst)> real_ctx(
      data, fail, real_dst);
  auto parser = make_parser_(real_ctx, real_dst, DefaultReqs(), real_pat);
  return parser.consume(real_ctx, real_dst, real_pat);
}
template <typename DATA_RANGE_T, typename PAT_T, typename DST_T>
Result parse(const DATA_RANGE_T& data, const PAT_T& pat, DST_T& dst) {
  return parse(std::begin(data), std::end(data), pat, dst);
}
template <typename PAT_T, typename DATA_RANGE_T>
Result parse(const DATA_RANGE_T& data, const PAT_T& pat) {
  return parse(data, pat, nil);
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename DST_T, typename ITE_T, typename PAT_T>
DST_T decode(ITE_T b, ITE_T e, const PAT_T& pat) {
  DST_T dst;
  auto status = parse(b, e, pat, dst);
  if (status != Result::SUCCESS) {
    throw std::runtime_error("abulafia decode failure");
  }
  return dst;
}
template <typename DST_T, typename DATA_RANGE_T, typename PAT_T>
DST_T decode(const DATA_RANGE_T& data, const PAT_T& pat) {
  return decode<DST_T>(std::begin(data), std::end(data), pat);
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
using Parser =
    decltype(AdaptedParserFactory<CTX_T, DST_T, REQ_T, PAT_T>::create(
        std::declval<CTX_T>(), std::declval<DST_T>(), std::declval<PAT_T>()));
template <typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
struct Parser_t
    : decltype(AdaptedParserFactory<CTX_T, DST_T, REQ_T, PAT_T>::create(
          std::declval<CTX_T>(), std::declval<DST_T>(),
          std::declval<PAT_T>())) {
  using real_parser_t =
      decltype(AdaptedParserFactory<CTX_T, DST_T, REQ_T, PAT_T>::create(
          std::declval<CTX_T>(), std::declval<DST_T>(), std::declval<PAT_T>()));
  using real_parser_t::real_parser_t;
};
template <typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
auto make_parser_(CTX_T ctx, DST_T dst, REQ_T, PAT_T const& pat) {
  return AdaptedParserFactory<CTX_T, DST_T, REQ_T, PAT_T>::create(ctx, dst,
                                                                  pat);
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename REAL_PAT_T, typename REAL_DST_T, typename DATASOURCE_T>
struct ParserInterface {
  ParserInterface(REAL_PAT_T const& pat, REAL_DST_T& dst)
      : ctx_(data_source_, fail, dst),
        pat_(pat),
        dst_(dst),
        parser_(ctx_, dst, pat) {}
  DATASOURCE_T& data() { return data_source_; }
  Result consume() { return parser_.consume(ctx_, dst_, pat_); }
 private:
  using CTX_T = Context<DATASOURCE_T, Fail, REAL_DST_T>;
  DATASOURCE_T data_source_;
  CTX_T ctx_;
  REAL_PAT_T pat_;
  REAL_DST_T dst_;
  Parser<CTX_T, REAL_DST_T, DefaultReqs, REAL_PAT_T> parser_;
};
template <typename BUFFER_T, typename PAT_T, typename DST_T>
auto make_parser(PAT_T const& p, DST_T& s) {
  auto real_pat = make_pattern(p);
  auto real_dst = wrap_dst(s);
  return ParserInterface<decltype(real_pat), decltype(real_dst),
                         ContainerSequenceDataSource<BUFFER_T>>(real_pat,
                                                                real_dst);
}
template <typename BUFFER_T, typename PAT_T>
auto make_parser(PAT_T const& p) {
  return make_parser<BUFFER_T>(p, nil);
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
struct RecurPayload {
  virtual ~RecurPayload() {}
};
struct RecurMemoryPool {
  std::unique_ptr<RecurPayload>* alloc() {
    handles_.emplace_back();
    return &handles_.back();
  }
 private:
  std::deque<std::unique_ptr<RecurPayload>> handles_;
};
template <typename CHILD_PAT_T, typename ATTR_T = Nil>
class Recur : public Pattern<Recur<CHILD_PAT_T, ATTR_T>> {
  std::unique_ptr<RecurPayload>* pat_;
 public:
  using operand_pat_t = CHILD_PAT_T;
  using attr_t = ATTR_T;
  Recur(RecurMemoryPool& pool) : pat_(pool.alloc()) {}
  Recur(Recur const& rhs) = default;
  Recur(Recur&& rhs) = default;
  Recur& operator=(CHILD_PAT_T rhs) {
    *pat_ = std::make_unique<CHILD_PAT_T>(std::move(rhs));
    return *this;
  }
  CHILD_PAT_T const& operand() const {
    return static_cast<CHILD_PAT_T&>(**pat_);
  }
 private:
  template <typename T, typename U>
  friend class WeakRecur;
};
}  // namespace ABULAFIA_NAMESPACE
#define ABU_Recur_define(var, RECUR_TAG, pattern)               \
  struct RECUR_TAG : public ABULAFIA_NAMESPACE ::RecurPayload { \
    using pattern_t = std::decay_t<decltype(pattern)>;          \
    using impl_t = pattern_t;                                   \
    impl_t impl;                                                \
                                                                \
    RECUR_TAG(pattern_t const& p) : impl(p) {}                  \
  };                                                            \
  var = RECUR_TAG(pattern);

namespace ABULAFIA_NAMESPACE {
template <typename CHILD_PAT_T, typename SKIP_T>
class WithSkipper : public Pattern<WithSkipper<CHILD_PAT_T, SKIP_T>> {
  CHILD_PAT_T child_pat_;
  SKIP_T skip_pat_;
 public:
  CHILD_PAT_T const& getChild() const { return child_pat_; }
  SKIP_T const& getSkip() const { return skip_pat_; }
  WithSkipper(CHILD_PAT_T c, SKIP_T s)
      : child_pat_(std::move(c)), skip_pat_(std::move(s)) {}
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
template <typename OP_T, typename NEG_T>
auto except(OP_T lhs, NEG_T rhs) {
  return Except<pattern_t<OP_T>, pattern_t<NEG_T>>(
      make_pattern(std::move(lhs)), make_pattern(std::move(rhs)));
}
template <
    typename LHS_T, typename RHS_T,
    typename Enable = enable_if_t<are_valid_binary_operands<LHS_T, RHS_T>()>>
auto operator-(LHS_T lhs, RHS_T rhs) {
  return except(std::move(lhs), std::move(rhs));
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename OP_T, typename SEP_PAT_T>
class List : public Pattern<List<OP_T, SEP_PAT_T>> {
 public:
  List(OP_T val_pat, SEP_PAT_T sep)
      : val_(std::move(val_pat)), sep_(std::move(sep)) {}
  OP_T const& op() const { return val_; }
  SEP_PAT_T const& sep() const { return sep_; }
 private:
  OP_T val_;
  SEP_PAT_T sep_;
};
template <typename LHS_T, typename RHS_T>
auto list(LHS_T lhs, RHS_T rhs) {
  return List<pattern_t<LHS_T>, pattern_t<RHS_T>>(make_pattern(std::move(lhs)),
                                                  make_pattern(std::move(rhs)));
}
template <
    typename LHS_T, typename RHS_T,
    typename Enable = enable_if_t<are_valid_binary_operands<LHS_T, RHS_T>()>>
auto operator%(LHS_T&& lhs, RHS_T&& rhs) {
  return list(forward<LHS_T>(lhs), forward<RHS_T>(rhs));
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CHARSET_T>
class Char : public Pattern<Char<CHARSET_T>> {
  CHARSET_T char_set_;
 public:
  Char(CHARSET_T chars) : char_set_(std::move(chars)) {}
  CHARSET_T const& char_set() const { return char_set_; }
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
inline auto char_(const char* str) { return char_(char_set::set(str)); }
template <typename CHAR_SET_T>
struct expr_traits<CHAR_SET_T,
                   enable_if_t<char_set::is_char_set<CHAR_SET_T>::value>> {
  enum { is_pattern = false, converts_to_pattern = true };
  static Char<CHAR_SET_T> make_pattern(CHAR_SET_T const& v) { return char_(v); }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename PAT_T>
class Discard : public Pattern<Discard<PAT_T>> {
  PAT_T operand_;
 public:
  Discard(PAT_T op) : operand_(std::move(op)) {}
  PAT_T const& operand() const { return operand_; }
};
template <typename PAT_T>
auto discard(PAT_T pat) {
  return Discard<pattern_t<PAT_T>>(make_pattern(std::move(pat)));
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CHAR_T>
inline auto lit(CHAR_T chr) {
  auto res = char_(chr);
  return discard(res);
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
  CharSymbol(std::map<CHAR_T, VAL_T> vals) : mapping_(std::move(vals)) {}
  std::map<CHAR_T, VAL_T> const& mapping() const { return mapping_; }
};
template <typename CHAR_T, typename VAL_T>
auto symbol(std::map<CHAR_T, VAL_T> const& vals) {
  return CharSymbol<CHAR_T, VAL_T>(vals);
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
class Eoi : public Pattern<Eoi> {};
static constexpr Eoi eoi;
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
class Pass : public Pattern<Pass> {};
static constexpr Pass pass;
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CHAR_T>
class StringLiteral : public Pattern<StringLiteral<CHAR_T>> {
  std::shared_ptr<std::basic_string<CHAR_T>> str_;
 public:
  StringLiteral(std::basic_string<CHAR_T> str)
      : str_(std::make_shared<std::basic_string<CHAR_T>>(std::move(str))) {
    assert(str_->size() > 0);
  }
  auto begin() const { return str_->begin(); }
  auto end() const { return str_->end(); }
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
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CHAR_T, typename VAL_T>
class Symbol : public Pattern<Symbol<CHAR_T, VAL_T>> {
  // symbols->value map will be stored as a trie
  struct Node {
    std::map<CHAR_T, Node> child;
    std::optional<VAL_T> val;
  };
  std::shared_ptr<Node> root_;
 public:
  using node_t = Node;
  Symbol(std::map<std::basic_string<CHAR_T>, VAL_T> const& vals)
      : root_(std::make_shared<Node>()) {
    for (auto const& entry : vals) {
      node_t* next = root_.get();
      for (auto const& chr : entry.first) {
        next = &next->child[chr];
      }
      if (next->val) {
        throw std::runtime_error("cannot have two symbols with the same value");
      }
      next->val = entry.second;
    }
  }
  Node const* root() const { return root_.get(); }
};
template <typename CHAR_T, typename VAL_T>
auto symbol(std::map<std::basic_string<CHAR_T>, VAL_T> const& vals) {
  return Symbol<CHAR_T, VAL_T>(vals);
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <std::size_t BASE, std::size_t DIGITS_MIN = 1,
          std::size_t DIGITS_MAX = 0>
class Int : public Pattern<Int<BASE, DIGITS_MIN, DIGITS_MAX>> {
 public:
  static_assert(DIGITS_MIN >= 1);
  static_assert(DIGITS_MAX >= DIGITS_MIN || DIGITS_MAX == 0);
};
static constexpr Int<10, 1, 0> int_;
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
  static auto build(LHS_T lhs, RHS_T rhs) {
    return type(std::make_tuple(std::move(lhs), std::move(rhs)));
  }
};
template <template <typename...> typename PAT_T, typename RHS_T,
          typename... LHS_T>
struct NaryPatternBuilder<PAT_T, PAT_T<LHS_T...>, RHS_T,
                          enable_if_t<!is_nary_pattern<RHS_T, PAT_T>()>> {
  using type = PAT_T<LHS_T..., RHS_T>;
  static auto build(PAT_T<LHS_T...> const& lhs, RHS_T rhs) {
    return type(std::tuple_cat(lhs.childs(), std::make_tuple(std::move(rhs))));
  }
};
template <template <typename...> typename PAT_T, typename LHS_T,
          typename... RHS_T>
struct NaryPatternBuilder<PAT_T, LHS_T, PAT_T<RHS_T...>,
                          enable_if_t<!is_nary_pattern<LHS_T, PAT_T>()>> {
  using type = PAT_T<LHS_T, RHS_T...>;
  static auto build(LHS_T lhs, PAT_T<RHS_T...> const& rhs) {
    return type(std::tuple_cat(std::make_tuple(std::move(lhs)), rhs.childs()));
  }
};
template <template <typename...> typename PAT_T, typename... LHS_T,
          typename... RHS_T>
struct NaryPatternBuilder<PAT_T, PAT_T<LHS_T...>, PAT_T<RHS_T...>, void> {
  using type = PAT_T<LHS_T..., RHS_T...>;
  static auto build(PAT_T<LHS_T...> const& lhs, PAT_T<RHS_T...> const& rhs) {
    return type(std::tuple_cat(lhs.childs(), rhs.childs()));
  }
};
}  // namespace detail
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename... CHILD_PATS_T>
class Alt : public Pattern<Alt<CHILD_PATS_T...>> {
 public:
  using child_tuple_t = std::tuple<CHILD_PATS_T...>;
  Alt(child_tuple_t childs) : childs_(std::move(childs)) {}
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
template <typename LHS_T, typename RHS_T>
std::enable_if_t<are_valid_binary_operands<LHS_T, RHS_T>(),
                 typename detail::NaryPatternBuilder<Alt, pattern_t<LHS_T>,
                                                     pattern_t<RHS_T>>::type>
operator|(LHS_T lhs, RHS_T rhs) {
  return detail::NaryPatternBuilder<Alt, pattern_t<LHS_T>, pattern_t<RHS_T>>::
      build(make_pattern(std::move(lhs)), make_pattern(std::move(rhs)));
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
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
template <typename LHS_T, typename RHS_T>
std::enable_if_t<are_valid_binary_operands<LHS_T, RHS_T>(),
                 typename detail::NaryPatternBuilder<Seq, pattern_t<LHS_T>,
                                                     pattern_t<RHS_T>>::type>
operator>>(LHS_T lhs, RHS_T rhs) {
  return detail::NaryPatternBuilder<Seq, pattern_t<LHS_T>, pattern_t<RHS_T>>::
      build(make_pattern(std::move(lhs)), make_pattern(std::move(rhs)));
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
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
template <typename PAT_T, typename ACT_T>
auto apply_action(PAT_T&& pat, ACT_T&& act) {
  return Action<std::decay_t<PAT_T>, std::decay_t<ACT_T>>(
      std::forward<PAT_T>(pat), std::forward<ACT_T>(act));
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename PAT_T>
class BindDst : public Pattern<BindDst<PAT_T>> {
  PAT_T child_;
 public:
  BindDst(PAT_T child) : child_(std::move(child)) {}
  PAT_T const& operand() const { return child_; }
};
template <typename PAT_T>
inline auto bind_dst(PAT_T pat) {
  return BindDst<pattern_t<PAT_T>>(make_pattern(std::move(pat)));
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CHILD_PAT_T, typename... ARGS_T>
class Construct : public Pattern<Construct<CHILD_PAT_T, ARGS_T...>> {
  CHILD_PAT_T pat_;
 public:
  Construct(CHILD_PAT_T pat) : pat_(std::move(pat)) {}
  CHILD_PAT_T const& child_pattern() const { return pat_; }
};
template <typename... ARGS_T, typename CHILD_PAT_T>
auto construct(CHILD_PAT_T pat) {
  return Construct<CHILD_PAT_T, ARGS_T...>(std::move(pat));
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename PAT_T>
class Lexeme : public Pattern<Lexeme<PAT_T>> {
  PAT_T child_;
 public:
  Lexeme(PAT_T child) : child_(std::move(child)) {}
  PAT_T const& operand() const { return child_; }
};
template <typename PAT_T>
inline auto lexeme(PAT_T pat) {
  return Lexeme<pattern_t<PAT_T>>(make_pattern(std::move(pat)));
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
template <typename PAT_T,
          typename Enable = enable_if_t<is_valid_unary_operand<PAT_T>(),
                                        Not<pattern_t<PAT_T>>>>
auto operator!(PAT_T&& pat) {
  return Not<pattern_t<PAT_T>>(make_pattern(pat));
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename PAT_T>
class Optional : public Pattern<Optional<PAT_T>> {
  PAT_T child_;
 public:
  Optional(PAT_T child) : child_(std::move(child)) {}
  PAT_T const& operand() const { return child_; }
};
template <typename PAT_T,
          typename Enable = enable_if_t<is_valid_unary_operand<PAT_T>()>>
auto operator-(PAT_T&& pat) {
  return Optional<pattern_t<PAT_T>>(make_pattern(pat));
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename PAT_T, std::size_t MIN_REP, std::size_t MAX_REP>
class Repeat : public Pattern<Repeat<PAT_T, MIN_REP, MAX_REP>> {
  PAT_T operand_;
 public:
  Repeat(const PAT_T op) : operand_(std::move(op)) {}
  PAT_T const& operand() const { return operand_; }
};
template <std::size_t MIN_REP = 0, std::size_t MAX_REP = 0, typename PAT_T>
inline auto repeat(PAT_T pat) {
  return Repeat<pattern_t<PAT_T>, MIN_REP, MAX_REP>(
      make_pattern(std::move(pat)));
}
template <typename PAT_T,
          typename Enable = enable_if_t<is_valid_unary_operand<PAT_T>()>>
auto operator*(PAT_T pat) {
  return repeat<0, 0>(std::move(pat));
}
template <typename PAT_T,
          typename Enable = enable_if_t<is_valid_unary_operand<PAT_T>()>>
auto operator+(PAT_T&& pat) {
  return repeat<1, 0>(forward<PAT_T>(pat));
}
}  // namespace ABULAFIA_NAMESPACE


namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, std::size_t BASE,
          std::size_t DIGITS_MIN, std::size_t DIGITS_MAX>
class IntImpl {
 public:
  using pat_t = Int<BASE, DIGITS_MIN, DIGITS_MAX>;
  using digit_vals = DigitValues<BASE>;
  IntImpl(CTX_T, DST_T dst, pat_t const&) { dst.get() = 0; }
  Result consume(CTX_T ctx, DST_T dst, pat_t const&) {
    while (true) {
      if (ctx.data().empty()) {
        if (ctx.data().final_buffer()) {
          return digit_count_ >= DIGITS_MIN ? Result::SUCCESS : Result::FAILURE;
        } else {
          return Result::PARTIAL;
        }
      }
      auto next = ctx.data().next();
      if (digit_count_ == 0 && look_for_sign_) {
        look_for_sign_ = false;
        if (next == '-') {
          ctx.data().advance();
          neg_ = true;
          continue;
        }
        if (next == '+') {
          ctx.data().advance();
          continue;
        }
      }
      if (digit_vals::is_valid(next)) {
        dst.get() *= BASE;
        dst.get() += digit_vals::value(next);
        ++digit_count_;
        ctx.data().advance();
        if (digit_count_ == DIGITS_MAX) {
          if (neg_) {
            dst.get() *= -1;
          }
          return Result::SUCCESS;
        }
      } else {
        if (neg_) {
          dst.get() *= -1;
        }
        return digit_count_ >= DIGITS_MIN ? Result::SUCCESS : Result::FAILURE;
      }
    }
  }
 private:
  std::size_t digit_count_ = 0;
  bool look_for_sign_ = true;
  bool neg_ = false;
};
template <std::size_t BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX>
struct ParserFactory<Int<BASE, DIGITS_MIN, DIGITS_MAX>> {
  using pat_t = Int<BASE, DIGITS_MIN, DIGITS_MAX>;
  static constexpr DstBehavior dst_behavior() { return DstBehavior::VALUE; }
  enum {
    // TODO: We could provide ATOMIC in the DIGITS_MIN == DIGITS_MAX case
    ATOMIC = false,
    FAILS_CLEANLY = DIGITS_MAX == 1,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = IntImpl<CTX_T, DST_T, BASE, DIGITS_MIN, DIGITS_MAX>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, std::size_t BASE,
          std::size_t DIGITS_MIN, std::size_t DIGITS_MAX>
class UIntImpl {
 public:
  using pat_t = UInt<BASE, DIGITS_MIN, DIGITS_MAX>;
  using digit_vals = DigitValues<BASE>;
  UIntImpl(CTX_T, DST_T dst, pat_t const&) { dst.get() = 0; }
  Result consume(CTX_T ctx, DST_T dst, pat_t const&) {
    while (true) {
      if (ctx.data().empty()) {
        if (ctx.data().final_buffer()) {
          return digit_count_ >= DIGITS_MIN ? Result::SUCCESS : Result::FAILURE;
        } else {
          return Result::PARTIAL;
        }
      }
      auto next = ctx.data().next();
      if (digit_vals::is_valid(next)) {
        dst.get() *= typename DST_T::dst_value_type(BASE);
        dst.get() += digit_vals::value(next);
        ++digit_count_;
        ctx.data().advance();
        if (digit_count_ == DIGITS_MAX) {
          return Result::SUCCESS;
        }
      } else {
        return digit_count_ >= DIGITS_MIN ? Result::SUCCESS : Result::FAILURE;
      }
    }
  }
 private:
  std::size_t digit_count_ = 0;
};
template <std::size_t BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX>
struct ParserFactory<UInt<BASE, DIGITS_MIN, DIGITS_MAX>> {
  using pat_t = UInt<BASE, DIGITS_MIN, DIGITS_MAX>;
  static constexpr DstBehavior dst_behavior() { return DstBehavior::VALUE; }
  enum {
    // TODO: We could provide ATOMIC in the DIGITS_MIN == DIGITS_MAX case
    ATOMIC = false,
    FAILS_CLEANLY = DIGITS_MAX == 1,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = UIntImpl<CTX_T, DST_T, BASE, DIGITS_MIN, DIGITS_MAX>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename CHAR_T, typename VAL_T>
class CharSymbolImpl {
 public:
  using pat_t = CharSymbol<CHAR_T, VAL_T>;
  CharSymbolImpl(CTX_T, DST_T, pat_t const&) {}
  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    if (ctx.data().empty()) {
      return ctx.data().final_buffer() ? Result::FAILURE : Result::PARTIAL;
    }
    auto next = ctx.data().next();
    auto found = pat.mapping().find(next);
    if (found == pat.mapping().end()) {
      return Result::FAILURE;
    }
    dst = found->second;
    return Result::SUCCESS;
  }
  Result peek(CTX_T ctx, pat_t const& pat) {
    if (ctx.data().empty()) {
      return ctx.data().final_buffer() ? Result::FAILURE : Result::PARTIAL;
    }
    auto next = ctx.data().next();
    auto found = pat.mapping().find(next);
    if (found == pat.mapping().end()) {
      return Result::FAILURE;
    }
    return Result::SUCCESS;
  }
};
template <typename CHAR_T, typename VAL_T>
struct ParserFactory<CharSymbol<CHAR_T, VAL_T>> {
  using pat_t = CharSymbol<CHAR_T, VAL_T>;
  static constexpr DstBehavior dst_behavior() { return DstBehavior::VALUE; }
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = CharSymbolImpl<CTX_T, DST_T, CHAR_T, VAL_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename CHARSET_T>
class CharImpl {
 public:
  using pat_t = Char<CHARSET_T>;
  CharImpl(CTX_T, DST_T, pat_t const&) {}
  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    if (ctx.data().empty()) {
      return ctx.data().final_buffer() ? Result::FAILURE : Result::PARTIAL;
    }
    auto next = ctx.data().next();
    if (pat.char_set().is_valid(next)) {
      dst = next;
      ctx.data().advance();
      return Result::SUCCESS;
    }
    return Result::FAILURE;
  }
};
template <typename CHARSET_T>
struct ParserFactory<Char<CHARSET_T>> {
  using pat_t = Char<CHARSET_T>;
  static constexpr DstBehavior dst_behavior() { return DstBehavior::VALUE; }
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = CharImpl<CTX_T, DST_T, CHARSET_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T>
class EoiImpl {
  using ctx_t = CTX_T;
  using dst_t = Nil;
  using pat_t = Fail;
 public:
  EoiImpl(CTX_T, Nil, Eoi const&) {}
  Result consume(CTX_T ctx, Nil, Eoi const& pat) { return peek(ctx, pat); }
  Result peek(CTX_T ctx, Eoi const&) {
    if (ctx.data().empty()) {
      return ctx.data().final_buffer() ? Result::SUCCESS : Result::PARTIAL;
    }
    return Result::FAILURE;
  }
};
template <>
struct ParserFactory<Eoi> {
  using pat_t = Eoi;
  static constexpr DstBehavior dst_behavior() { return DstBehavior::IGNORE; }
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = EoiImpl<CTX_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T>
class FailImpl {
  using ctx_t = CTX_T;
  using dst_t = Nil;
  using pat_t = Fail;
 public:
  FailImpl(CTX_T, Nil, Fail const&) {}
  Result consume(CTX_T, Nil, Fail const&) { return Result::FAILURE; }
  Result peek(CTX_T, Fail const&) { return Result::FAILURE; }
};
template <>
struct ParserFactory<Fail> {
  using pat_t = Fail;
  static constexpr DstBehavior dst_behavior() { return DstBehavior::IGNORE; }
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = FailImpl<CTX_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename REQ_T>
class PassImpl {
  static_assert(!REQ_T::CONSUMES_ON_SUCCESS);
  using ctx_t = CTX_T;
  using dst_t = Nil;
  using pat_t = Pass;
 public:
  PassImpl(CTX_T, Nil, Pass const&) {}
  Result consume(CTX_T, Nil, Pass const&) { return Result::SUCCESS; }
  Result peek(CTX_T, Pass const&) { return Result::SUCCESS; }
};
template <>
struct ParserFactory<Pass> {
  using pat_t = Pass;
  static constexpr DstBehavior dst_behavior() { return DstBehavior::IGNORE; }
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = PassImpl<CTX_T, REQ_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename CHAR_T>
class StringLiteralImpl {
  using PAT_T = StringLiteral<CHAR_T>;
  typename std::basic_string<CHAR_T>::const_iterator next_expected_;
 public:
  StringLiteralImpl(CTX_T, DST_T, PAT_T const& pat)
      : next_expected_(pat.begin()) {}
  Result consume(CTX_T ctx, DST_T, PAT_T const& pat) {
    while (1) {
      if (next_expected_ == pat.end()) {
        return Result::SUCCESS;
      }
      if (ctx.data().empty()) {
        return ctx.data().final_buffer() ? Result::FAILURE : Result::PARTIAL;
      }
      auto next = ctx.data().next();
      if (next == *next_expected_) {
        ctx.data().advance();
        ++next_expected_;
      } else {
        return Result::FAILURE;
      }
    }
  }
};
template <typename CHAR_T>
struct ParserFactory<StringLiteral<CHAR_T>> {
  static constexpr DstBehavior dst_behavior() { return DstBehavior::IGNORE; }
  using pat_t = StringLiteral<CHAR_T>;
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = false,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = StringLiteralImpl<CTX_T, DST_T, CHAR_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename CHAR_T, typename VAL_T>
class SymbolImpl {
  using pat_t = Symbol<CHAR_T, VAL_T>;
  using node_t = typename pat_t::node_t;
  node_t const* next_ = nullptr;
  node_t const* current_valid_ = nullptr;
 public:
  SymbolImpl(CTX_T, DST_T, pat_t const& pat) : next_(pat.root()) {}
  Result consume(CTX_T ctx, DST_T dst, pat_t const&) {
    while (1) {
      if (ctx.data().empty()) {
        if (ctx.data().final_buffer()) {
          if (current_valid_) {
            dst = *current_valid_->val;
            ctx.data().commit_rollback();
            return Result::SUCCESS;
          } else {
            return Result::FAILURE;
          }
        } else {
          // If we were conclusively done, we would have returned success
          // before looping.
          return Result::PARTIAL;
        }
      }
      auto next = ctx.data().next();
      auto found = next_->child.find(next);
      if (found == next_->child.end()) {
        // the next character leads nowhere
        if (current_valid_) {
          // we had a match along the way
          dst = *current_valid_->val;
          ctx.data().commit_rollback();
          return Result::SUCCESS;
        }
        return Result::FAILURE;
      } else {
        // consume the value
        ctx.data().advance();
        next_ = &found->second;
        if (next_->val) {
          // we got a hit!
          if (current_valid_) {
            ctx.data().cancel_rollback();
          }
          if (next_->child.empty()) {
            // nowhere to go from here
            dst = *next_->val;
            return Result::SUCCESS;
          }
          current_valid_ = next_;
          ctx.data().prepare_rollback();
        }
      }
    }
  }
};
template <typename CHAR_T, typename VAL_T>
struct ParserFactory<Symbol<CHAR_T, VAL_T>> {
  using pat_t = Symbol<CHAR_T, VAL_T>;
  static constexpr DstBehavior dst_behavior() { return DstBehavior::VALUE; }
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = false,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = SymbolImpl<CTX_T, DST_T, CHAR_T, VAL_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T, typename OP_T,
          typename NEG_T>
class ExceptImpl {
  using pat_t = Except<OP_T, NEG_T>;
  struct op_req_t : public REQ_T {
    enum {
      // We don't care
      FAILS_CLEANLY = false,
    };
  };
  struct neg_req_t {
    enum {
      ATOMIC = false,
      FAILS_CLEANLY = true,
      CONSUMES_ON_SUCCESS = false,
    };
  };
  using op_parser_t = Parser<CTX_T, DST_T, op_req_t, OP_T>;
  using neg_parser_t = Parser<CTX_T, Nil, neg_req_t, NEG_T>;
  using child_parsers_t = std::variant<neg_parser_t, op_parser_t>;
  child_parsers_t child_parsers_;
 public:
  ExceptImpl(CTX_T ctx, DST_T, pat_t const& pat)
      : child_parsers_(std::in_place_index_t<0>(), ctx, nil, pat.neg()) {}
  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    if (child_parsers_.index() == 0) {
      auto res = std::get<0>(child_parsers_).consume(ctx, nil, pat.neg());
      switch (res) {
        case Result::PARTIAL:
          return Result::PARTIAL;
        case Result::SUCCESS:
          return Result::FAILURE;
        case Result::FAILURE:
          child_parsers_ =
              child_parsers_t(std::in_place_index_t<1>(), ctx, dst, pat.op());
      }
    }
    abu_assume(child_parsers_.index() == 1);
    return std::get<1>(child_parsers_).consume(ctx, dst, pat.op());
  }
};
template <typename OP_T, typename NEG_T>
struct ParserFactory<Except<OP_T, NEG_T>> {
  using pat_t = Except<OP_T, NEG_T>;
  static constexpr DstBehavior dst_behavior() {
    return ParserFactory<OP_T>::dst_behavior();
  }
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = false,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = ExceptImpl<CTX_T, DST_T, REQ_T, OP_T, NEG_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T, typename OP_T,
          typename SEP_T>
class ListImpl {
  using pat_t = List<OP_T, SEP_T>;
  struct op_req_t : public DefaultReqs {
    enum {
      ATOMIC = true,
      // We don't care
      FAILS_CLEANLY = false,
    };
  };
  struct sep_req_t : public DefaultReqs {
    enum {
      ATOMIC = false,
      FAILS_CLEANLY = false,
    };
  };
  using op_parser_t = Parser<CTX_T, DST_T, op_req_t, OP_T>;
  using sep_parser_t = Parser<CTX_T, Nil, sep_req_t, SEP_T>;
  using child_parsers_t = std::variant<op_parser_t, sep_parser_t>;
  child_parsers_t child_parsers_;
 public:
  ListImpl(CTX_T ctx, DST_T dst, pat_t const& pat)
      : child_parsers_(std::in_place_index_t<0>(), ctx, dst, pat.op()) {
    ctx.data().prepare_rollback();
  }
  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    while (1) {
      if (child_parsers_.index() == 0) {
        // We are parsing a value.
        auto child_res =
            std::get<0>(child_parsers_).consume(ctx, dst, pat.op());
        switch (child_res) {
          case Result::SUCCESS: {
            ctx.data().cancel_rollback();
            ctx.data().prepare_rollback();
            child_parsers_ = child_parsers_t(std::in_place_index_t<1>(), ctx,
                                             nil, pat.sep());
          } break;
          case Result::FAILURE:
            // this will cancel the consumption of the separator if there was
            // any
            ctx.data().commit_rollback();
            return Result::SUCCESS;
          case Result::PARTIAL:
            return Result::PARTIAL;
        }
      } else {
        abu_assume(child_parsers_.index() == 1);
        // We are parsing a separator
        auto child_res =
            std::get<1>(child_parsers_).consume(ctx, nil, pat.sep());
        switch (child_res) {
          case Result::SUCCESS:
            child_parsers_ =
                child_parsers_t(std::in_place_index_t<0>(), ctx, dst, pat.op());
            break;
          case Result::FAILURE:
            // rollback whatever the separator may have eaten
            ctx.data().commit_rollback();
            return Result::SUCCESS;
          case Result::PARTIAL:
            return Result::PARTIAL;
        }
      }
    }
  }
};
template <typename OP_T, typename SEP_T>
struct ParserFactory<List<OP_T, SEP_T>> {
  using pat_t = List<OP_T, SEP_T>;
  static constexpr DstBehavior dst_behavior() {
    return ParserFactory<OP_T>::dst_behavior();
  }
  enum {
    ATOMIC = false,
    FAILS_CLEANLY = true,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = ListImpl<CTX_T, DST_T, REQ_T, OP_T, SEP_T>;
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
template <typename CTX_T, typename DST_T, typename REQ_T,
          typename... CHILD_PATS_T>
class AltImpl {
  using pat_t = Alt<CHILD_PATS_T...>;
  struct child_req_t : public REQ_T {
    enum { FAILS_CLEANLY = true };
  };
  using child_parsers_t =
      std::variant<Parser<CTX_T, DST_T, child_req_t, CHILD_PATS_T>...>;
  child_parsers_t child_parsers_;
 public:
  AltImpl(CTX_T ctx, DST_T dst, pat_t const& pat)
      : child_parsers_(std::in_place_index_t<0>(), ctx, dst, getChild<0>(pat)) {
  }
  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    if (ctx.data().isResumable()) {
      return visit_val<sizeof...(CHILD_PATS_T)>(
          child_parsers_.index(),
          [&](auto N) { return this->consume_from<N()>(ctx, dst, pat); });
    } else {
      // Skip the visitation when using non_resumable parsers.
      return consume_from<0>(ctx, dst, pat);
    }
  }
  template <std::size_t ID>
  Result consume_from(CTX_T ctx, DST_T dst, pat_t const& pat) {
    abu_assume(child_parsers_.index() == ID);
    auto& c_parser = std::get<ID>(child_parsers_);
    auto const& c_pattern = getChild<ID>(pat);
    Result child_res = c_parser.consume(ctx, dst, c_pattern);
    if (Result::FAILURE == child_res) {
      constexpr int next_id = ID + 1;
      // if we have reached the end of the child parsers list
      if (sizeof...(CHILD_PATS_T) == next_id) {
        return Result::FAILURE;
      } else {
        constexpr int new_id = next_id < sizeof...(CHILD_PATS_T) ? next_id : 0;
        auto const& new_c_pattern = getChild<new_id>(pat);
        child_parsers_ = child_parsers_t(std::in_place_index_t<new_id>(), ctx,
                                         dst, new_c_pattern);
        return consume_from<new_id>(ctx, dst, pat);
      }
    }
    return child_res;
  }
};
template <typename... CHILD_PATS_T>
struct ParserFactory<Alt<CHILD_PATS_T...>> {
  using pat_t = Alt<CHILD_PATS_T...>;
  static constexpr DstBehavior dst_behavior() { return DstBehavior::VALUE; }
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = AltImpl<CTX_T, DST_T, REQ_T, CHILD_PATS_T...>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
namespace seq_ {
template <int PAT_ID, typename CHILDS_TUPLE_T>
constexpr bool child_ignores() {
  using pattern_t = tuple_element_t<PAT_ID == -1 ? 0 : PAT_ID, CHILDS_TUPLE_T>;
  using pattern_factory_t = ParserFactory<pattern_t>;
  return pattern_factory_t::dst_behavior() == DstBehavior::IGNORE;
}
template <int PAT_ID, typename CHILDS_TUPLE_T, typename Enable = void>
struct choose_tuple_index;
template <typename CHILDS_TUPLE_T>
struct choose_tuple_index<-1, CHILDS_TUPLE_T> {
  enum { value = -1, next_val = 0 };
};
template <int PAT_ID, typename CHILDS_TUPLE_T>
struct choose_tuple_index<
    PAT_ID, CHILDS_TUPLE_T,
    enable_if_t<(PAT_ID != -1 && child_ignores<PAT_ID, CHILDS_TUPLE_T>())>> {
  enum {
    value = -1,
    next_val = choose_tuple_index<PAT_ID - 1, CHILDS_TUPLE_T>::next_val
  };
};
template <int PAT_ID, typename CHILDS_TUPLE_T>
struct choose_tuple_index<
    PAT_ID, CHILDS_TUPLE_T,
    enable_if_t<(PAT_ID != -1 && !child_ignores<PAT_ID, CHILDS_TUPLE_T>())>> {
  enum {
    value = choose_tuple_index<PAT_ID - 1, CHILDS_TUPLE_T>::next_val,
    next_val = value + 1
  };
};
enum class DstAccessorCategory { USE_NIL, PASSTHROUGH, INDEXED };
template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
constexpr DstAccessorCategory ChooseAccessorCategory() {
  using pattern_t = tuple_element_t<PAT_ID, CHILDS_TUPLE_T>;
  using pattern_factory_t = ParserFactory<pattern_t>;
  if (std::is_same<Nil, DST_T>::value ||
      pattern_factory_t::dst_behavior() == DstBehavior::IGNORE) {
    return DstAccessorCategory::USE_NIL;
  } else if (is_tuple<typename DST_T::dst_type>::value) {
    return DstAccessorCategory::INDEXED;
  } else {
    return DstAccessorCategory::PASSTHROUGH;
  }
}
template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T, typename Enable = void>
struct choose_dst_accessor;
template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
struct choose_dst_accessor<
    PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T,
    enable_if_t<
        ChooseAccessorCategory<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>() ==
        DstAccessorCategory::USE_NIL>> {
  using type = Nil;
  static Nil access(DST_T const&) { return nil; }
};
template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
struct choose_dst_accessor<
    PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T,
    enable_if_t<
        ChooseAccessorCategory<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>() ==
        DstAccessorCategory::PASSTHROUGH>> {
  using type = DST_T;
  static DST_T const& access(DST_T const& dst) { return dst; }
};
template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
struct choose_dst_accessor<
    PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T,
    enable_if_t<
        ChooseAccessorCategory<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>() ==
        DstAccessorCategory::INDEXED>> {
  enum { dst_index = choose_tuple_index<PAT_ID, CHILDS_TUPLE_T>::value };
  using type =
      wrapped_dst_t<tuple_element_t<dst_index, typename DST_T::dst_type>>;
  static auto access(DST_T dst) {
    return wrap_dst(std::get<dst_index>(dst.get()));
  }
};
template <std::size_t PAT_ID, typename CTX_T, typename DST_T, typename REQ_T,
          typename CHILDS_TUPLE_T>
struct WrappedParser {
  using dst_t =
      typename choose_dst_accessor<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>::type;
  using type =
      Parser<CTX_T, dst_t, REQ_T, tuple_element_t<PAT_ID, CHILDS_TUPLE_T>>;
};
template <std::size_t PAT_ID, typename CTX_T, typename DST_T, typename REQ_T,
          typename CHILDS_TUPLE_T>
using WrappedParser_t =
    typename WrappedParser<PAT_ID, CTX_T, DST_T, REQ_T, CHILDS_TUPLE_T>::type;
template <typename CTX_T, typename DST_T, typename REQ_T,
          typename CHILDS_TUPLE_T, typename INDEX_SEQ>
struct SeqSubParser;
template <typename CTX_T, typename DST_T, typename REQ_T,
          typename CHILDS_TUPLE_T, std::size_t... PAT_IDS>
struct SeqSubParser<CTX_T, DST_T, REQ_T, CHILDS_TUPLE_T,
                    std::index_sequence<PAT_IDS...>> {
  using test_test = std::index_sequence<PAT_IDS...>;
  using type = std::variant<
      WrappedParser_t<PAT_IDS, CTX_T, DST_T, REQ_T, CHILDS_TUPLE_T>...>;
};
}  // namespace seq_
template <typename CTX_T, typename DST_T, typename REQ_T,
          typename... CHILD_PATS_T>
class SeqImpl {
  using pat_t = Seq<CHILD_PATS_T...>;
  struct child_req_t : public REQ_T {
    enum { CONSUMES_ON_SUCCESS = false, ATOMIC = false, FAILS_CLEANLY = false };
  };
  using childs_tuple_t = typename pat_t::child_tuple_t;
  using child_parsers_t = typename seq_::SeqSubParser<
      CTX_T, DST_T, child_req_t, childs_tuple_t,
      std::index_sequence_for<CHILD_PATS_T...>>::type;
  child_parsers_t child_parsers_;
 public:
  template <std::size_t ID>
  decltype(auto) getDstFor(DST_T dst) {
    using accessor_t =
        seq_::choose_dst_accessor<ID, CTX_T, DST_T, childs_tuple_t>;
    return accessor_t::access(dst);
  }
  SeqImpl(CTX_T ctx, DST_T dst, pat_t const& pat)
      : child_parsers_(std::in_place_index_t<0>(),
                       std::variant_alternative_t<0, child_parsers_t>(
                           ctx, getDstFor<0>(dst), getChild<0>(pat))) {
    //    reset_if_collection<DST_T>::exec(dst);
  }
  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
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
  Result consume_from(CTX_T ctx, DST_T dst, pat_t const& pat) {
    abu_assume(child_parsers_.index() == ID);
    auto& c_parser = std::get<ID>(child_parsers_);
    auto const& c_pattern = getChild<ID>(pat);
    Result child_res = c_parser.consume(ctx, getDstFor<ID>(dst), c_pattern);
    if (Result::SUCCESS == child_res) {
      constexpr int next_id = ID + 1;
      // if we have reached the end of the child parsers list
      if (sizeof...(CHILD_PATS_T) == next_id) {
        return Result::SUCCESS;
      } else {
        // This does not matter nearly that much, as we will never enter
        // here with a saturated value
        // The sole purpose of the max is to prevent reset<N>() from
        // being called with an out of bounds value,
        // which would cause a compile-time error.
        constexpr int new_id = next_id < sizeof...(CHILD_PATS_T) ? next_id : 0;
        auto const& new_c_pattern = getChild<new_id>(pat);
        child_parsers_ = child_parsers_t(std::in_place_index_t<new_id>(), ctx,
                                         getDstFor<new_id>(dst), new_c_pattern);
        return consume_from<new_id>(ctx, dst, pat);
      }
    }
    return child_res;
  }
};
template <typename... CHILD_PATS_T>
struct ParserFactory<Seq<CHILD_PATS_T...>> {
  using pat_t = Seq<CHILD_PATS_T...>;
  static constexpr DstBehavior dst_behavior() { return DstBehavior::VALUE; }
  enum {
    ATOMIC = false,
    FAILS_CLEANLY = false,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = SeqImpl<CTX_T, DST_T, REQ_T, CHILD_PATS_T...>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
namespace act_ {
struct arbitrary {
  template <class T>
  operator T();
};
template <typename BOUND_DST_T>
struct ActionParam {
  BOUND_DST_T bound_dst;
};
template <typename ACT_T, typename DST_T = Nil>
constexpr bool has_incoming_val() {
  return (std::is_invocable<ACT_T, arbitrary>::value &&
          !std::is_invocable<ACT_T, ActionParam<DST_T>>::value) ||
         std::is_invocable<ACT_T, arbitrary, ActionParam<DST_T>>::value;
}
template <typename ACT_T, typename DST_T = Nil>
constexpr bool has_incoming_param() {
  return std::is_invocable<ACT_T, ActionParam<DST_T>>::value ||
         std::is_invocable<ACT_T, arbitrary, ActionParam<DST_T>>::value;
}
/*Used primarily to determine if the return type is void or not*/
template <typename ACT_T, typename Enable = void>
struct determine_result_type {
  using type = typename function_traits<ACT_T>::result_type;
};
template <typename ACT_T>
struct determine_result_type<ACT_T,
                             std::enable_if_t<has_incoming_param<ACT_T>() &&
                                              has_incoming_val<ACT_T>()>> {
  using type = decltype(std::declval<ACT_T>()(arbitrary{}, ActionParam<Nil>()));
};
template <typename ACT_T>
struct determine_result_type<ACT_T,
                             std::enable_if_t<has_incoming_param<ACT_T>() &&
                                              !has_incoming_val<ACT_T>()>> {
  using type = decltype(std::declval<ACT_T>()(ActionParam<Nil>()));
};
template <typename ACT_T>
constexpr bool returns_void() {
  return std::is_same<typename determine_result_type<ACT_T>::type, void>::value;
}
template <typename ACT_T, typename DST_T, typename Enable = void>
struct determine_landing_type {
  using type = Nil;
};
template <typename ACT_T, typename DST_T>
struct determine_landing_type<
    ACT_T, DST_T,
    std::enable_if_t<has_incoming_val<ACT_T, DST_T>() &&
                     !has_incoming_param<ACT_T, DST_T>()>> {
  using type = typename function_traits<ACT_T>::template arg<0>::type;
};
template <typename ACT_T, typename DST_T>
struct determine_landing_type<
    ACT_T, DST_T,
    std::enable_if_t<has_incoming_val<ACT_T, DST_T>() &&
                     has_incoming_param<ACT_T, DST_T>()>> {
  using oper = decltype(&ACT_T::template operator()<ActionParam<DST_T>>);
  using type = typename function_traits<oper>::template arg<0>::type;
};
template <typename ACT_T, typename Enable = void>
struct Dispatch;
template <typename ACT_T>
struct Dispatch<
    ACT_T, enable_if_t<!returns_void<ACT_T>() && !has_incoming_val<ACT_T>() &&
                       !has_incoming_param<ACT_T>()>> {
  template <typename LAND_T, typename DST_T, typename CTX_T>
  static void dispatch(ACT_T const& act, LAND_T, DST_T dst, CTX_T) {
    dst = act();
  }
};
template <typename ACT_T>
struct Dispatch<
    ACT_T, enable_if_t<returns_void<ACT_T>() && !has_incoming_val<ACT_T>() &&
                       !has_incoming_param<ACT_T>()>> {
  template <typename LAND_T, typename DST_T, typename CTX_T>
  static void dispatch(ACT_T const& act, LAND_T, DST_T, CTX_T) {
    act();
  }
};
template <typename ACT_T>
struct Dispatch<
    ACT_T, enable_if_t<!returns_void<ACT_T>() && has_incoming_val<ACT_T>() &&
                       !has_incoming_param<ACT_T>()>> {
  template <typename LAND_T, typename DST_T, typename CTX_T>
  static void dispatch(ACT_T const& act, LAND_T land, DST_T dst, CTX_T) {
    dst = act(std::move(land));
  }
};
template <typename ACT_T>
struct Dispatch<
    ACT_T, enable_if_t<returns_void<ACT_T>() && has_incoming_val<ACT_T>() &&
                       !has_incoming_param<ACT_T>()>> {
  template <typename LAND_T, typename DST_T, typename CTX_T>
  static void dispatch(ACT_T const& act, LAND_T land, DST_T, CTX_T) {
    act(std::move(land));
  }
};
template <typename ACT_T>
struct Dispatch<
    ACT_T, enable_if_t<!returns_void<ACT_T>() && !has_incoming_val<ACT_T>() &&
                       has_incoming_param<ACT_T>()>> {
  template <typename LAND_T, typename DST_T, typename CTX_T>
  static void dispatch(ACT_T const& act, LAND_T, DST_T dst, CTX_T ctx) {
    ActionParam<typename CTX_T::bound_dst_t> p{ctx.bound_dst()};
    dst = act(p);
  }
};
template <typename ACT_T>
struct Dispatch<
    ACT_T, enable_if_t<returns_void<ACT_T>() && !has_incoming_val<ACT_T>() &&
                       has_incoming_param<ACT_T>()>> {
  template <typename LAND_T, typename DST_T, typename CTX_T>
  static void dispatch(ACT_T const& act, LAND_T, DST_T dst, CTX_T ctx) {
    ActionParam<typename CTX_T::bound_dst_t> p{ctx.bound_dst()};
    act(p);
  }
};
template <typename ACT_T>
struct Dispatch<
    ACT_T, enable_if_t<!returns_void<ACT_T>() && has_incoming_val<ACT_T>() &&
                       has_incoming_param<ACT_T>()>> {
  template <typename LAND_T, typename DST_T, typename CTX_T>
  static void dispatch(ACT_T const& act, LAND_T land, DST_T dst, CTX_T ctx) {
    ActionParam<typename CTX_T::bound_dst_t> p{ctx.bound_dst()};
    dst = act(std::move(land), p);
  }
};
template <typename ACT_T>
struct Dispatch<
    ACT_T, enable_if_t<returns_void<ACT_T>() && has_incoming_val<ACT_T>() &&
                       has_incoming_param<ACT_T>()>> {
  template <typename LAND_T, typename DST_T, typename CTX_T>
  static void dispatch(ACT_T const& act, LAND_T land, DST_T, CTX_T ctx) {
    ActionParam<typename CTX_T::bound_dst_t> p{ctx.bound_dst()};
    act(std::move(land), p);
  }
};
}  // namespace act_
template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T,
          typename ACT_T>
class ActionImpl {
  using PAT_T = Action<CHILD_PAT_T, ACT_T>;
  struct child_req_t {
    enum {
      ATOMIC = false,
      // This is extremely important, since we can succeed even if the child
      // parser fails. The exception to this is if MIN_REP == MAX_REP (except
      // for 0). In which case, failure of the child guarantees failure of the
      // parent.
      FAILS_CLEANLY = REQ_T::FAILS_CLEANLY,
      // Propagate
      CONSUMES_ON_SUCCESS = REQ_T::CONSUMES_ON_SUCCESS
    };
  };
  using child_ctx_t = CTX_T;
  using landing_type_t =
      typename act_::determine_landing_type<ACT_T,
                                            typename CTX_T::bound_dst_t>::type;
  using child_parser_t = Parser<child_ctx_t, wrapped_dst_t<landing_type_t>,
                                child_req_t, CHILD_PAT_T>;
  landing_type_t landing;
  child_parser_t child_parser_;
 public:
  ActionImpl(CTX_T ctx, DST_T, PAT_T const& pat)
      : child_parser_(ctx, wrap_dst(landing), pat.child_pattern()) {}
  Result consume(CTX_T ctx, DST_T dst, PAT_T const& pat) {
    auto status =
        child_parser_.consume(ctx, wrap_dst(landing), pat.child_pattern());
    if (status == Result::SUCCESS) {
      act_::Dispatch<ACT_T>::template dispatch(pat.action(), std::move(landing),
                                               dst, ctx);
    }
    return status;
  }
};
template <typename CHILD_PAT_T, typename ACT_T>
struct ParserFactory<Action<CHILD_PAT_T, ACT_T>> {
  using pat_t = Action<CHILD_PAT_T, ACT_T>;
  static constexpr DstBehavior dst_behavior() {
    return act_::returns_void<ACT_T>() ? DstBehavior::IGNORE
                                       : DstBehavior::VALUE;
  }
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = ActionImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T, ACT_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T>
class BindDstImpl {
  using pat_t = BindDst<CHILD_PAT_T>;
  using sub_ctx_t = typename CTX_T::template bind_dst<DST_T>;
  struct child_req_t : public REQ_T {
    enum {
      ATOMIC = false,
    };
  };
  using child_parser_t = Parser<sub_ctx_t, DST_T, child_req_t, CHILD_PAT_T>;
  child_parser_t parser_;
 public:
  BindDstImpl(CTX_T ctx, DST_T dst, pat_t const& pat)
      : parser_(sub_ctx_t(ctx.data(), ctx.skipper(), dst), dst, pat.operand()) {
  }
  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    return parser_.consume(sub_ctx_t(ctx.data(), ctx.skipper(), dst), dst,
                           pat.operand());
  }
};
template <typename CHILD_PAT_T>
struct ParserFactory<BindDst<CHILD_PAT_T>> {
  using pat_t = BindDst<CHILD_PAT_T>;
  static constexpr DstBehavior dst_behavior() {
    static_assert(
        ParserFactory<CHILD_PAT_T>::dst_behavior() != DstBehavior::IGNORE,
        "Why are we binding the dst for a Nil pattern?");
    return ParserFactory<CHILD_PAT_T>::dst_behavior();
  }
  enum {
    ATOMIC = false,
    FAILS_CLEANLY = true,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = BindDstImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T,
          typename... ARGS_T>
class ConstructImpl {
  using pat_t = Construct<CHILD_PAT_T, ARGS_T...>;
  using buffer_t = std::tuple<ARGS_T...>;
  using child_dst_t = typename SelectDstWrapper<buffer_t>::type;
  struct childs_reqs_t : public REQ_T {
    enum { ATOMIC = false };
  };
  using child_parser_t = Parser<CTX_T, child_dst_t, childs_reqs_t, CHILD_PAT_T>;
  buffer_t buffer_;
  child_parser_t parser_;
 public:
  ConstructImpl(CTX_T ctx, DST_T, pat_t const& pat)
      : parser_(ctx, child_dst_t(buffer_), pat.child_pattern()) {}
  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    auto status =
        parser_.consume(ctx, child_dst_t(buffer_), pat.child_pattern());
    if (status == Result::SUCCESS) {
      dst = std::make_from_tuple<typename DST_T::dst_type>(buffer_);
    }
    return status;
  }
};
template <typename CHILD_PAT_T, typename... ARGS_T>
struct ParserFactory<Construct<CHILD_PAT_T, ARGS_T...>> {
  using pat_t = Construct<CHILD_PAT_T, ARGS_T...>;
  static constexpr DstBehavior dst_behavior() { return DstBehavior::VALUE; }
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = ConstructImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T, ARGS_T...>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T>
class DiscardImpl {
  using ctx_t = CTX_T;
  using dst_t = DST_T;
  using req_t = REQ_T;
  using pat_t = Discard<CHILD_PAT_T>;
  using child_parser_t = Parser<ctx_t, Nil, req_t, CHILD_PAT_T>;
  child_parser_t child_parser_;
 public:
  DiscardImpl(ctx_t ctx, dst_t, pat_t const& pat)
      : child_parser_(ctx, nil, pat.operand()) {}
  Result consume(ctx_t ctx, dst_t, pat_t const& pat) {
    return child_parser_.consume(ctx, nil, pat.operand());
  }
};
template <typename CHILD_PAT_T>
struct ParserFactory<Discard<CHILD_PAT_T>> {
  using pat_t = Discard<CHILD_PAT_T>;
  static constexpr DstBehavior dst_behavior() { return DstBehavior::IGNORE; }
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = DiscardImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T>
class LexemeImpl {
  using pat_t = Lexeme<CHILD_PAT_T>;
  using sub_ctx_t = typename CTX_T::template set_skipper_t<Fail>;
  using child_parser_t = Parser<sub_ctx_t, DST_T, REQ_T, CHILD_PAT_T>;
  child_parser_t parser_;
 public:
  LexemeImpl(CTX_T ctx, DST_T dst, pat_t const& pat)
      : parser_(sub_ctx_t(ctx.data(), fail), dst, pat.operand()) {}
  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    return parser_.consume(sub_ctx_t(ctx.data(), fail), dst, pat.operand());
  }
  Result peek(CTX_T ctx, pat_t const& pat) {
    return parser_.peek(sub_ctx_t(ctx.data(), fail), pat.operand());
  }
};
template <typename CHILD_PAT_T>
struct ParserFactory<Lexeme<CHILD_PAT_T>> {
  using pat_t = Lexeme<CHILD_PAT_T>;
  static constexpr DstBehavior dst_behavior() {
    return ParserFactory<CHILD_PAT_T>::CHILD_PAT_T::dst_behavior();
  }
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = LexemeImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T>
class NotImpl {
  using pat_t = Not<CHILD_PAT_T>;
  struct child_req_t : public REQ_T {
    enum {
      ATOMIC = false,
      FAILS_CLEANLY = false,
    };
  };
  using child_parser_t = Parser<CTX_T, DST_T, child_req_t, CHILD_PAT_T>;
  child_parser_t parser_;
 public:
  NotImpl(CTX_T ctx, DST_T dst, pat_t const& pat)
      : parser_(ctx, dst, pat.operand()) {
    ctx.data().prepare_rollback();
  }
  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    auto status = parser_.consume(ctx, dst, pat.operand());
    switch (status) {
      case Result::SUCCESS:
        // just commit the rollback anyways, this allows us to promise
        // FAILS_CLEANLY
        ctx.data().commit_rollback();
        return Result::FAILURE;
      case Result::FAILURE:
        ctx.data().commit_rollback();
        return Result::SUCCESS;
      case Result::PARTIAL:
        return Result::PARTIAL;
    }
    abu_unreachable();
  }
  Result peek(CTX_T ctx, pat_t const& pat) {
    auto status = parser_.peek(ctx, pat.operand());
    switch (status) {
      case Result::SUCCESS:
        return Result::FAILURE;
      case Result::FAILURE:
        return Result::SUCCESS;
      case Result::PARTIAL:
        return Result::PARTIAL;
    }
    abu_unreachable();
  }
};
template <typename CHILD_PAT_T>
struct ParserFactory<Not<CHILD_PAT_T>> {
  using pat_t = Not<CHILD_PAT_T>;
  static constexpr DstBehavior dst_behavior() { return DstBehavior::IGNORE; }
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = NotImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T>
class OptImpl {
  using pat_t = Optional<CHILD_PAT_T>;
  struct child_req_t {
    enum {
      // The aternative is to push_back on start, and pop_back on failure,
      // which gets a little messy.
      ATOMIC = true,
      // This is extremely important, since we can succeed even if the child
      // parser fails.
      // The exception to this is if MIN_REP == MAX_REP (except for 0). In which
      // case, failure
      // of the child guarantees failure of the parent.
      FAILS_CLEANLY = false,
      // Propagate
      CONSUMES_ON_SUCCESS = REQ_T::CONSUMES_ON_SUCCESS
    };
  };
  using child_parser_t = Parser<CTX_T, DST_T, child_req_t, CHILD_PAT_T>;
  child_parser_t parser_;
 public:
  OptImpl(CTX_T ctx, DST_T dst, pat_t const& pat)
      : parser_(ctx, dst, pat.operand()) {
    ctx.data().prepare_rollback();
  }
  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    auto status = parser_.consume(ctx, dst, pat.operand());
    switch (status) {
      case Result::SUCCESS:
        return Result::SUCCESS;
      case Result::FAILURE:
        ctx.data().commit_rollback();
        return Result::SUCCESS;
      case Result::PARTIAL:
        return Result::PARTIAL;
    }
    abu_unreachable();
  }
  Result peek(CTX_T ctx, pat_t const& pat) { return Result::SUCCESS; }
};
template <typename CHILD_PAT_T>
struct ParserFactory<Optional<CHILD_PAT_T>> {
  using pat_t = Optional<CHILD_PAT_T>;
  static constexpr DstBehavior dst_behavior() {
    return ParserFactory<CHILD_PAT_T>::dst_behavior();
  }
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = OptImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T,
          std::size_t MIN_REP, std::size_t MAX_REP>
class RepeatImpl {
  static_assert(!REQ_T::CONSUMES_ON_SUCCESS || MIN_REP > 0);
  using ctx_t = CTX_T;
  using dst_t = DST_T;
  using req_t = REQ_T;
  using pat_t = Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>;
  struct child_req_t {
    enum {
      // The aternative is to push_back on start, and pop_back on failure,
      // which gets a little messy.
      ATOMIC = true,
      // This is extremely important, since we can succeed even if the child
      // parser fails.
      // The exception to this is if MIN_REP == MAX_REP (except for 0). In which
      // case, failure
      // of the child guarantees failure of the parent.
      FAILS_CLEANLY = MIN_REP != MAX_REP || MAX_REP == 0,
      // Propagate
      CONSUMES_ON_SUCCESS = REQ_T::CONSUMES_ON_SUCCESS
    };
  };
  using child_dst_t = dst_t;
  using child_parser_t = Parser<ctx_t, child_dst_t, child_req_t, CHILD_PAT_T>;
  std::size_t count_ = 0;
  child_parser_t child_parser_;
 public:
  RepeatImpl(ctx_t ctx, dst_t dst, pat_t const& pat)
      : child_parser_(ctx, child_dst_t(dst), pat.operand()) {}
  Result consume(ctx_t ctx, dst_t dst, pat_t const& pat) {
    while (1) {
      auto child_res =
          child_parser_.consume(ctx, child_dst_t(dst), pat.operand());
      switch (child_res) {
        case Result::FAILURE:
          if (count_ >= MIN_REP) {
            return Result::SUCCESS;
          } else {
            return Result::FAILURE;
          }
        case Result::PARTIAL:
          return Result::PARTIAL;
        case Result::SUCCESS:
          count_++;
          if (MAX_REP != 0 && count_ == MAX_REP) {
            return Result::SUCCESS;
          }
          // If we are still going, then we need to reset the child's parser
          child_parser_ = child_parser_t(ctx, dst, pat.operand());
      }
    }
  }
};
template <typename CHILD_PAT_T, std::size_t MIN_REP, std::size_t MAX_REP>
struct ParserFactory<Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>> {
  using pat_t = Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>;
  static constexpr DstBehavior dst_behavior() {
    return ParserFactory<CHILD_PAT_T>::dst_behavior();
  }
  enum {
    ATOMIC = false,
    FAILS_CLEANLY = false,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = RepeatImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T, MIN_REP, MAX_REP>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
struct RecurChildReqs {
  enum { ATOMIC = false, FAILS_CLEANLY = false, CONSUMES_ON_SUCCESS = false };
};
template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T,
          typename ATTR_T>
class RecurImpl {
  using pat_t = Recur<CHILD_PAT_T, ATTR_T>;
  using operand_pat_t = typename pat_t::operand_pat_t::impl_t;
  using operand_parser_t = Parser<CTX_T, DST_T, RecurChildReqs, operand_pat_t>;
  std::unique_ptr<operand_parser_t> child_parser_;
 public:
  RecurImpl(CTX_T, DST_T, pat_t const&) {
    // We do not create the child parser here, since this is a recursive
    // process.
  }
  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    if (!child_parser_) {
      child_parser_ =
          std::make_unique<operand_parser_t>(ctx, dst, pat.operand().impl);
    }
    return child_parser_->consume(ctx, dst, pat.operand().impl);
  }
};
template <typename CHILD_PAT_T, typename ATTR_T>
struct ParserFactory<Recur<CHILD_PAT_T, ATTR_T>> {
  static_assert(is_pattern<typename CHILD_PAT_T::impl_t>());
  using pat_t = Recur<CHILD_PAT_T, ATTR_T>;
  static constexpr DstBehavior dst_behavior() {
    if (std::is_same<Nil, ATTR_T>::value) {
      return DstBehavior::IGNORE;
    }
    return DstBehavior::VALUE;
  }
  enum {
    ATOMIC = false,
    FAILS_CLEANLY = false,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = RecurImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T, ATTR_T>;
};
template <typename CTX_T, typename DST_T, typename PAT_T>
struct AdaptedParserFactory<CTX_T, DST_T, RecurChildReqs, PAT_T> {
  static RecurParserChildImpl<CTX_T, DST_T, RecurChildReqs, PAT_T> create(
      CTX_T ctx, DST_T dst, PAT_T const& pat) {
    return RecurParserChildImpl<CTX_T, DST_T, RecurChildReqs, PAT_T>(ctx, dst,
                                                                     pat);
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T,
          typename SKIP_T>
class WithSkipperImpl {
  using ctx_t = CTX_T;
  using dst_t = DST_T;
  using pat_t = WithSkipper<CHILD_PAT_T, SKIP_T>;
  using sub_ctx_t = typename ctx_t::template set_skipper_t<SKIP_T>;
  Parser<sub_ctx_t, dst_t, REQ_T, CHILD_PAT_T> child_parser_;
 public:
  WithSkipperImpl(ctx_t ctx, dst_t dst, pat_t const& pat)
      : child_parser_(sub_ctx_t(ctx.data(), pat.getSkip(), ctx.bound_dst()),
                      dst, pat.getChild()) {}
  Result consume(ctx_t ctx, dst_t dst, pat_t const& pat) {
    return child_parser_.consume(
        sub_ctx_t(ctx.data(), pat.getSkip(), ctx.bound_dst()), dst,
        pat.getChild());
  }
};
template <typename CHILD_PAT_T, typename SKIP_T>
struct ParserFactory<WithSkipper<CHILD_PAT_T, SKIP_T>> {
  using pat_t = WithSkipper<CHILD_PAT_T, SKIP_T>;
  static constexpr DstBehavior dst_behavior() {
    return ParserFactory<CHILD_PAT_T>::dst_behavior();
  }
  enum {
    ATOMIC = ParserFactory<CHILD_PAT_T>::ATOMIC,
    FAILS_CLEANLY = ParserFactory<CHILD_PAT_T>::FAILS_CLEANLY,
  };
  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = WithSkipperImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T, SKIP_T>;
};
}  // namespace ABULAFIA_NAMESPACE



#endif
