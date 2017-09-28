//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_SEQUENCE_H_
#define ABULAFIA_PARSERS_COROUTINE_SEQUENCE_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/nary/sequence.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename... CHILD_PATS_T>
class Parser<CTX_T, DST_T, Seq<CHILD_PATS_T...>>
    : public ParserBase<CTX_T, DST_T> {
  using PAT_T = Seq<CHILD_PATS_T...>;
  using childs_tuple_t = std::tuple<CHILD_PATS_T...>;

  using child_parsers_t = typename seq_::SeqSubParser<
      CTX_T, DST_T, childs_tuple_t,
      std::index_sequence_for<CHILD_PATS_T...>>::type;
  child_parsers_t child_parsers_;

 public:
  template <std::size_t ID>
  decltype(auto) getDstFor(DST_T& dst) {
    using accessor_t =
        seq_::choose_dst_accessor<ID, CTX_T, DST_T, childs_tuple_t>;

    return accessor_t::access(dst);
  }

  Parser(CTX_T& ctx, DST_T& dst, PAT_T const& pat)
      : ParserBase<CTX_T, DST_T>(ctx, dst),
        child_parsers_(
            std::in_place_index_t<0>(),
            std::variant_alternative_t<0, child_parsers_t>(
                ctx, force_lvalue(getDstFor<0>(dst)), getChild<0>(pat))) {
    reset_if_collection<DST_T>::exec(dst);
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

    result child_res =
        c_parser.consume(ctx, force_lvalue(getDstFor<ID>(dst)), c_pattern);

    if (result::SUCCESS == child_res) {
      constexpr int next_id = ID + 1;
      // if we have reached the end of the child parsers list
      if (sizeof...(CHILD_PATS_T) == next_id) {
        return result::SUCCESS;
      } else {
        // This does not matter nearly that much, as we will never enter
        // here with a saturated value
        // The sole purpose of the max is to prevent reset<N>() from
        // being called with an out of bounds value,
        // which would cause a compile-time error.
        constexpr int new_id = next_id < sizeof...(CHILD_PATS_T) ? next_id : 0;
        auto const& new_c_pattern = getChild<new_id>(pat);

        child_parsers_ = child_parsers_t(
            std::in_place_index_t<new_id>(),
            make_parser_(ctx, force_lvalue(getDstFor<new_id>(dst)),
                         new_c_pattern));

        return consume_from<new_id>(ctx, dst, pat);
      }
    }
    return child_res;
  }
};
}  // namespace ABULAFIA_NAMESPACE

#endif
