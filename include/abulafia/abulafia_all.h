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
#include <memory>
#include <set>
#include <stack>
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
template <template <typename...> class C, typename...Ts>
std::true_type is_base_of_template_impl(const C<Ts...>*);
template <template <typename...> class C>
std::false_type is_base_of_template_impl(...);
template <typename T, template <typename...> class C>
using is_base_of_template = decltype(is_base_of_template_impl<C>(std::declval<T*>()));
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
    }
    else {
      rollback_stack_.emplace_back(current_, current_buffer_);
    }
  }
  void commit_rollback() {
    if (empty_rollbacks_) {
      --empty_rollbacks_;
    }
    else {
      current_buffer_ = rollback_stack_.back().second;
      current_ = rollback_stack_.back().first;
      rollback_stack_.pop_back();
      cleanup_rollback_();
    }
  }
  void cancel_rollback() {
    if (empty_rollbacks_) {
      --empty_rollbacks_;
    }
    else {
      rollback_stack_.pop_back();
      cleanup_rollback_();
    }
  }
  bool isResumable() {return true;}
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
  bool isResumable() {return false;}
};
}  // namespace ABULAFIA_NAMESPACE

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
  bool operator==(Nil const&) const {return true;}
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
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CRTP_T>
class Pattern {
public:
  using pat_t = CRTP_T;
};
template <typename T>
struct expr_traits<
    T, enable_if_t<is_base_of_template<decay_t<T>, Pattern>::value>> {
  enum { is_pattern = true, converts_to_pattern = false };
  static const T& make_pattern(const T& v) { return v; }
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
  template<typename CRTP>
  struct LeafPattern : public Pattern<CRTP> {};
  template <typename T,
    typename CB_T,
    typename = std::enable_if_t<is_base_of_template<T, LeafPattern>::value>>
  T transform(T const& tgt, CB_T const&) {
    return tgt;
  }
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
class Fail : public LeafPattern<Fail> {};
static constexpr Fail fail;
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
enum class Result { SUCCESS, FAILURE, PARTIAL };
}

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
class UInt : public LeafPattern<UInt<BASE, DIGITS_MIN, DIGITS_MAX>> {
public:
  static_assert(DIGITS_MIN >= 1);
  static_assert(DIGITS_MAX >= DIGITS_MIN || DIGITS_MAX == 0);
};
static constexpr UInt<10, 1, 0> uint_;
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
  template<typename CTX_T, typename DST_T, typename REQ_T, typename PARSER_FACTORY_T>
  class AtomicAdapter {
  public:
    using pat_t = typename PARSER_FACTORY_T::pat_t;
    using buffer_t = typename DST_T::dst_type;
    struct adapted_reqs_t : public REQ_T {
      enum { ATOMIC = false };
    };
    using adapted_dst_t = typename SelectDstWrapper<buffer_t>::type;
    using child_parser_t = typename PARSER_FACTORY_T:: template type<CTX_T, adapted_dst_t, adapted_reqs_t>;
    AtomicAdapter(CTX_T ctx, DST_T, pat_t const& pat)
      : adapted_parser_(ctx, adapted_dst_t(buffer_), pat) {
    }
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
  template<typename FACTORY_T>
  struct AtomicFactoryAdapter {
    static_assert(!FACTORY_T::ATOMIC);
    using pat_t = typename FACTORY_T::pat_t;
    enum {
      ATOMIC = true,
      FAILS_CLEANLY = FACTORY_T::FAILS_CLEANLY,
    };
    template<typename CTX_T, typename DST_T, typename REQ_T>
    using type = AtomicAdapter<CTX_T, DST_T, REQ_T, FACTORY_T>;
  };
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template<typename CTX_T, typename DST_T, typename REQ_T, typename PARSER_FACTORY_T>
class CleanFailureAdapter {
public:
  using pat_t = typename PARSER_FACTORY_T::pat_t;
  struct adapted_reqs_t : public REQ_T {
    enum { FAILS_CLEANLY = false };
  };
  using child_parser_t = typename PARSER_FACTORY_T:: template type<CTX_T, DST_T, adapted_reqs_t>;
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
  template<typename FACTORY_T>
  struct CleanFailureFactoryAdapter {
    static_assert(!FACTORY_T::FAILS_CLEANLY);
    using pat_t = typename FACTORY_T::pat_t;
    enum {
      ATOMIC = FACTORY_T::ATOMIC,
      FAILS_CLEANLY = true,
    };
    template<typename CTX_T, typename DST_T, typename REQ_T>
    using type = CleanFailureAdapter<CTX_T, DST_T, REQ_T, FACTORY_T>;
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
  template<typename PAT_T>
  struct ParserFactory;
  template<typename T, template<typename> typename ADAPT_T, bool enable>
  struct ConditionalAdapter {
    using type = T;
  };
  template<typename T, template<typename> typename ADAPT_T>
  struct ConditionalAdapter<T, ADAPT_T, true> {
    using type = ADAPT_T<T>;
  };
  template<typename T, template<typename> typename ADAPT_T, bool enable>
  using ConditionalAdapter_t = typename ConditionalAdapter<T, ADAPT_T, enable>::type;
  template<typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
  struct AdaptedParserFactory {
    static auto create(CTX_T ctx, DST_T dst, PAT_T const& pat) {
      using raw_factory = ParserFactory<PAT_T>;
      constexpr bool apply_atomic_adapter = REQ_T::ATOMIC && !raw_factory::ATOMIC;
      constexpr bool apply_clean_failure_adapter = REQ_T::FAILS_CLEANLY && !raw_factory::FAILS_CLEANLY;
      using a = raw_factory;
      using b = ConditionalAdapter_t<a, AtomicFactoryAdapter, apply_atomic_adapter>;
      using c = ConditionalAdapter_t<b, CleanFailureFactoryAdapter, apply_clean_failure_adapter>;
      //TODO: Apply skipper here.
      using parser_type = typename c::template type<CTX_T, DST_T, REQ_T>;
      return parser_type(ctx, dst, pat);
    }
  };
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
using Parser = decltype(
  AdaptedParserFactory<CTX_T, DST_T, REQ_T, PAT_T>::create(std::declval<CTX_T>(),
                                                    std::declval<DST_T>(),
                                                    std::declval<PAT_T>()));
template <typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
auto make_parser_(CTX_T ctx, DST_T dst, REQ_T, PAT_T const& pat) {
  return AdaptedParserFactory<CTX_T, DST_T, REQ_T, PAT_T>::create(ctx, dst, pat);
}
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename REAL_PAT_T, typename REAL_DST_T, typename DATASOURCE_T>
struct ParserInterface {
  ParserInterface(REAL_PAT_T const& pat, REAL_DST_T& dst)
    : ctx_(data_source_, fail)
    , pat_(pat), dst_(dst), parser_(ctx_, dst, pat) {}
  DATASOURCE_T& data() { return data_source_; }
  Result consume() {
    return parser_.consume(ctx_, dst_, pat_);
  }
 private:
  using CTX_T = Context<DATASOURCE_T, Fail>;
  DATASOURCE_T data_source_;
  CTX_T ctx_;
  REAL_PAT_T pat_;
  REAL_DST_T dst_;
  Parser<CTX_T, REAL_DST_T, DefaultReqs, REAL_PAT_T> parser_;
};
template <typename BUFFER_T , typename PAT_T, typename DST_T>
auto make_parser(PAT_T const& p, DST_T& s) {
  auto real_pat = make_pattern(p);
  auto real_dst = wrap_dst(s);
  return ParserInterface<decltype(real_pat), decltype(real_dst), ContainerSequenceDataSource<BUFFER_T>>(real_pat, real_dst);
}
template <typename BUFFER_T , typename PAT_T>
auto make_parser(PAT_T const& p) {
  return make_parser<BUFFER_T>(p, nil);
}
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
class Char : public LeafPattern<Char<CHARSET_T>> {
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
template <typename PAT_T, typename CB_T>
auto transform(Discard<PAT_T> const& tgt, CB_T const& cb) {
  return discard(cb(tgt.operand()));
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
class Eoi : public LeafPattern<Eoi> {};
static constexpr Eoi eoi;
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
class Pass : public LeafPattern<Pass> {};
static constexpr Pass pass;
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <std::size_t BASE, std::size_t DIGITS_MIN = 1, std::size_t DIGITS_MAX = 0>
class Int : public LeafPattern<Int<BASE, DIGITS_MIN, DIGITS_MAX>> {
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
operator|(LHS_T&& lhs, RHS_T&& rhs) {
  return detail::NaryPatternBuilder<Alt, pattern_t<LHS_T>, pattern_t<RHS_T>>::
      build(make_pattern(forward<LHS_T>(lhs)),
            make_pattern(forward<RHS_T>(rhs)));
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
template <std::size_t MIN_REP, std::size_t MAX_REP, typename PAT_T, typename CB_T>
auto transform(Repeat<PAT_T, MIN_REP, MAX_REP> const& tgt, CB_T const& cb) {
  return repeat(cb(tgt.operand()));
}
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
template <typename CTX_T, typename DST_T,
  std::size_t BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX>
class IntImpl {
public:
  using pat_t = Int<BASE, DIGITS_MIN, DIGITS_MAX>;
  using digit_vals = DigitValues<BASE>;
  IntImpl(CTX_T, DST_T dst, pat_t const&) {
    dst.get() = 0;
  }
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
template<std::size_t BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX>
struct ParserFactory<Int<BASE, DIGITS_MIN, DIGITS_MAX>> {
  using pat_t = Int<BASE, DIGITS_MIN, DIGITS_MAX>;
  enum {
    //TODO: We could provide ATOMIC in the DIGITS_MIN == DIGITS_MAX case
    ATOMIC = false,
    FAILS_CLEANLY = DIGITS_MAX == 1,
  };
  template<typename CTX_T, typename DST_T, typename REQ_T>
  using type = IntImpl<CTX_T, DST_T, BASE, DIGITS_MIN, DIGITS_MAX>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T,
  std::size_t BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX>
class UIntImpl {
public:
  using pat_t = UInt<BASE, DIGITS_MIN, DIGITS_MAX>;
  using digit_vals = DigitValues<BASE>;
  UIntImpl(CTX_T, DST_T dst, pat_t const&) {
    dst.get() = 0;
  }
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
        dst.get() *= typename DST_T::dst_type(BASE);
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
template<std::size_t BASE, std::size_t DIGITS_MIN, std::size_t DIGITS_MAX>
struct ParserFactory<UInt<BASE, DIGITS_MIN, DIGITS_MAX>> {
  using pat_t = UInt<BASE, DIGITS_MIN, DIGITS_MAX>;
  enum {
    //TODO: We could provide ATOMIC in the DIGITS_MIN == DIGITS_MAX case
    ATOMIC = false,
    FAILS_CLEANLY = DIGITS_MAX == 1,
  };
  template<typename CTX_T, typename DST_T, typename REQ_T>
  using type = UIntImpl<CTX_T, DST_T, BASE, DIGITS_MIN, DIGITS_MAX>;
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
template <typename CHARSET_T >
struct ParserFactory<Char<CHARSET_T>> {
  using pat_t = Char<CHARSET_T>;
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template<typename CTX_T, typename DST_T, typename REQ_T>
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
  Result consume(CTX_T ctx, Nil, Eoi const& pat) {
    return peek(ctx, pat);
  }
  Result peek(CTX_T ctx, Eoi const&) {
    if (ctx.data().empty()) {
      return ctx.data().final_buffer() ? Result::SUCCESS : Result::PARTIAL;
    }
    return Result::FAILURE;
  }
};
template<>
struct ParserFactory<Eoi> {
  using pat_t = Eoi;
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template<typename CTX_T, typename DST_T, typename REQ_T>
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
template<>
struct ParserFactory<Fail> {
  using pat_t = Fail;
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template<typename CTX_T, typename DST_T, typename REQ_T>
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
template<>
struct ParserFactory<Pass> {
  using pat_t = Pass;
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template<typename CTX_T, typename DST_T, typename REQ_T>
  using type = PassImpl<CTX_T, REQ_T>;
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
template <typename CTX_T, typename DST_T, typename REQ_T, typename... CHILD_PATS_T>
class AltImpl {
  using pat_t = Alt<CHILD_PATS_T...>;
  struct child_req_t : public REQ_T {
    enum {
      FAILS_CLEANLY = true
    };
  };
  using child_parsers_t = std::variant<Parser<CTX_T, DST_T, child_req_t, CHILD_PATS_T>...>;
  child_parsers_t child_parsers_;
 public:
  AltImpl(CTX_T& ctx, DST_T& dst, pat_t const& pat)
      : child_parsers_(std::in_place_index_t<0>(), ctx, dst, getChild<0>(pat)) {}
  Result consume(CTX_T& ctx, DST_T& dst, pat_t const& pat) {
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
  Result consume_from(CTX_T& ctx, DST_T& dst, pat_t const& pat) {
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
        child_parsers_ = child_parsers_t(std::in_place_index_t<new_id>(),
                                         ctx, dst, new_c_pattern);
        return consume_from<new_id>(ctx, dst, pat);
      }
    }
    return child_res;
  }
};
template <typename... CHILD_PATS_T>
struct ParserFactory<Alt<CHILD_PATS_T...>> {
  using pat_t = Alt<CHILD_PATS_T...>;
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template<typename CTX_T, typename DST_T, typename REQ_T>
  using type = AltImpl<CTX_T, DST_T, REQ_T, CHILD_PATS_T...>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T,
  typename CHILD_PAT_T>
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
  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };
  template<typename CTX_T, typename DST_T, typename REQ_T>
  using type = DiscardImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T>;
};
}  // namespace ABULAFIA_NAMESPACE

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T,
  typename CHILD_PAT_T, std::size_t MIN_REP, std::size_t MAX_REP>
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
      // This is extremely important, since we can succeed even if the child parser fails.
      // The exception to this is if MIN_REP == MAX_REP (except for 0). In which case, failure
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
template <typename CHILD_PAT_T, std::size_t MIN_REP, std::size_t MAX_REP>
struct ParserFactory<Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>> {
  using pat_t = Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>;
  enum {
    ATOMIC = false,
    FAILS_CLEANLY = MIN_REP == MAX_REP && MAX_REP != 0,
  };
  template<typename CTX_T, typename DST_T, typename REQ_T>
  using type = RepeatImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T, MIN_REP, MAX_REP>;
};
}  // namespace ABULAFIA_NAMESPACE



#endif
