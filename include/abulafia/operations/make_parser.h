//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_OPERATIONS_MAKE_PARSER_H_
#define ABULAFIA_OPERATIONS_MAKE_PARSER_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/pattern.h"
#include "abulafia/result.h"
#include "abulafia/support/nil.h"

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

#endif
