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
#include "abulafia/support/visit_val.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename REQ_T,
          typename... CHILD_PATS_T>
class AltImpl {
  using pat_t = Alt<CHILD_PATS_T...>;

  struct child_req_t : public REQ_T {
    enum { FAILS_CLEANLY = true };
  };

  using child_parsers_t =
      std::variant<Parser<CTX_T, DST_T, child_req_t, CHILD_PATS_T>...>;

  child_parsers_t child_parsers_;

 public:
  AltImpl(CTX_T& ctx, DST_T& dst, pat_t const& pat)
      : child_parsers_(std::in_place_index_t<0>(), ctx, dst, getChild<0>(pat)) {
  }

  Result consume(CTX_T& ctx, DST_T& dst, pat_t const& pat) {
    if (ctx.data().isResumable()) {
      return visit_val<sizeof...(CHILD_PATS_T)>(
          child_parsers_.index(),
          [&](auto N) { return this->consume_from<N()>(ctx, dst, pat); });
    } else {
      // Skip the visitation when using non_resumable parsers.
      return consume_from<0>(ctx, dst, pat);
    }
  }

  template <std::size_t ID>
  Result consume_from(CTX_T& ctx, DST_T& dst, pat_t const& pat) {
    abu_assume(child_parsers_.index() == ID);

    auto& c_parser = std::get<ID>(child_parsers_);
    auto const& c_pattern = getChild<ID>(pat);

    Result child_res = c_parser.consume(ctx, dst, c_pattern);

    if (Result::FAILURE == child_res) {
      constexpr int next_id = ID + 1;

      // if we have reached the end of the child parsers list
      if (sizeof...(CHILD_PATS_T) == next_id) {
        return Result::FAILURE;
      } else {
        constexpr int new_id = next_id < sizeof...(CHILD_PATS_T) ? next_id : 0;
        auto const& new_c_pattern = getChild<new_id>(pat);

        child_parsers_ = child_parsers_t(std::in_place_index_t<new_id>(), ctx,
                                         dst, new_c_pattern);

        return consume_from<new_id>(ctx, dst, pat);
      }
    }
    return child_res;
  }
};

template <typename... CHILD_PATS_T>
struct ParserFactory<Alt<CHILD_PATS_T...>> {
  using pat_t = Alt<CHILD_PATS_T...>;

  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = AltImpl<CTX_T, DST_T, REQ_T, CHILD_PATS_T...>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
