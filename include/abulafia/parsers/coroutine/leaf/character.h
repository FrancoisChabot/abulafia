//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_CHARACTER_H_
#define ABULAFIA_PARSERS_COROUTINE_CHARACTER_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/leaf/character.h"
#include "abulafia/support/assert.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename CHARSET_T>
class Parser<CTX_T, DST_T, Char<CHARSET_T>> : public ParserBase<CTX_T, DST_T> {
  using PAT_T = Char<CHARSET_T>;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const&)
      : ParserBase<CTX_T, DST_T>(ctx, dst) {}

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    // std::cout << "char is writing at: " << std::hex << (uint64_t)&dst <<
    // std::endl;
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }

    if (ctx.empty()) {
      return ctx.final_buffer() ? result::FAILURE : result::PARTIAL;
    }

    auto next = ctx.next();
    if (pat.char_set().is_valid(next)) {
      dst = next;
      ctx.advance();
      return result::SUCCESS;
    }
    return result::FAILURE;
  }

  result peek(CTX_T const& ctx, PAT_T const& pat) const {
    if (ctx.empty()) {
      return ctx.final_buffer() ? result::FAILURE : result::PARTIAL;
    }

    return pat.char_set().is_valid(ctx.next()) ? result::SUCCESS
                                               : result::FAILURE;
  }
};
}  // namespace ABULAFIA_NAMESPACE

#endif
