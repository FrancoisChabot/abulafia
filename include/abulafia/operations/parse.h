//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_OPERATIONS_PARSE_H_
#define ABULAFIA_OPERATIONS_PARSE_H_

#include "abulafia/config.h"

#include "abulafia/data_source/single_forward.h"
#include "abulafia/dst_wrapper/select_wrapper.h"

#include "abulafia/parsers/coroutine/parser_factory.h"
#include "abulafia/patterns/leaf/fail.h"

#include "abulafia/context.h"
#include "abulafia/patterns/pattern.h"
#include "abulafia/result.h"

namespace ABULAFIA_NAMESPACE {

// binds a pattern, a data source and a destination as a parser
// and executes it immediately. It's implied that this will be a
// single buffer parser.
// If you need a multi-buffer parser, use make_parser() instead.
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

// calling parse() with no dst implies using a Nil as destination.
// The resulting parser simply checks that the range matches the pattern.
template <typename ITE_T, typename PAT_T>
Result parse(ITE_T b, ITE_T e, const PAT_T& pat) {
  return parse(b, e, pat, nil);
}

// Container-based verison of the parse api.
template <typename DATA_RANGE_T, typename PAT_T, typename DST_T>
Result parse(const DATA_RANGE_T& data, const PAT_T& pat, DST_T& dst) {
  return parse(std::begin(data), std::end(data), pat, dst);
}

template <typename PAT_T, typename DATA_RANGE_T>
Result parse(const DATA_RANGE_T& data, const PAT_T& pat) {
  return parse(std::begin(data), std::end(data), pat, nil);
}
}  // namespace ABULAFIA_NAMESPACE

#endif
