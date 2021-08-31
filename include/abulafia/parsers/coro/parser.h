//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_PARSER_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_PARSER_H_INCLUDED

#include <concepts>
#include <iterator>

#include "abulafia/op_result.h"
#include "abulafia/parsers/coro/context.h"
#include "abulafia/pattern.h"
#include "abulafia/policy.h"
#include "abulafia/token.h"

namespace abu::coro {

template <typename Ctx>
class operation;

// template <std::input_iterator I,
//           std::sentinel_for<I> S,
//           Pattern Pat,
//           Policy Pol,
//           typename ResultCb>
// class parser {
//   using root_ctx_type = parse_context<I, S, Pol, Pat, ResultCb>;

//  public:
//   constexpr parser(const Pat& pat, const ResultCb& cb) : pat_(pat), cb_(cb) {}

//   constexpr op_result advance(I i, S e) {
//     root_ctx_type ctx{i, e, pat_, cb_};
//     return root_op_.on_tokens(ctx);
//   }
//   constexpr op_result end() {
//     root_ctx_type ctx{i, e, pat_, cb_};
//     return root_op_.end(ctx);
//   }

//  private:
//   using root_op = operation<root_ctx_type>;

//   const Pat& pat_;
//   const ResultCb& cb_;
//   root_op root_op_;
// };

// template <std::input_iterator I, std::sentinel_for<I> S, Pattern Pat>
// class matcher {
//  public:
//   matcher(const Pat&) {}

//   constexpr op_result advance(I, S);
//   constexpr op_result end();
// };

template <Token TokType, Policy pol=default_policy, Pattern Pat, typename ResultCb>
constexpr auto make_parser(const Pat&, const ResultCb&) {

}

// template <std::input_iterator I,
//           std::sentinel_for<I> S,
//           Pattern Pat,
//           Policy Pol = default_policy>
// constexpr auto make_matcher(const Pat& pat) {
//   return matcher<I, S, Pat>{pat};
// }

// template <std::input_iterator I,
//           std::sentinel_for<I> S,
//           Pattern Pat,
//           op_type OpType,
//           Policy Pol>
// struct basic_context
//     : public ::abu::context<std::iter_value_t<I>, OpType, Pol> {
//   static constexpr op_type operation_type = OpType;

//   using pattern_type = Pat;
//   using iterator_type = I;
//   using sentinel_type = S;
//   using value_type = pattern_value_t<
//       Pat,
//       ::abu::context<std::iter_value_t<I>, op_type::parse, Pol>>;

//   constexpr basic_context(I& init_ite, S init_end, const Pat& pat)
//       : iterator(init_ite), end(init_end), pattern(pat) {}

//   iterator_type& iterator;
//   sentinel_type end;
//   const pattern_type& pattern;
// };

// template <std::input_iterator I,
//           std::sentinel_for<I> S,
//           Pattern Pat,
//           op_type OpType,
//           Policy Pol = default_policy>
// struct context : public basic_context<I, S, Pat, OpType, Pol> {
//   using basic_context<I, S, Pat, OpType, Pol>::basic_context;
// };

// template <std::input_iterator I,
//           std::sentinel_for<I> S,
//           Pattern Pat,
//           Policy Pol>
// struct context<I, S, Pat, op_type::parse, Pol>
//     : public basic_context<I, S, Pat, op_type::parse, Pol> {
//   using value_type = pattern_value_t<
//       Pat,
//       ::abu::context<std::iter_value_t<I>, op_type::parse, Pol>>;

//   constexpr context(I& init_ite, S init_end, const Pat& pat, value_type& dst)
//       : basic_context<I, S, Pat, op_type::parse, Pol>(init_ite, init_end,
//       pat),
//         result_value(dst) {}

//   value_type& result_value;
// };

// template <std::forward_iterator I,
//           std::sentinel_for<I> S,
//           Pattern Pat,
//           op_type OpType,
//           Policy Pol>
// struct context<I, S, Pat, OpType, Pol>
//     : public ::abu::context<std::iter_value_t<I>, OpType, Pol> {
//   static constexpr op_type operation_type = OpType;

//   using pattern_type = Pat;
//   using iterator_type = I;
//   using sentinel_type = S;
//   using value_type =
//       pattern_value_t<Pat, ::abu::context<std::iter_value_t<I>, OpType,
//       Pol>>;

//   using checkpoint_type = I;

//   constexpr context(I& init_ite, S init_end, const Pat& pat)
//       : iterator(init_ite), end(init_end), pattern(pat) {}

//   checkpoint_type checkpoint() const { return iterator; }

//   void rollback(checkpoint_type cp) const { iterator = cp; }

//   iterator_type& iterator;
//   sentinel_type end;
//   const pattern_type& pattern;
// };

}  // namespace abu::coro

#endif