//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_OPERATIONS_DECODE_H_
#define ABULAFIA_OPERATIONS_DECODE_H_

#include "abulafia/config.h"

#include "abulafia/operations/parse.h"

namespace ABULAFIA_NAMESPACE {

// binds a pattern, a data source and a destination as a parser
// and executes it immediately. It's implied that this will be a
// single buffer parser.
// If you need a multi-buffer parser, use begin_parse() instead.

template <typename DST_T, typename ITE_T, typename PAT_T>
DST_T decode(ITE_T b, ITE_T e, const PAT_T& pat) {
  DST_T dst;

  auto status = parse(b, e, pat, dst);

  if(status != Result::SUCCESS) {
    throw std::runtime_error("abulafia decode failure");
  }

  return dst;
}

template <typename DST_T, typename DATA_RANGE_T, typename PAT_T>
DST_T decode(const DATA_RANGE_T& data, const PAT_T& pat) {
  return decode<DST_T>(std::begin(data), std::end(data), pat);
}

}  // namespace ABULAFIA_NAMESPACE

#endif
