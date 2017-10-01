//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_ADAPTER_CLEAN_FAILURE_H_
#define ABULAFIA_PARSERS_COROUTINE_ADAPTER_CLEAN_FAILURE_H_

#include "abulafia/config.h"

namespace ABULAFIA_NAMESPACE {

// This is to be used to enforce FAILS_CLEANLY on a parser
// that does not naturally meet that requirement
//
// There is NO peek method, because if the parser can peek,
// then it should have no trouble meeting FAILS_CLEANLY by itself.

template<typename PARSER_T>
class CleanFailureAdapter {
public:
  using ctx_t = typename PARSER_T::ctx_t;
  using dst_t = typename PARSER_T::dst_t;
  using pat_t = typename PARSER_T::pat_t;

  CleanFailureAdapter(ctx_t ctx, dst_t dst, pat_t const& pat)
    : adapted_parser_(ctx, dst, pat) {
    ctx.data().prepare_rollback();
  }

  result consume(ctx_t ctx, dst_t dst, pat_t const& pat) {
    auto status = adapted_parser_.consume(ctx, dst, pat);
    switch (status) {
    case result::SUCCESS:
      ctx.data().cancel_rollback();
      break;
    case result::FAILURE:
      ctx.data().commit_rollback();
      break;
    }

    return status;
  }

private:
  PARSER_T adapted_parser_;
};

}  // namespace ABULAFIA_NAMESPACE

#endif