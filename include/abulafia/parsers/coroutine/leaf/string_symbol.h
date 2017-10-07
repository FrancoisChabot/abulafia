//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_STRING_SYMBOL_H_
#define ABULAFIA_PARSERS_COROUTINE_STRING_SYMBOL_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/leaf/string_symbol.h"
#include "abulafia/support/assert.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename CHAR_T, typename VAL_T>
class SymbolImpl {
  using pat_t = Symbol<CHAR_T, VAL_T>;
  using node_t = typename pat_t::node_t;

  node_t const* next_ = nullptr;
  node_t const* current_valid_ = nullptr;

 public:
  SymbolImpl(CTX_T, DST_T, pat_t const& pat) : next_(pat.root()) {}

  Result consume(CTX_T ctx, DST_T dst, pat_t const&) {
    while (1) {
      if (ctx.data().empty()) {
        if (ctx.data().final_buffer()) {
          if (current_valid_) {
            dst = *current_valid_->val;
            ctx.data().commit_rollback();
            return Result::SUCCESS;
          } else {
            return Result::FAILURE;
          }
        } else {
          // If we were conclusively done, we would have returned success
          // before looping.
          return Result::PARTIAL;
        }
      }

      auto next = ctx.data().next();
      auto found = next_->child.find(next);
      if (found == next_->child.end()) {
        // the next character leads nowhere
        if (current_valid_) {
          // we had a match along the way
          dst = *current_valid_->val;
          ctx.data().commit_rollback();
          return Result::SUCCESS;
        }
        return Result::FAILURE;
      } else {
        // consume the value
        ctx.data().advance();
        next_ = &found->second;
        if (next_->val) {
          // we got a hit!
          if (current_valid_) {
            ctx.data().cancel_rollback();
          }

          if (next_->child.empty()) {
            // nowhere to go from here
            dst = *next_->val;
            return Result::SUCCESS;
          }
          current_valid_ = next_;
          ctx.data().prepare_rollback();
        }
      }
    }
  }
};

template <typename CHAR_T, typename VAL_T>
struct ParserFactory<Symbol<CHAR_T, VAL_T>> {
  using pat_t = Symbol<CHAR_T, VAL_T>;

  static constexpr DstBehavior dst_behavior() { return DstBehavior::VALUE; }

  enum {
    ATOMIC = true,
    FAILS_CLEANLY = false,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = SymbolImpl<CTX_T, DST_T, CHAR_T, VAL_T>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
