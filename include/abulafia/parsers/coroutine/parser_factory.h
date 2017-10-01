//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_PARSER_FACTORY_H_
#define ABULAFIA_PARSERS_COROUTINE_PARSER_FACTORY_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/parsers/helpers/digit_values.h"
#include "abulafia/patterns/leaf/numeric/uint.h"
#include "abulafia/support/assert.h"

namespace ABULAFIA_NAMESPACE {
  
  // This should have a static create(CTX_T, DST_T, PAT_T const&) function
  template<typename CTX_T, typename DST_T, typename PAT_T>
  struct ParserFactory;

}  // namespace ABULAFIA_NAMESPACE

#endif