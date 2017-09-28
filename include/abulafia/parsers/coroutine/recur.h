//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_RECUR_H_
#define ABULAFIA_PARSERS_COROUTINE_RECUR_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/recur.h"

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename CHILD_PAT_T, typename ATTR_T>
class Parser<CTX_T, DST_T, Recur<CHILD_PAT_T, ATTR_T>>
    : public ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP> {
  using PAT_T = Recur<CHILD_PAT_T, ATTR_T>;
  using operand_pat_t = typename PAT_T::operand_pat_t;
  using operand_parser_t = Parser<CTX_T, DST_T, operand_pat_t>;

  std::unique_ptr<operand_parser_t> child_parser_;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const&)
      : ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP>(ctx, dst) {
    // We do not create the child parser here, since this is a recursive
    // process.
  }

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (!child_parser_) {
      child_parser_ =
          std::make_unique<operand_parser_t>(ctx, dst, pat.operand());
    }
    return child_parser_->consume(ctx, dst, pat.operand());
  }
};

template <typename CTX_T, typename DST_T, typename CHILD_PAT_T, typename ATTR_T>
class Parser<CTX_T, DST_T, WeakRecur<CHILD_PAT_T, ATTR_T>>
    : public ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP> {
  using PAT_T = WeakRecur<CHILD_PAT_T, ATTR_T>;
  using operand_pat_t = typename PAT_T::operand_pat_t;
  using operand_parser_t = Parser<CTX_T, DST_T, operand_pat_t>;

  std::unique_ptr<operand_parser_t> child_parser_;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const&)
      : ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP>(ctx, dst) {
    // We do not create the child parser here, since this is a recursive
    // process.
  }

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (!child_parser_) {
      child_parser_ =
          std::make_unique<operand_parser_t>(ctx, dst, pat.operand());
    }
    return child_parser_->consume(ctx, dst, pat.operand());
  }
};
}  // namespace ABULAFIA_NAMESPACE

#endif
