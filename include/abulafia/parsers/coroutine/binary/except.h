//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_EXCEPT_H_
#define ABULAFIA_PARSERS_COROUTINE_EXCEPT_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/binary/except.h"
#include "abulafia/support/assert.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename REQ_T, typename OP_T,
          typename NEG_T>
class ExceptImpl {
  using pat_t = Except<OP_T, NEG_T>;

  struct op_req_t : public REQ_T {
    enum {
      // We don't care
      FAILS_CLEANLY = false,
    };
  };

  struct neg_req_t {
    enum {
      ATOMIC = false,
      FAILS_CLEANLY = true,
      CONSUMES_ON_SUCCESS = false,
    };
  };

  using op_parser_t = Parser<CTX_T, DST_T, op_req_t, OP_T>;
  using neg_parser_t = Parser<CTX_T, Nil, neg_req_t, NEG_T>;

  using child_parsers_t = std::variant<neg_parser_t, op_parser_t>;
  child_parsers_t child_parsers_;

 public:
  ExceptImpl(CTX_T ctx, DST_T, pat_t const& pat)
      : child_parsers_(std::in_place_index_t<0>(), ctx, nil, pat.neg()) {}

  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    if (child_parsers_.index() == 0) {
      auto res = std::get<0>(child_parsers_).consume(ctx, nil, pat.neg());

      switch (res) {
        case Result::PARTIAL:
          return Result::PARTIAL;
        case Result::SUCCESS:
          return Result::FAILURE;
        case Result::FAILURE:
          child_parsers_ =
              child_parsers_t(std::in_place_index_t<1>(), ctx, dst, pat.op());
      }
    }
    abu_assume(child_parsers_.index() == 1);
    return std::get<1>(child_parsers_).consume(ctx, dst, pat.op());
  }
};

template <typename OP_T, typename NEG_T>
struct ParserFactory<Except<OP_T, NEG_T>> {
  using pat_t = Except<OP_T, NEG_T>;

  static constexpr DstBehavior dst_behavior() { return ParserFactory<OP_T>::dst_behavior(); }

  enum {
    ATOMIC = true,
    FAILS_CLEANLY = false,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = ExceptImpl<CTX_T, DST_T, REQ_T, OP_T, NEG_T>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
