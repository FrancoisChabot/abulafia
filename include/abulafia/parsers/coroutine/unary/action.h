//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_ACTION_H_
#define ABULAFIA_PARSERS_COROUTINE_ACTION_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/unary/action.h"

namespace ABULAFIA_NAMESPACE {
  /*
template <typename CTX_T, typename DST_T, typename CHILD_PAT_T, typename ACT_T>
class Parser<CTX_T, DST_T, Action<CHILD_PAT_T, ACT_T>>
    : public ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP> {
  using PAT_T = Action<CHILD_PAT_T, ACT_T>;

  using child_ctx_t = CTX_T;
  using landing_type_t = typename act_::determine_landing_type<ACT_T>::type;
  using child_parser_t = Parser<child_ctx_t, landing_type_t, CHILD_PAT_T>;

  landing_type_t landing;
  child_parser_t child_parser_;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T, PARSER_OPT_NO_SKIP>(ctx, dst),
        child_parser_(ctx, landing, pat.child_pattern()) {}

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    auto status = child_parser_.consume(ctx, landing, pat.child_pattern());
    if (status == result::SUCCESS) {
      act_::Dispatch<ACT_T>::template dispatch(pat.action(), std::move(landing),
                                               dst);
    }
    return status;
  }
};
*/
}  // namespace ABULAFIA_NAMESPACE

#endif
