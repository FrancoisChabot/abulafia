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
#include <cstring>
#include <exception>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <stack>
#include <stdexcept>
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
  using base_ctx_t = SingleForwardDataSource<ITE_T>;
  base_ctx_t& root_ctx() { return *this; }
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
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
enum class Result { SUCCESS, FAILURE, PARTIAL };
}

namespace ABULAFIA_NAMESPACE {
struct Nil {
  using value_type = Nil;
  using dst_type = Nil;
  Nil() = default;
  template <typename T>
  Nil(T const&) {}
  Nil& get() {
    return *this;
  }
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
class Fail : public Pattern<Fail> {};
template <typename CTX_T>
struct pat_attr_t<Fail, CTX_T> {
  using attr_type = Nil;
};
static constexpr Fail fail;
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
template <std::size_t BASE, std::size_t DIGITS_MIN = 1, std::size_t DIGITS_MAX = 0>
class Uint : public Pattern<Uint<BASE, DIGITS_MIN, DIGITS_MAX>> {
public:
  static constexpr std::size_t base() { return BASE; }
  static constexpr std::size_t digits_min() { return DIGITS_MIN; }
  static constexpr std::size_t digits_max() { return DIGITS_MAX; }
  static_assert(DIGITS_MIN >= 1, "Numeric parser must parse at least 1 digit");
  static_assert(DIGITS_MAX >= DIGITS_MIN || DIGITS_MAX == 0,
                "Max < Min? really?");
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
  // This should have a static create(CTX_T, DST_T, PAT_T const&) function
  template<typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
  struct ParserFactory;
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template<template<typename , typename , typename, typename> typename PARSER_T,
  typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
struct ParserBase {
  template<typename T>
  using change_dst_t = PARSER_T<CTX_T, T, REQ_T, PAT_T>;
};
template <typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
using Parser = decltype(
  ParserFactory<CTX_T, DST_T, REQ_T, PAT_T>::create(std::declval<CTX_T>(),
                                                    std::declval<DST_T>(),
                                                    std::declval<PAT_T>()));
template <typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
auto make_parser_(CTX_T ctx, DST_T dst, REQ_T, PAT_T const& pat) {
  return ParserFactory<CTX_T, DST_T, REQ_T, PAT_T>::create(ctx, dst, pat);
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
  template<typename T>
  class ValueWrapper {
  public:
    using dst_type = T;
    ValueWrapper(T& v) : v_(v) {}
    ValueWrapper(ValueWrapper const&) = default;
    template<typename U>
    ValueWrapper& operator=(U&& v) {
      v_ = std::forward<U>(v);
      return *this;
    }
    // using this implies that we are NOT atomic in nature.
    T& get() {
      return v_;
    }
  private:
    T& v_;
  };
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
  namespace details {
    template <typename CONT_T, typename... ARGS>
    void append_to_container(CONT_T& container, ARGS&&... args) {
      container.emplace_back(std::forward<ARGS>(args)...);
    }
    // std::basic_string does not have an emplace_back, and is a commonly used type.
    template <class CharT, class Traits = std::char_traits<CharT>,
      class Allocator = std::allocator<CharT>, typename... ARGS>
      void append_to_container(std::basic_string<CharT, Traits, Allocator>& container,
        ARGS&&... args) {
      container.push_back(std::forward<ARGS>(args)...);
    }
  }  // namespace details
  template<typename T>
  class CollectionWrapper {
  public:
    using dst_type = typename T::value_type;
    CollectionWrapper(T& v) : v_(v) {}
    CollectionWrapper(CollectionWrapper const&) = default;
    template<typename U>
    CollectionWrapper& operator=(U&& rhs) {
      details::append_to_container(v_, std::forward<U>(rhs));
      return *this;
    }
    void clear() {
      v_.clear();
    }
  private:
    T& v_;
  };
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
  template <typename T, typename Enable=void>
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
  template<typename T>
  auto wrap_dst(T& dst) {
    return typename SelectDstWrapper<T>::type(dst);
  }
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
  // This is to be used to enforce ATOMIC on a parser
  // that does not naturally meet that requirement
  template<typename PARSER_T>
  class AtomicAdapter {
  public:
    using ctx_t = typename PARSER_T::ctx_t;
    using dst_t = typename PARSER_T::dst_t;
    using pat_t = typename PARSER_T::pat_t;
    using buffer_t = typename dst_t::dst_type;
    using adapted_dst_t = typename SelectDstWrapper<buffer_t>::type;
    using adapted_parser_t = typename PARSER_T::template change_dst_t<adapted_dst_t>;
    AtomicAdapter(ctx_t ctx, dst_t, pat_t const& pat)
      : adapted_parser_(ctx, adapted_dst_t(buffer_), pat) {
    }
    Result consume(ctx_t ctx, dst_t dst, pat_t const& pat) {
      auto status = adapted_parser_.consume(ctx, adapted_dst_t(buffer_), pat);
      if (status == Result::SUCCESS) {
        dst = buffer_;
      }
      return status;
    }
  private:
    buffer_t buffer_;
    adapted_parser_t adapted_parser_;
  };
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template<typename PARSER_T>
class CleanFailureAdapter {
public:
  using ctx_t = typename PARSER_T::ctx_t;
  using dst_t = typename PARSER_T::dst_t;
  using pat_t = typename PARSER_T::pat_t;
  CleanFailureAdapter(ctx_t ctx, dst_t dst, pat_t const& pat)
    : adapted_parser_(ctx, dst, pat) {
    ctx.data().prepare_rollback();
  }
  Result consume(ctx_t ctx, dst_t dst, pat_t const& pat) {
    auto status = adapted_parser_.consume(ctx, dst, pat);
    switch (status) {
    case Result::SUCCESS:
      ctx.data().cancel_rollback();
      break;
    case Result::FAILURE:
      ctx.data().commit_rollback();
      break;
    }
    return status;
  }
private:
  PARSER_T adapted_parser_;
};
}  // namespace ABULAFIA_NAMESPACE

#define PARSER_REQUEST_TABLE                                  \
  PARSER_REQUEST_ENTRY(ATOMIC, AtomicAdapter)                \
  PARSER_REQUEST_ENTRY(FAILS_CLEANLY, CleanFailureAdapter)
namespace ABULAFIA_NAMESPACE {
  enum class Req {
#define PARSER_REQUEST_ENTRY(R, A) R,
    PARSER_REQUEST_TABLE
#undef PARSER_REQUEST_ENTRY
  };
  struct DefaultReqs {
    enum {
#define PARSER_REQUEST_ENTRY(R, A) R = false,
      PARSER_REQUEST_TABLE
#undef PARSER_REQUEST_ENTRY
    };
  };
  template<typename PARSER_T, typename REQ_T, Req req, bool enable, typename enable_2=void>
  struct ReqFullfiller {
    using type = PARSER_T;
  };
#define PARSER_REQUEST_ENTRY(Request, Adapter) \
  template<typename PARSER_T, typename REQ_T> \
  struct ReqFullfiller<PARSER_T,              \
                       REQ_T,                 \
                       Req:: Request,         \
                       true,                  \
                       std::enable_if_t<      \
                         REQ_T:: Request >> { \
    using type = Adapter <PARSER_T>;                                \
  };
  PARSER_REQUEST_TABLE
#undef PARSER_REQUEST_ENTRY
  template<typename PARSER_T, typename REQ_T, Req req, bool enable>
  using fulfill_req_if_t = typename ReqFullfiller<PARSER_T, REQ_T, req, enable>::type;
}  // namespace ABULAFIA_NAMESPACE
#undef PARSER_REQUEST_TABLE

namespace ABULAFIA_NAMESPACE {
template <typename REAL_PAT_T, typename CTX_T, typename DST_T>
struct ParserInterface {
  ParserInterface(REAL_PAT_T const& pat, CTX_T& ctx, DST_T& dst)
      : pat_(pat), ctx_(ctx), dst_(dst), parser_(ctx, dst, pat) {}
  Result consume() { return parser_.consume(ctx_, dst_, pat_); }
 private:
  REAL_PAT_T const& pat_;
  CTX_T& ctx_;
  DST_T& dst_;
  Parser<CTX_T, DST_T, REAL_PAT_T, DefaultReqs> parser_;
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
  template<typename DATASOURCE_T, typename SKIPPER_T>
  struct Context {
    using datasource_t = DATASOURCE_T;
    using skip_pattern_t = SKIPPER_T;
    Context(datasource_t& ds, skip_pattern_t const& skip)
      : data_(ds), skipper_(skip) {}
    template<typename T>
    using set_skipper_t = Context<datasource_t, T>;
    enum {
      IS_RESUMABLE = DATASOURCE_T::IS_RESUMABLE,
      HAS_SKIPPER = !std::is_same<Fail, skip_pattern_t>::value,
    };
    DATASOURCE_T& data() { return data_; }
  private:
    DATASOURCE_T& data_;
    SKIPPER_T const& skipper_;
  };
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename ITE_T, typename PAT_T, typename DST_T>
Result parse(ITE_T b, ITE_T e, const PAT_T& pat, DST_T& dst) {
  SingleForwardDataSource<ITE_T> data(b, e);
  auto real_pat = make_pattern(pat);
  auto real_dst = wrap_dst(dst);
  Context<SingleForwardDataSource<ITE_T>, Fail> real_ctx(data, fail);
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
template <typename CHARSET_T>
class Char : public Pattern<Char<CHARSET_T>> {
  CHARSET_T char_set_;
 public:
  Char(CHARSET_T chars) : char_set_(std::move(chars)) {}
  CHARSET_T const& char_set() const { return char_set_; }
};
template <typename CHARSET_T, typename RECUR_TAG>
struct pattern_traits<Char<CHARSET_T>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    BACKTRACKS = false,
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
template <typename PAT_T, std::size_t MIN_REP, std::size_t MAX_REP>
class Repeat : public Pattern<Repeat<PAT_T, MIN_REP, MAX_REP>> {
  PAT_T operand_;
 public:
  Repeat(const PAT_T& op) : operand_(op) {}
  Repeat(PAT_T&& op) : operand_(std::move(op)) {}
  PAT_T const& operand() const { return operand_; }
};
template <std::size_t MIN_REP, std::size_t MAX_REP, typename PAT_T, typename CB_T>
auto transform(Repeat<PAT_T, MIN_REP, MAX_REP> const& tgt, CB_T const& cb) {
  return repeat(cb(tgt.operand()));
}
template <std::size_t MIN_REP = 0, std::size_t MAX_REP = 0, typename PAT_T>
inline auto repeat(PAT_T&& pat) {
  return Repeat<pattern_t<PAT_T>, MIN_REP, MAX_REP>(
      make_pattern(forward<PAT_T>(pat)));
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
static constexpr Int<10, 1, 0> int_;
static constexpr Uint<10, 1, 0> uint_;
static constexpr Eoi eoi;
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
template <typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
class UIntImpl : public ParserBase<UIntImpl, CTX_T, DST_T, REQ_T, PAT_T> {
public:
  using digit_vals = DigitValues<PAT_T::base()>;
  UIntImpl(CTX_T, DST_T dst, PAT_T const&) {
    dst.get() = 0;
  }
  Result consume(CTX_T ctx, DST_T dst, PAT_T const&) {
    while (true) {
      if (ctx.data().empty()) {
        if (ctx.data().final_buffer()) {
          return digit_count_ >= PAT_T::digits_min() ? Result::SUCCESS : Result::FAILURE;
        } else {
          return Result::PARTIAL;
        }
      }
      auto next = ctx.data().next();
      if (digit_vals::is_valid(next)) {
        dst.get() *= DST_T::dst_type(PAT_T::base());
        dst.get() += digit_vals::value(next);
        ++digit_count_;
        ctx.data().advance();
        if (digit_count_ == PAT_T::digits_max()) {
          return Result::SUCCESS;
        }
      } else {
        return digit_count_ >= PAT_T::digits_min() ? Result::SUCCESS : Result::FAILURE;
      }
    }
  }
 private:
  std::size_t digit_count_ = 0;
};
template <typename CTX_T, typename DST_T, typename REQ_T,
  std::size_t BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX>
struct ParserFactory<CTX_T, DST_T, REQ_T, Uint<BASE, DIGITS_MIN, DIGITS_MAX>> {
  static auto create(CTX_T ctx, DST_T dst, Uint<BASE, DIGITS_MIN, DIGITS_MAX> const& pat) {
    constexpr bool apply_atomic = !std::is_same<Nil, DST_T>::value;
    constexpr bool apply_fails_cleanly = DIGITS_MAX != 1;
    using a = UIntImpl<CTX_T, DST_T, REQ_T, Uint<BASE, DIGITS_MIN, DIGITS_MAX>>;
    using b = fulfill_req_if_t<a, REQ_T, Req::ATOMIC, apply_atomic>;
    using c = fulfill_req_if_t<b, REQ_T, Req::FAILS_CLEANLY, apply_fails_cleanly>;
    return c(ctx, dst, pat);
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename CHARSET_T>
class CharImpl {
  using PAT_T = Char<CHARSET_T>;
 public:
   CharImpl(CTX_T, DST_T, PAT_T const&) {}
  Result consume(CTX_T ctx, DST_T dst, PAT_T const& pat) {
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
template <typename CTX_T, typename DST_T, typename REQ_T, typename CHARSET_T >
struct ParserFactory<CTX_T, DST_T, REQ_T, Char<CHARSET_T>> {
  static auto create(CTX_T ctx, DST_T dst, Char<CHARSET_T> const& pat) {
    return CharImpl<CTX_T, DST_T, CHARSET_T>(ctx, dst, pat);
  }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T,
  typename CHILD_PAT_T, std::size_t MIN_REP, std::size_t MAX_REP>
class RepeatImpl {
  using ctx_t = CTX_T;
  using dst_t = DST_T;
  using req_t = REQ_T;
  using pat_t = Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>;
  struct child_req_t : public req_t {
    enum {
      // The aternative is to push_back on start, and pop_back on failure,
      // which gets a little messy.
      ATOMIC = true,
      // This is extremely important, since we can succeed even if the child parser fails.
      // The exception to this is if MIN_REP == MAX_REP (except for 0). In which case, failure
      // of the child guarantees failure of the parent.
      FAILS_CLEANLY = MIN_REP != MAX_REP || MAX_REP == 0
    };
  };
  using child_dst_t = dst_t;
  using child_parser_t = Parser<ctx_t, child_dst_t, child_req_t, CHILD_PAT_T>;
  std::size_t count_ = 0;
  child_parser_t child_parser_;
 public:
  RepeatImpl(ctx_t ctx, dst_t dst, pat_t const& pat)
      : child_parser_(ctx, child_dst_t(dst), pat.operand()) {
    dst.clear();
  }
  Result consume(ctx_t ctx, dst_t dst, pat_t const& pat) {
    while (1) {
      auto child_res = child_parser_.consume(ctx, child_dst_t(dst), pat.operand());
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
template <typename CTX_T, typename DST_T, typename REQ_T,
  typename CHILD_PAT_T, std::size_t MIN_REP, std::size_t MAX_REP>
struct ParserFactory<CTX_T, DST_T, REQ_T, Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>> {
  static auto create(CTX_T ctx, DST_T dst, Repeat<CHILD_PAT_T, MIN_REP, MAX_REP> const& pat) {
    // Check which requirements we need to fullfill
    constexpr bool apply_fails_cleanly = MIN_REP != MAX_REP || MAX_REP == 0;
    // Determine if we need to re-wrap the dst
    using a = RepeatImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T, MIN_REP, MAX_REP>;
    using b = fulfill_req_if_t<a, REQ_T, Req::FAILS_CLEANLY, apply_fails_cleanly>;
    return b(ctx, dst, pat);
  }
};
}  // namespace ABULAFIA_NAMESPACE



#endif
