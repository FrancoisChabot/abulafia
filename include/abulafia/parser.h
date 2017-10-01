//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSER_H_
#define ABULAFIA_PARSER_H_

#include "abulafia/config.h"

#include "abulafia/result.h"
#include "abulafia/patterns/leaf/fail.h"
#include "abulafia/support/nil.h"
#include "abulafia/support/type_traits.h"
#include "abulafia/parsers/coroutine/parser_factory.h"

namespace ABULAFIA_NAMESPACE {


template <typename CTX_T, typename DST_T, typename PAT_T>
using Parser = decltype(
  ParserFactory<CTX_T, DST_T, PAT_T>::create(std::declval<CTX_T>(), 
                                     std::declval<DST_T>(), 
                                     std::declval<PAT_T>()));

template <typename CTX_T, typename DST_T, typename PAT_T>
auto make_parser_(CTX_T ctx, DST_T dst, PAT_T const& pat) {
  return ParserFactory<CTX_T, DST_T, PAT_T>::create(ctx, dst, pat);
}

// Skipping is implemented as a base class so that we can leverage the empty
// base class optimization when no skipper is involved.
template <typename CTX_T, typename Enable=void>
class ParserBase;

template <typename CTX_T>
class ParserBase<CTX_T, std::enable_if_t<!CTX_T::HAS_SKIPPER>> {
public:
  constexpr ParserBase(CTX_T) {}
  constexpr result performSkip(CTX_T) { return result::SUCCESS; }
};

template <typename CTX_T>
class ParserBase<CTX_T, std::enable_if_t<CTX_T::HAS_SKIPPER>> {
  using skip_pattern_t = typename CTX_T::skip_pattern_t;
  using skipper_ctx_t = typename CTX_T::template set_skipper_t<Fail>;
  using skip_parser_t = Parser<skipper_ctx_t, Nil, skip_pattern_t>;

  skip_parser_t skipper_parser_t;
  bool skipping_done_ = false;

public:
  ParserBase(CTX_T ctx)
    : skipper_parser_t(ctx.changeSkipper(fail), nil, ctx.skipper()) {
    // TODO: Unecessary for skippers that fai cleanly
    ctx.prepare_rollback();
  }

  result performSkip(CTX_T ctx) {
    if (CTX::IS_RESUMABLE && skipping_done_) {
      return result::SUCCESS;
    }

    auto skip_ctx = ctx.changeSkipper(fail);

    while (1) {
      auto status = skip_parser_.consume(skip_ctx, nil, ctx.skipper());

      switch (status) {
      case result::PARTIAL:
        return result::PARTIAL;
      case result::FAILURE:
        ctx.commit_rollback();
        skipping_done_ = true;
        return result::SUCCESS;
      case result::SUCCESS:
        ctx.cancel_rollback();
        skip_parser_ = skip_parser_t(ctx_wrap, nil, ctx.skipPattern());
        ctx.prepare_rollback();
        break;
      }
    }
  }
};
}  // namespace ABULAFIA_NAMESPACE

#endif
