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

struct RecurChildReqs {
  enum { ATOMIC = false, FAILS_CLEANLY = false, CONSUMES_ON_SUCCESS = false };
};

template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T,
          typename ATTR_T>
class RecurImpl {
  using pat_t = Recur<CHILD_PAT_T, ATTR_T>;
  using operand_pat_t = typename pat_t::operand_pat_t::impl_t;

  using operand_parser_t = Parser<CTX_T, DST_T, RecurChildReqs, operand_pat_t>;

  std::unique_ptr<operand_parser_t> child_parser_;

 public:
  RecurImpl(CTX_T, DST_T, pat_t const&) {
    // We do not create the child parser here, since this is a recursive
    // process.
  }

  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    if (!child_parser_) {
      child_parser_ =
          std::make_unique<operand_parser_t>(ctx, dst, pat.operand().impl);
    }
    return child_parser_->consume(ctx, dst, pat.operand().impl);
  }
};

template <typename CHILD_PAT_T, typename ATTR_T>
struct ParserFactory<Recur<CHILD_PAT_T, ATTR_T>> {
  static_assert(is_pattern<typename CHILD_PAT_T::impl_t>());
  using pat_t = Recur<CHILD_PAT_T, ATTR_T>;

  static constexpr DstBehavior dst_behavior() {
    if (std::is_same<Nil, ATTR_T>::value) {
      return DstBehavior::IGNORE;
    }

    return DstBehavior::VALUE;
  }

  enum {
    ATOMIC = false,
    FAILS_CLEANLY = false,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = RecurImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T, ATTR_T>;
};

// Very special case to break up the recursive cycle. Any requirement will be
// applied on the recur pattern proper.
template <typename CTX_T, typename DST_T, typename PAT_T>
struct AdaptedParserFactory<CTX_T, DST_T, RecurChildReqs, PAT_T> {
  static RecurParserChildImpl<CTX_T, DST_T, RecurChildReqs, PAT_T> create(
      CTX_T ctx, DST_T dst, PAT_T const& pat) {
    return RecurParserChildImpl<CTX_T, DST_T, RecurChildReqs, PAT_T>(ctx, dst,
                                                                     pat);
  }
};
}  // namespace ABULAFIA_NAMESPACE

#endif
