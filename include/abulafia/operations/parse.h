//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_OPERATIONS_PARSE_H_
#define ABULAFIA_OPERATIONS_PARSE_H_

#include "abulafia/config.h"

#include "abulafia/contexts/single_forward.h"
#include "abulafia/pattern.h"
#include "abulafia/result.h"

namespace ABULAFIA_NAMESPACE {

// binds a pattern, a data source and a destination as a parser
// and executes it immediately. It's implied that this will be a
// single buffer parser.
// If you need a multi-buffer parser, use begin_parse() instead.
template <typename PAT_T, typename DATA_RANGE_T, typename DST_T>
result parse(const PAT_T& pat, const DATA_RANGE_T& data, DST_T& dst) {
  using iterator_t = decltype(std::begin(data));

  auto real_pat = make_pattern(pat);
  SingleForwardContext<iterator_t> ctx(std::begin(data), std::end(data));

  auto parser = make_parser(ctx, dst, real_pat);

  return parser.consume(ctx, dst, real_pat);
}

template <typename PAT_T, typename ITE_T, typename DST_T>
result parse(const PAT_T& pat, ITE_T b, ITE_T e, DST_T& dst) {
  auto real_pat = make_pattern(pat);
  SingleForwardContext<ITE_T> ctx(b, e);

  auto parser = make_parser(ctx, dst, real_pat);

  return parser.consume(ctx, dst, real_pat);
}

// calling parse() with no dst implies using a Nil as destination.
// The resulting parser simply checks that the range matches the pattern.
template <typename PAT_T, typename DATA_RANGE_T>
result parse(const PAT_T& pat, const DATA_RANGE_T& data) {
  Nil fake_dest;
  return parse(pat, data, fake_dest);
}
}  // namespace ABULAFIA_NAMESPACE

#endif
