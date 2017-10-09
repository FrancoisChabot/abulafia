//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_ADAPTER_CLEAN_FAILURE_H_
#define ABULAFIA_PARSERS_COROUTINE_ADAPTER_CLEAN_FAILURE_H_

#include "abulafia/config.h"

#include "abulafia/parsers/coroutine/dst_behavior.h"
#include "abulafia/result.h"

namespace ABULAFIA_NAMESPACE {

// This is to be used to enforce FAILS_CLEANLY on a parser
// that does not naturally meet that requirement
//
// There is NO peek method, because if the parser can peek,
// then it should have no trouble meeting FAILS_CLEANLY by itself.

template <typename CTX_T, typename DST_T, typename REQ_T,
          typename PARSER_FACTORY_T>
class CleanFailureAdapter {
 public:
  using pat_t = typename PARSER_FACTORY_T::pat_t;

  struct adapted_reqs_t : public REQ_T {
    enum { FAILS_CLEANLY = false };
  };

  using child_parser_t =
      typename PARSER_FACTORY_T::template type<CTX_T, DST_T, adapted_reqs_t>;

  CleanFailureAdapter(CTX_T ctx, DST_T dst, pat_t const& pat)
      : adapted_parser_(ctx, dst, pat) {
    ctx.data().prepare_rollback();
  }

  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    auto status = adapted_parser_.consume(ctx, dst, pat);
    switch (status) {
      case Result::SUCCESS:
        ctx.data().cancel_rollback();
        break;
      case Result::FAILURE:
        ctx.data().commit_rollback();
        break;
      case Result::PARTIAL:
        break;
    }

    return status;
  }

 private:
  child_parser_t adapted_parser_;
};

template <typename FACTORY_T>
struct CleanFailureFactoryAdapter {
  static_assert(!FACTORY_T::FAILS_CLEANLY);

  static constexpr DstBehavior dst_behavior() {
    return FACTORY_T::dst_behavior();
  }

  using pat_t = typename FACTORY_T::pat_t;

  enum {
    ATOMIC = FACTORY_T::ATOMIC,
    FAILS_CLEANLY = true,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = CleanFailureAdapter<CTX_T, DST_T, REQ_T, FACTORY_T>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif