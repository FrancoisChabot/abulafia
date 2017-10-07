//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_PARSER_FACTORY_H_
#define ABULAFIA_PARSERS_COROUTINE_PARSER_FACTORY_H_

#include "abulafia/config.h"

#include "abulafia/parsers/helpers/digit_values.h"
#include "abulafia/patterns/leaf/numeric/uint.h"
#include "abulafia/support/assert.h"

#include "abulafia/parsers/coroutine/dst_behavior.h"

#include "abulafia/parsers/coroutine/adapters/atomic.h"
#include "abulafia/parsers/coroutine/adapters/clean_failure.h"
#include "abulafia/parsers/coroutine/adapters/skip.h"

namespace ABULAFIA_NAMESPACE {

struct DefaultReqs {
  enum {
    ATOMIC = false,
    FAILS_CLEANLY = false,
    CONSUMES_ON_SUCCESS = false,
  };
};

template <typename PAT_T>
struct ParserFactory;

template <typename CTX_T, typename DST_T, typename REQ_T, typename REF_PAT_T>
struct RecurParserChildImpl
    : public ParserFactory<REF_PAT_T>::template type<CTX_T, DST_T, REQ_T> {
  using parent_t =
      typename ParserFactory<REF_PAT_T>::template type<CTX_T, DST_T, REQ_T>;
  using parent_t::parent_t;
};

template <typename PAT_T>
struct ParserFactory {
  // Is this not a recur or a rename?
  using pat_t = typename PAT_T::pattern_t;

  static constexpr DstBehavior dst_behavior() {
    return ParserFactory<pat_t>::dst_behavior();
  }

  enum {
    ATOMIC = ParserFactory<pat_t>::ATOMIC,
    FAILS_CLEANLY = ParserFactory<pat_t>::FAILS_CLEANLY,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = RecurParserChildImpl<CTX_T, DST_T, REQ_T, pat_t>;
};

template <typename T, template <typename> typename ADAPT_T, bool enable>
struct ConditionalAdapter {
  using type = T;
};

template <typename T, template <typename> typename ADAPT_T>
struct ConditionalAdapter<T, ADAPT_T, true> {
  using type = ADAPT_T<T>;
};

template <typename T, template <typename> typename ADAPT_T, bool enable>
using ConditionalAdapter_t =
    typename ConditionalAdapter<T, ADAPT_T, enable>::type;

template <typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
struct AdaptedParserFactory {
  static auto create(CTX_T ctx, DST_T dst, PAT_T const& pat) {
    using raw_factory = ParserFactory<PAT_T>;
    using skip_t = typename CTX_T::skip_pattern_t;

    constexpr bool apply_atomic_adapter = REQ_T::ATOMIC && !raw_factory::ATOMIC;
    constexpr bool apply_clean_failure_adapter =
        REQ_T::FAILS_CLEANLY && !raw_factory::FAILS_CLEANLY;
    constexpr bool apply_skipper_adapter = !std::is_same<skip_t, Fail>::value;

    using a = raw_factory;
    using b =
        ConditionalAdapter_t<a, AtomicFactoryAdapter, apply_atomic_adapter>;
    using c = ConditionalAdapter_t<b, CleanFailureFactoryAdapter,
                                   apply_clean_failure_adapter>;
    using d =
        ConditionalAdapter_t<c, SkipFactoryAdapter, apply_skipper_adapter>;

    // TODO: Apply skipper here.

    using parser_type = typename d::template type<CTX_T, DST_T, REQ_T>;
    return parser_type(ctx, dst, pat);
  }
};

}  // namespace ABULAFIA_NAMESPACE

#endif