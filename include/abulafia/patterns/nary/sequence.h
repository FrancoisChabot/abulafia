//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_NARY_SEQUENCE_H_
#define ABULAFIA_PATTERNS_NARY_SEQUENCE_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/pattern.h"
#include "abulafia/patterns/helpers/buffer.h"
#include "abulafia/support/type_traits.h"

#include <utility>
#include <variant>

namespace ABULAFIA_NAMESPACE {

namespace seq_ {

template <typename LHS_T, typename RHS_T, typename Enable = void>
struct l_append_t {
  using type = std::tuple<LHS_T, RHS_T>;
};

template <>
struct l_append_t<Nil, Nil, void> {
  using type = Nil;
};

template <typename LHS_T>
struct l_append_t<LHS_T, Nil, void> {
  using type = LHS_T;
};

template <typename RHS_T>
struct l_append_t<Nil, RHS_T, void> {
  using type = RHS_T;
};

template <typename LHS_T, typename... RHS_TPL_T>
struct l_append_t<LHS_T, std::tuple<RHS_TPL_T...>,
                  std::enable_if_t<!std::is_same<LHS_T, Nil>::value &&
                                   !is_tuple<LHS_T>::value>> {
  using type = std::tuple<LHS_T, RHS_TPL_T...>;
};

template <typename... RHS_TPL_T>
struct l_append_t<Nil, std::tuple<RHS_TPL_T...>, void> {
  using type = std::tuple<RHS_TPL_T...>;
};

template <typename... LHS_TPL_T, typename RHS_T>
struct l_append_t<std::tuple<LHS_TPL_T...>, RHS_T, void> {
  using type = std::tuple<LHS_TPL_T..., RHS_T>;
};

template <typename... LHS_TPL_T>
struct l_append_t<std::tuple<LHS_TPL_T...>, Nil, void> {
  using type = std::tuple<LHS_TPL_T...>;
};

template <typename CTX_T, typename T, typename... REST_T>
struct determine_attr_type {
  using type = attr_t<T, CTX_T>;
};

template <typename CTX_T, typename T, typename U, typename... REST_T>
struct determine_attr_type<CTX_T, T, U, REST_T...> {
  using lhs_t_ = attr_t<T, CTX_T>;
  using rhs_t_ = typename determine_attr_type<CTX_T, U, REST_T...>::type;

  using type = l_append_t<lhs_t_, rhs_t_>;
};

template <int PAT_ID, typename CTX_T, typename CHILDS_TUPLE_T,
          typename Enable = void>
struct choose_tuple_index;

template <typename CTX_T, typename CHILDS_TUPLE_T>
struct choose_tuple_index<-1, CTX_T, CHILDS_TUPLE_T> {
  enum { value = -1, next_val = 0 };
};

template <int PAT_ID, typename CTX_T, typename CHILDS_TUPLE_T>
struct choose_tuple_index<
    PAT_ID, CTX_T, CHILDS_TUPLE_T,
    enable_if_t<(
        PAT_ID != -1 &&
        std::is_same<Nil, attr_t<tuple_element_t<PAT_ID == -1 ? 0 : PAT_ID,
                                                 CHILDS_TUPLE_T>,
                                 CTX_T>>::value)>> {
  enum {
    value = -1,
    next_val = choose_tuple_index<PAT_ID - 1, CTX_T, CHILDS_TUPLE_T>::next_val
  };
};

template <int PAT_ID, typename CTX_T, typename CHILDS_TUPLE_T>
struct choose_tuple_index<
    PAT_ID, CTX_T, CHILDS_TUPLE_T,
    enable_if_t<(
        PAT_ID != -1 &&
        !std::is_same<Nil, attr_t<tuple_element_t<PAT_ID == -1 ? 0 : PAT_ID,
                                                  CHILDS_TUPLE_T>,
                                  CTX_T>>::value)>> {
  enum {
    value = choose_tuple_index<PAT_ID - 1, CTX_T, CHILDS_TUPLE_T>::next_val,
    next_val = value + 1
  };
};

enum DstAccessorCategory {
  USE_NIL = 0,
  PASSTHROUGH = 1,
  INDEXED = 2,
  COLLECTION = 3
};

template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
constexpr DstAccessorCategory ChooseAccessorCategory() {
  if (std::is_same<Nil, DST_T>::value ||
      std::is_same<
          Nil, attr_t<tuple_element_t<PAT_ID, CHILDS_TUPLE_T>, CTX_T>>::value) {
    return USE_NIL;
  } else if (is_tuple<DST_T>::value) {
    return INDEXED;
  } else if (is_collection<DST_T>::value) {
    return COLLECTION;
  } else {
    return PASSTHROUGH;
  }
}

template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
struct AccessorCategoryChooser {
  enum {
    use_nil = ChooseAccessorCategory<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>() ==
              USE_NIL,
    passthrough =
        ChooseAccessorCategory<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>() ==
        PASSTHROUGH,
    indexed = ChooseAccessorCategory<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>() ==
              INDEXED,
    collection =
        ChooseAccessorCategory<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>() ==
        COLLECTION,
  };
};

template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T, typename Enable = void>
struct choose_dst_accessor;

// Pass nil
template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
struct choose_dst_accessor<
    PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T,
    enable_if_t<AccessorCategoryChooser<PAT_ID, CTX_T, DST_T,
                                        CHILDS_TUPLE_T>::use_nil>> {
  using type = Nil;
  static Nil& access(DST_T&) { return nil; }
};

template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
struct choose_dst_accessor<
    PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T,
    enable_if_t<AccessorCategoryChooser<PAT_ID, CTX_T, DST_T,
                                        CHILDS_TUPLE_T>::collection>> {
  using type = CollectionAssignWrapper<DST_T>;
  static type access(DST_T& dst) { return type(dst); }
};

// Passthrough
template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
struct choose_dst_accessor<
    PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T,
    enable_if_t<AccessorCategoryChooser<PAT_ID, CTX_T, DST_T,
                                        CHILDS_TUPLE_T>::passthrough>> {
  using type = DST_T;
  static DST_T& access(DST_T& dst) { return dst; }
};

// Indexed
template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
struct choose_dst_accessor<
    PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T,
    enable_if_t<AccessorCategoryChooser<PAT_ID, CTX_T, DST_T,
                                        CHILDS_TUPLE_T>::indexed>> {
  enum { dst_index = choose_tuple_index<PAT_ID, CTX_T, CHILDS_TUPLE_T>::value };

  using type = tuple_element_t<dst_index, DST_T>;
  static auto& access(DST_T& dst) { return std::get<dst_index>(dst); }
};

template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
struct WrappedParser {
  using dst_t =
      typename choose_dst_accessor<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>::type;
  using type = Parser<
      CTX_T,
      typename choose_dst_accessor<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>::type,
      tuple_element_t<PAT_ID, CHILDS_TUPLE_T>>;
};

template <std::size_t PAT_ID, typename CTX_T, typename DST_T,
          typename CHILDS_TUPLE_T>
using WrappedParser_t =
    typename WrappedParser<PAT_ID, CTX_T, DST_T, CHILDS_TUPLE_T>::type;

template <typename CTX_T, typename DST_T, typename CHILDS_TUPLE_T,
          typename INDEX_SEQ>
struct SeqSubParser;

template <typename CTX_T, typename DST_T, typename CHILDS_TUPLE_T,
          std::size_t... PAT_IDS>
struct SeqSubParser<CTX_T, DST_T, CHILDS_TUPLE_T,
                    std::index_sequence<PAT_IDS...>> {
  //  using type = std::variant<WrappedParser_t<I, CTX_T, DST_T,
  //  CHILDS_TUPLE_T>...>;
  using test_test = std::index_sequence<PAT_IDS...>;
  using type =
      std::variant<WrappedParser_t<PAT_IDS, CTX_T, DST_T, CHILDS_TUPLE_T>...>;
};

}  // namespace seq_

// The sequence nary pattern.
template <typename... CHILD_PATS_T>
class Seq : public Pattern<Seq<CHILD_PATS_T...>> {
 public:
  using child_tuple_t = std::tuple<CHILD_PATS_T...>;

  // The computed type for individual members of the sequence
  Seq(child_tuple_t const& childs) : childs_(childs) {}

  child_tuple_t const& childs() const { return childs_; }

 private:
  child_tuple_t childs_;
};

template <std::size_t Index, typename... CHILD_PATS_T>
auto const& getChild(Seq<CHILD_PATS_T...> const& pat) {
  return std::get<Index>(pat.childs());
}

template <typename... CHILD_PATS_T>
auto seq(CHILD_PATS_T&&... childs) {
  return Seq<CHILD_PATS_T...>(
      std::make_tuple(std::forward<CHILD_PATS_T>(childs)...));
}

template <typename CHILD_TUP_T, typename CB_T, std::size_t... Is>
auto convert_seq_impl(CHILD_TUP_T const& c, CB_T const& cb,
                      std::index_sequence<Is...>) {
  return seq(convert(std::get<Is>(c), cb)...);
}

template <typename... CHILD_PATS_T, typename CB_T>
auto convert(Seq<CHILD_PATS_T...> const& tgt, CB_T const& cb) {
  using indices = std::make_index_sequence<sizeof...(CHILD_PATS_T)>;
  auto const& childs_tuple = tgt.childs();

  return convert_seq_impl(childs_tuple, cb, indices());
}

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
            make_parser(ctx, force_lvalue(getDstFor<new_id>(dst)),
                        new_c_pattern));

        return consume_from<new_id>(ctx, dst, pat);
      }
    }
    return child_res;
  }
};

template <typename RECUR_TAG, typename... CHILD_PATS>
struct pattern_traits<Seq<CHILD_PATS...>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    BACKTRACKS = any_pat_traits<RECUR_TAG, CHILD_PATS...>::BACKTRACKS,
    FAILS_CLEANLY = false,
    MAY_NOT_CONSUME = all_pat_traits<RECUR_TAG, CHILD_PATS...>::MAY_NOT_CONSUME,
    PEEKABLE = false,  // TODO: not quite true, "char_() + -char_()" is peekable
                       // for example
    ATOMIC = false,
    APPENDS_DST = true,

    // TODO: sort this out
    // for a sequence to be stable, there must be no more than 1 child that
    //   is Non-Nil
    // AND
    //   is non-optional
    // AND
    //    is a non-appender
    //  OR
    //    is an unstable appender

    STABLE_APPENDS = false
  };
};

template <typename CTX_T, typename... CHILD_PATS>
struct pat_attr_t<Seq<CHILD_PATS...>, CTX_T> {
  using attr_type =
      typename seq_::determine_attr_type<CTX_T, CHILD_PATS...>::type;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
