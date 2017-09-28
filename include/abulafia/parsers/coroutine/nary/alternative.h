//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_ALTERNATIVE_H_
#define ABULAFIA_PARSERS_COROUTINE_ALTERNATIVE_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/nary/alternative.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename... CHILD_PATS_T>
class Parser<CTX_T, DST_T, Alt<CHILD_PATS_T...>>
    : public ParserBase<CTX_T, DST_T> {
  using PAT_T = Alt<CHILD_PATS_T...>;
  using child_pat_tuple_t = typename PAT_T::child_tuple_t;
  using child_parsers_t = std::variant<Parser<CTX_T, DST_T, CHILD_PATS_T>...>;

  child_parsers_t child_parsers_;

 public:
  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst),
        child_parsers_(std::in_place_index_t<0>(), ctx, dst, getChild<0>(pat)) {
    if (rolls_back_at<0>()) {
      ctx.prepare_rollback();
    }
  }

  result consume(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    if (this->performSkip(ctx) == result::PARTIAL) {
      return result::PARTIAL;
    }

    if (CTX_T::IS_RESUMABLE) {
      return visit_val<sizeof...(CHILD_PATS_T)>(
          child_parsers_.index(),
          [&](auto N) { return this->consume_from<N()>(ctx, dst, pat); });
    } else {
      // Skip the visitation when using non_resumable parsers.
      return consume_from<0>(ctx, dst, pat);
    }
  }

  template <std::size_t ID>
  result consume_from(CTX_T& ctx, DST_T& dst, PAT_T const& pat) {
    abu_assume(child_parsers_.index() == ID);

    auto& c_parser = std::get<ID>(child_parsers_);
    auto const& c_pattern = getChild<ID>(pat);

    result child_res = c_parser.consume(ctx, dst, c_pattern);

    if (result::FAILURE == child_res) {
      if (rolls_back_at<ID>()) {
        ctx.commit_rollback();
      }
      constexpr int next_id = ID + 1;

      // if we have reached the end of the child parsers list
      if (sizeof...(CHILD_PATS_T) == next_id) {
        return result::FAILURE;
      } else {
        constexpr int new_id = next_id < sizeof...(CHILD_PATS_T) ? next_id : 0;
        auto const& new_c_pattern = getChild<new_id>(pat);

        child_parsers_ = child_parsers_t(std::in_place_index_t<new_id>(),
                                         make_parser_(ctx, dst, new_c_pattern));

        if (rolls_back_at<new_id>()) {
          ctx.prepare_rollback();
        }
        return consume_from<new_id>(ctx, dst, pat);
      }
    }

    if (child_res == result::SUCCESS) {
      if (rolls_back_at<ID>()) {
        ctx.cancel_rollback();
      }
    }
    return child_res;
  }

 private:
  template <std::size_t N>
  static constexpr bool rolls_back_at() {
    return !pattern_traits<std::tuple_element_t<N, child_pat_tuple_t>,
                           void>::FAILS_CLEANLY;
  }
};
}  // namespace ABULAFIA_NAMESPACE

#endif
