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
#include "abulafia/support/visit_val.h"

#include <variant>

namespace ABULAFIA_NAMESPACE {

namespace seq_ {

template <int PAT_ID, typename CHILDS_TUPLE_T>
constexpr bool child_ignores() {
  using pattern_t = tuple_element_t<PAT_ID == -1 ? 0 : PAT_ID, CHILDS_TUPLE_T>;
  using pattern_factory_t = ParserFactory<pattern_t>;
  return pattern_factory_t::dst_behavior() == DstBehavior::IGNORE;
}

template <int PAT_ID, typename CHILDS_TUPLE_T, typename Enable = void>
struct choose_tuple_index;

template <typename CHILDS_TUPLE_T>
struct choose_tuple_index<-1, CHILDS_TUPLE_T> {
  enum { value = -1, next_val = 0 };
};

template <int PAT_ID, typename CHILDS_TUPLE_T>
struct choose_tuple_index<
    PAT_ID, CHILDS_TUPLE_T,
    enable_if_t<(PAT_ID != -1 && child_ignores<PAT_ID, CHILDS_TUPLE_T>())>> {
  enum {
    value = -1,
    next_val = choose_tuple_index<PAT_ID - 1, CHILDS_TUPLE_T>::next_val
  };
};

template <int PAT_ID, typename CHILDS_TUPLE_T>
struct choose_tuple_index<
    PAT_ID, CHILDS_TUPLE_T,
    enable_if_t<(PAT_ID != -1 && !child_ignores<PAT_ID, CHILDS_TUPLE_T>())>> {
  enum {
    value = choose_tuple_index<PAT_ID - 1, CHILDS_TUPLE_T>::next_val,
    next_val = value + 1
  };
};

template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
struct choose_dst_accessor {
  using pattern_t = tuple_element_t<PAT_ID, CHILDS_TUPLE_T>;
  using pattern_factory_t = ParserFactory<pattern_t>;

  static auto access(DST_T dst) { 
    if constexpr (std::is_same<Nil, DST_T>::value ||
      pattern_factory_t::dst_behavior() == DstBehavior::IGNORE) {
      (void)dst;
      return nil;
    }
    else if constexpr (is_tuple<typename DST_T::dst_type>::value) {
      constexpr int dst_index = choose_tuple_index<PAT_ID, CHILDS_TUPLE_T>::value;
      return wrap_dst(std::get<dst_index>(dst.get()));
    }
    else {
      return dst;
    }
  }
  using type = decltype(access(std::declval<DST_T>()));
};

template <std::size_t PAT_ID, typename CTX_T, typename DST_T, typename REQ_T,
          typename CHILDS_TUPLE_T>
struct WrappedParser {
  using dst_t =
      typename choose_dst_accessor<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>::type;
  using type =
      Parser<CTX_T, dst_t, REQ_T, tuple_element_t<PAT_ID, CHILDS_TUPLE_T>>;
};

template <std::size_t PAT_ID, typename CTX_T, typename DST_T, typename REQ_T,
          typename CHILDS_TUPLE_T>
using WrappedParser_t =
    typename WrappedParser<PAT_ID, CTX_T, DST_T, REQ_T, CHILDS_TUPLE_T>::type;

template <typename CTX_T, typename DST_T, typename REQ_T,
          typename CHILDS_TUPLE_T, typename INDEX_SEQ>
struct SeqSubParser;

template <typename CTX_T, typename DST_T, typename REQ_T,
          typename CHILDS_TUPLE_T, std::size_t... PAT_IDS>
struct SeqSubParser<CTX_T, DST_T, REQ_T, CHILDS_TUPLE_T,
                    std::index_sequence<PAT_IDS...>> {
  using test_test = std::index_sequence<PAT_IDS...>;
  using type = std::variant<
      WrappedParser_t<PAT_IDS, CTX_T, DST_T, REQ_T, CHILDS_TUPLE_T>...>;
};
}  // namespace seq_

template <typename CTX_T, typename DST_T, typename REQ_T,
          typename... CHILD_PATS_T>
class SeqImpl {
  using pat_t = Seq<CHILD_PATS_T...>;

  struct child_req_t : public REQ_T {
    enum { CONSUMES_ON_SUCCESS = false, ATOMIC = false, FAILS_CLEANLY = false };
  };

  using childs_tuple_t = typename pat_t::child_tuple_t;
  using child_parsers_t = typename seq_::SeqSubParser<
      CTX_T, DST_T, child_req_t, childs_tuple_t,
      std::index_sequence_for<CHILD_PATS_T...>>::type;
  child_parsers_t child_parsers_;

 public:
  template <std::size_t ID>
  decltype(auto) getDstFor(DST_T dst) {
    using accessor_t =
        seq_::choose_dst_accessor<ID, CTX_T, DST_T, childs_tuple_t>;

    return accessor_t::access(dst);
  }

  SeqImpl(CTX_T ctx, DST_T dst, pat_t const& pat)
      : child_parsers_(std::in_place_index_t<0>(),
                       std::variant_alternative_t<0, child_parsers_t>(
                           ctx, getDstFor<0>(dst), getChild<0>(pat))) {
    //    reset_if_collection<DST_T>::exec(dst);
  }

  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
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
  Result consume_from(CTX_T ctx, DST_T dst, pat_t const& pat) {
    abu_assume(child_parsers_.index() == ID);

    auto& c_parser = std::get<ID>(child_parsers_);
    auto const& c_pattern = getChild<ID>(pat);

    Result child_res = c_parser.consume(ctx, getDstFor<ID>(dst), c_pattern);

    if (Result::SUCCESS == child_res) {
      constexpr int next_id = ID + 1;
      // if we have reached the end of the child parsers list
      if (sizeof...(CHILD_PATS_T) == next_id) {
        return Result::SUCCESS;
      } else {
        // This does not matter nearly that much, as we will never enter
        // here with a saturated value
        // The sole purpose of the max is to prevent reset<N>() from
        // being called with an out of bounds value,
        // which would cause a compile-time error.
        constexpr int new_id = next_id < sizeof...(CHILD_PATS_T) ? next_id : 0;
        auto const& new_c_pattern = getChild<new_id>(pat);

        child_parsers_ = child_parsers_t(std::in_place_index_t<new_id>(), ctx,
                                         getDstFor<new_id>(dst), new_c_pattern);

        return consume_from<new_id>(ctx, dst, pat);
      }
    }
    return child_res;
  }
};

template <typename... CHILD_PATS_T>
struct ParserFactory<Seq<CHILD_PATS_T...>> {
  using pat_t = Seq<CHILD_PATS_T...>;

  static constexpr DstBehavior dst_behavior() { return DstBehavior::VALUE; }

  enum {
    ATOMIC = false,
    FAILS_CLEANLY = false,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = SeqImpl<CTX_T, DST_T, REQ_T, CHILD_PATS_T...>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
