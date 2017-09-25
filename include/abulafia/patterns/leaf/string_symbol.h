//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_STRING_SYMBOL_H_
#define ABULAFIA_PATTERNS_STRING_SYMBOL_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/pattern.h"

#include <map>
#include <optional>
#include <stdexcept>

namespace ABULAFIA_NAMESPACE {

template <typename CHAR_T, typename VAL_T>
class Symbol : public Pattern<Symbol<CHAR_T, VAL_T>> {
  // symbols->value map will be stored as a trie
  struct Node {
    std::map<CHAR_T, Node> child;
    std::optional<VAL_T> val;
  };

  Node root_;

 public:
  using node_t = Node;

  Symbol(std::map<std::basic_string<CHAR_T>, VAL_T> const& vals) {
    for (auto const& entry : vals) {
      node_t* next = &root_;
      for (auto const& chr : entry.first) {
        next = &next->child[chr];
      }

      if (next->val) {
        throw std::runtime_error("cannot have two symbols with the same value");
      }

      next->val = entry.second;
    }
  }

  Node const* root() const { return &root_; }
};

template <typename CHAR_T, typename VAL_T>
auto symbol(std::map<std::basic_string<CHAR_T>, VAL_T> const& vals) {
  return Symbol<CHAR_T, VAL_T>(vals);
}

template <typename CHAR_T, typename VAL_T, typename RECUR_TAG>
struct pattern_traits<Symbol<CHAR_T, VAL_T>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    ATOMIC = true,
    BACKTRACKS = true,
    PEEKABLE = false,
    FAILS_CLEANLY = false,
    MAY_NOT_CONSUME = false,
  };
};

template <typename CHAR_T, typename VAL_T, typename CTX_T>
struct pat_attr_t<Symbol<CHAR_T, VAL_T>, CTX_T> {
  using attr_type = VAL_T;
};

template <typename CTX_T, typename DST_T, typename CHAR_T, typename VAL_T>
class Parser<CTX_T, DST_T, Symbol<CHAR_T, VAL_T>>
    : public ParserBase<CTX_T, DST_T> {
  using PAT_T = Symbol<CHAR_T, VAL_T>;
  using node_t = typename PAT_T::node_t;

  node_t const* next_ = nullptr;
  node_t const* current_valid_ = nullptr;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst), next_(pat.root()) {}

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const&) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }

    while (1) {
      if (ctx.empty()) {
        if (ctx.final_buffer()) {
          if (current_valid_) {
            dst = *current_valid_->val;
            ctx.commit_rollback();
            return result::SUCCESS;
          } else {
            return result::FAILURE;
          }
        } else {
          // If we were conclusively done, we would have returned success
          // before looping.
          return result::PARTIAL;
        }
      }

      auto next = ctx.next();
      auto found = next_->child.find(next);
      if (found == next_->child.end()) {
        // the next character leads nowhere
        if (current_valid_) {
          // we had a match along the way
          dst = *current_valid_->val;
          ctx.commit_rollback();
          return result::SUCCESS;
        }
        return result::FAILURE;
      } else {
        // consume the value
        ctx.advance();
        next_ = &found->second;
        if (next_->val) {
          // we got a hit!
          if (current_valid_) {
            ctx.cancel_rollback();
          }

          if (next_->child.empty()) {
            // nowhere to go from here
            dst = *next_->val;
            return result::SUCCESS;
          }
          current_valid_ = next_;
          ctx.prepare_rollback();
        }
      }
    }
  }
};

}  // namespace ABULAFIA_NAMESPACE

#endif
