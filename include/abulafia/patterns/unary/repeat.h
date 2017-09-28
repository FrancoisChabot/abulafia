//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_UNARY_REPEAT_H_
#define ABULAFIA_PATTERNS_UNARY_REPEAT_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/pattern.h"
#include "abulafia/patterns/helpers/buffer.h"
#include "abulafia/support/nil.h"

namespace ABULAFIA_NAMESPACE {

template <typename PAT_T, std::size_t MIN_REP, std::size_t MAX_REP>
class Repeat : public Pattern<Repeat<PAT_T, MIN_REP, MAX_REP>> {
  PAT_T operand_;

 public:
  Repeat(const PAT_T& op) : operand_(op) {}
  Repeat(PAT_T&& op) : operand_(std::move(op)) {}

  PAT_T const& operand() const { return operand_; }
};

template <typename CTX_T, typename DST_T, typename CHILD_PAT_T,
          std::size_t MIN_REP, std::size_t MAX_REP>
class Parser<CTX_T, DST_T, Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>>
    : public ParserBase<CTX_T, DST_T> {
  using PAT_T = Repeat<CHILD_PAT_T, MIN_REP, MAX_REP>;
  using child_adapter_t =
      buf_::CollectionParserAdapter<CTX_T, DST_T, CHILD_PAT_T>;

  std::size_t count_ = 0;
  child_adapter_t child_parser_;

  enum { needs_backtrack = !pattern_traits<PAT_T, void>::FAILS_CLEANLY };

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst),
        child_parser_(ctx, dst, pat.operand()) {
    dst.clear();
    if (needs_backtrack) {
      ctx.prepare_rollback();
    }
  }

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }

    while (1) {
      auto child_res = child_parser_.consume(ctx, dst, pat.operand());
      switch (child_res) {
        case result::FAILURE:
          // cancel the child's data consumption
          // Technically, we could cancel the rollback in the failure branch,
          // but guaranteeing FAILS_CLEANLY is better.
          if (needs_backtrack) {
            ctx.commit_rollback();
          }
          if (count_ >= MIN_REP) {
            return result::SUCCESS;
          } else {
            return result::FAILURE;
          }

        case result::PARTIAL:
          return result::PARTIAL;
        case result::SUCCESS:
          count_++;
          if (needs_backtrack) {
            ctx.cancel_rollback();
          }

          if (MAX_REP != 0 && count_ == MAX_REP) {
            return result::SUCCESS;
          }

          if (needs_backtrack) {
            ctx.prepare_rollback();
          }

          // If we are still going, then we need to reset the child's parser
          child_parser_ = child_adapter_t(ctx, dst, pat.operand());
      }
    }
  }
};

template <std::size_t MIN_REP = 0, std::size_t MAX_REP = 0, typename PAT_T>
inline auto repeat(PAT_T&& pat) {
  return Repeat<pattern_t<PAT_T>, MIN_REP, MAX_REP>(
      make_pattern(forward<PAT_T>(pat)));
}

template <std::size_t MIN_REP = 0, std::size_t MAX_REP = 0, typename PAT_T,
          typename CB_T>
auto transform(Repeat<PAT_T, MIN_REP, MAX_REP> const& tgt, CB_T const& cb) {
  return repeat(cb(tgt.operand()));
}

namespace repeat_ {
template <typename PAT_T, typename CTX_T, typename Enable = void>
struct extract_value_type;

// *P is still vector<p::attr_t>
template <typename PAT_T, typename CTX_T>
struct extract_value_type<
    PAT_T, CTX_T, enable_if_t<!pattern_traits<PAT_T, void>::APPENDS_DST>> {
  using type = attr_t<PAT_T, CTX_T>;
};

// next,  **P is still vector<p::attr_t>
template <typename PAT_T, typename CTX_T>
struct extract_value_type<
    PAT_T, CTX_T, enable_if_t<pattern_traits<PAT_T, void>::APPENDS_DST>> {
  using attr_t_type = attr_t<PAT_T, CTX_T>;
  using type = typename attr_t_type::value_type;
};

}  // namespace repeat_

template <typename PAT_T, std::size_t MIN_REP, std::size_t MAX_REP,
          typename RECUR_TAG>
struct pattern_traits<Repeat<PAT_T, MIN_REP, MAX_REP>, RECUR_TAG>
    : public default_pattern_traits {
  static_assert(pattern_traits<PAT_T, RECUR_TAG>::MAY_NOT_CONSUME == false ||
                    MAX_REP != 0,
                "invalid repeat pattern, infinite loop");
  enum {
    ATOMIC = false,
    BACKTRACKS = pattern_traits<PAT_T, RECUR_TAG>::BACKTRACKS ||
                 !pattern_traits<PAT_T, RECUR_TAG>::FAILS_CLEANLY,
    FAILS_CLEANLY = true,
    MAY_NOT_CONSUME =
        MIN_REP == 0 || pattern_traits<PAT_T, RECUR_TAG>::MAY_NOT_CONSUME,

    PEEKABLE = MIN_REP == 0 ||
               (MIN_REP == 1 && pattern_traits<PAT_T, RECUR_TAG>::PEEKABLE),
    APPENDS_DST = true,
    STABLE_APPENDS = MIN_REP <= 1
  };
};

template <typename PAT_T, std::size_t MIN_REP, std::size_t MAX_REP,
          typename CTX_T>
struct pat_attr_t<Repeat<PAT_T, MIN_REP, MAX_REP>, CTX_T> {
  using value_type = typename repeat_::extract_value_type<PAT_T, CTX_T>::type;
  using attr_type =
      typename std::conditional<is_same<Nil, value_type>::value, Nil,
                                std::vector<value_type>>::type;
};
}  // namespace ABULAFIA_NAMESPACE

#endif
