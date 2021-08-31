//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_CNTEXT_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_CNTEXT_H_INCLUDED

#include <concepts>
#include <iterator>

#include "abulafia/context.h"
#include "abulafia/op_result.h"
#include "abulafia/pattern.h"
#include "abulafia/policy.h"
#include "abulafia/token.h"

namespace abu::coro {

template <typename T>
class operation;
// template <std::input_iterator I,
//           std::sentinel_for<I> S,
//           Policy Pol,
//           Pattern Pat,
//           op_type OpType>
// struct context;

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

// template <std::input_iterator I,
//           std::sentinel_for<I> S,
//           Policy Pol,
//           Pattern Pat,
//           typename ResultCb>
// struct parse_context
//     : public op_context<std::iter_value_t<I>, Pol, Pat, op_type::parse> {
//   using iterator_type = I;
//   using sentinel_type = S;
//   using value_type =
//       typename op_context<std::iter_value_t<I>, Pol, Pat, op_type::parse>::
//           value_type;

//   constexpr parse_context(I& init_ite,
//                           S init_end,
//                           const Pat& pat,
//                           const ResultCb& cb)
//       : iterator(init_ite), end(init_end), pattern(pat), cb_(cb) {}

//   constexpr void return_value(value_type v) const { cb_(std::move(v)); }

//   iterator_type& iterator;
//   sentinel_type end;
//   const Pat& pattern;
//   const ResultCb& cb_;
// };

template <typename T, typename PatTag, op_type OpType = op_type::any>
concept ContextFor = PatternContext<T, PatTag> &&
    (T::operation_type == OpType || OpType == op_type::any);

}  // namespace abu::coro

#endif