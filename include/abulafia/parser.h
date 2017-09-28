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
#include "abulafia/support/nil.h"
#include "abulafia/support/type_traits.h"
namespace ABULAFIA_NAMESPACE {

enum { PARSER_OPT_NO_SKIP = 1 };

template <typename CTX_T, typename DST_T, typename PAT_T>
class Parser : public Parser<CTX_T, DST_T, typename PAT_T::pattern_t> {
  using Parser<CTX_T, DST_T, typename PAT_T::pattern_t>::Parser;
  using Parser<CTX_T, DST_T, typename PAT_T::pattern_t>::operator=;
};

template <typename CTX_T, typename DST_T, typename PAT_T>
auto make_parser_(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
  return Parser<CTX_T, DST_T, PAT_T>(ctx, dst, pat);
}

// Skipping is implemented as a base class so that we can leverage the empty
// base class optimization when no skipper is involved.
template <typename CTX_T, typename DST_T, int OPT_V = 0, typename Enable = void>
class ParserBase;

template <typename CTX_T, typename DST_T, int OPT_V>
class ParserBase<CTX_T, DST_T, OPT_V,
                 std::enable_if_t<!CTX_T::HAS_SKIPPER ||
                                  (OPT_V & PARSER_OPT_NO_SKIP) != 0>> {
 public:
  constexpr ParserBase(CTX_T&, DST_T&) {}
  constexpr result performSkip(CTX_T&) { return result::SUCCESS; }
};

template <typename PARENT_CTX_T>
class SkipperContext {
  PARENT_CTX_T& parent_ctx_;

 public:
  SkipperContext(PARENT_CTX_T& parent) : parent_ctx_(parent) {}

  using value_type = typename PARENT_CTX_T::value_type;
  using base_ctx_t = typename PARENT_CTX_T::base_ctx_t;

  enum { HAS_SKIPPER = false, IS_RESUMABLE = PARENT_CTX_T::IS_RESUMABLE };

  bool final_buffer() const { return parent_ctx_.final_buffer(); }

  void prepare_rollback() { parent_ctx_.prepare_rollback(); }

  void commit_rollback() { parent_ctx_.commit_rollback(); }

  void cancel_rollback() { parent_ctx_.cancel_rollback(); }

  value_type next() const { return parent_ctx_.next(); }

  void advance() { parent_ctx_.advance(); }

  bool empty() const { return parent_ctx_.empty(); }
};

template <typename CTX_T, typename DST_T, int OPT_V>
class ParserBase<
    CTX_T, DST_T, OPT_V,
    std::enable_if_t<CTX_T::HAS_SKIPPER && (OPT_V & PARSER_OPT_NO_SKIP) == 0>> {
 public:
  using skip_pattern_t = typename CTX_T::skip_pattern_t;
  using skip_parser_t = Parser<SkipperContext<CTX_T>, Nil, skip_pattern_t>;

  ParserBase(CTX_T& ctx, DST_T&)
      : skip_parser_(force_lvalue(SkipperContext<CTX_T>(ctx)), nil,
                     ctx.skipPattern()) {
    ctx.prepare_rollback();
  }

  result performSkip(CTX_T& ctx) {
    if (skip_done_) {
      return result::SUCCESS;
    }

    SkipperContext<CTX_T> ctx_wrap(ctx);
    result success = result::SUCCESS;

    do {
      success = skip_parser_.consume(ctx_wrap, nil, ctx.skipPattern());
      switch (success) {
        case result::PARTIAL:
          return result::PARTIAL;
        case result::FAILURE:
          ctx.commit_rollback();
          skip_done_ = true;
          return result::SUCCESS;
        case result::SUCCESS:
          ctx.cancel_rollback();
          skip_parser_ = skip_parser_t(ctx_wrap, nil, ctx.skipPattern());
          ctx.prepare_rollback();
          break;
      }
    } while (success == result::SUCCESS);

    return result::FAILURE;
  }

 private:
  bool skip_done_ = false;
  skip_parser_t skip_parser_;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
