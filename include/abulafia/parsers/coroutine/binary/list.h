//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_LIST_H_
#define ABULAFIA_PARSERS_COROUTINE_LIST_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/binary/list.h"
#include "abulafia/support/assert.h"
#include "abulafia/support/nil.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename REQ_T, typename OP_T,
          typename SEP_T>
class ListImpl {
  using pat_t = List<OP_T, SEP_T>;

  struct op_req_t : public DefaultReqs {
    enum {
      ATOMIC = true,
      // We don't care
      FAILS_CLEANLY = false,
    };
  };

  struct sep_req_t : public DefaultReqs {
    enum {
      ATOMIC = false,
      FAILS_CLEANLY = false,
    };
  };

  using op_parser_t = Parser<CTX_T, DST_T, op_req_t, OP_T>;
  using sep_parser_t = Parser<CTX_T, Nil, sep_req_t, SEP_T>;

  using child_parsers_t = std::variant<op_parser_t, sep_parser_t>;
  child_parsers_t child_parsers_;

 public:
  ListImpl(CTX_T ctx, DST_T dst, pat_t const& pat)
      : child_parsers_(std::in_place_index_t<0>(), ctx, dst, pat.op()) {
    ctx.data().prepare_rollback();
  }

  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    while (1) {
      if (child_parsers_.index() == 0) {
        // We are parsing a value.
        auto child_res =
            std::get<0>(child_parsers_).consume(ctx, dst, pat.op());
        switch (child_res) {
          case Result::SUCCESS: {
            ctx.data().cancel_rollback();
            ctx.data().prepare_rollback();

            child_parsers_ = child_parsers_t(std::in_place_index_t<1>(), ctx,
                                             nil, pat.sep());
          } break;
          case Result::FAILURE:
            // this will cancel the consumption of the separator if there was
            // any
            ctx.data().commit_rollback();
            return Result::SUCCESS;
          case Result::PARTIAL:
            return Result::PARTIAL;
        }
      } else {
        abu_assume(child_parsers_.index() == 1);
        // We are parsing a separator
        auto child_res =
            std::get<1>(child_parsers_).consume(ctx, nil, pat.sep());
        switch (child_res) {
          case Result::SUCCESS:
            child_parsers_ =
                child_parsers_t(std::in_place_index_t<0>(), ctx, dst, pat.op());
            break;
          case Result::FAILURE:
            // rollback whatever the separator may have eaten
            ctx.data().commit_rollback();
            return Result::SUCCESS;
          case Result::PARTIAL:
            return Result::PARTIAL;
        }
      }
    }
  }
};

template <typename OP_T, typename SEP_T>
struct ParserFactory<List<OP_T, SEP_T>> {
  using pat_t = List<OP_T, SEP_T>;

  static constexpr DstBehavior dst_behavior() { return ParserFactory<OP_T>::dst_behavior(); }

  enum {
    ATOMIC = false,
    FAILS_CLEANLY = true,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = ListImpl<CTX_T, DST_T, REQ_T, OP_T, SEP_T>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
