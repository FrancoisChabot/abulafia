//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_CHILD_OPERATION_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_CHILD_OPERATION_H_INCLUDED

#include <concepts>
#include <iterator>

#include "abulafia/op_result.h"
#include "abulafia/parsers/coro/operation.h"

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

  constexpr op_result on_tokens(const ParentCtx& ctx) {
    return op_.on_tokens(make_sub_context(ctx));
  }

  constexpr op_result on_end(const ParentCtx& ctx) {
    return op_.on_end(make_sub_context(ctx));
  }

  void reset(const ParentCtx& ctx) {
    op_ = operation<sub_context_type, pattern_type>{make_sub_context(ctx)};
  }

 private:
  operation<sub_context_type, pattern_type> op_;
};
/*
// ***** child_op_set ***** /
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

#endif