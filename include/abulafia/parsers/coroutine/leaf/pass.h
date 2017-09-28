//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_PASS_H_
#define ABULAFIA_PARSERS_COROUTINE_PASS_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/leaf/pass.h"
#include "abulafia/support/assert.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T>
class Parser<CTX_T, Nil, Pass>
    : public ParserBase<CTX_T, Nil, PARSER_OPT_NO_SKIP> {
  using PAT_T = Pass;

 public:
  Parser(CTX_T& ctx, Nil&, PAT_T const&)
      : ParserBase<CTX_T, Nil, PARSER_OPT_NO_SKIP>(ctx, nil) {}

  result consume(CTX_T&, Nil&, PAT_T const&) { return result::SUCCESS; }

  result peek(CTX_T&, PAT_T const&) { return result::SUCCESS; }
};

}  // namespace ABULAFIA_NAMESPACE

#endif
