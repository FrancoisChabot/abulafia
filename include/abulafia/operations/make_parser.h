//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_OPERATIONS_MAKE_PARSER_H_
#define ABULAFIA_OPERATIONS_MAKE_PARSER_H_

#include "abulafia/config.h"
#include "abulafia/context.h"
#include "abulafia/parser.h"
#include "abulafia/patterns/pattern.h"
#include "abulafia/result.h"
#include "abulafia/support/nil.h"
#include "abulafia/parsers/coroutine/parser_factory.h"
#include "abulafia/data_source/container_sequence.h"

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

#endif
