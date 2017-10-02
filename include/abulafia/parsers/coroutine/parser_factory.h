//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_PARSER_FACTORY_H_
#define ABULAFIA_PARSERS_COROUTINE_PARSER_FACTORY_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/parsers/helpers/digit_values.h"
#include "abulafia/patterns/leaf/numeric/uint.h"
#include "abulafia/support/assert.h"

#include "abulafia/parsers/coroutine/adapters/atomic.h"
#include "abulafia/parsers/coroutine/adapters/clean_failure.h"

namespace ABULAFIA_NAMESPACE {
  
  struct DefaultReqs {
    enum {
      ATOMIC = false,
      FAILS_CLEANLY = false,
    };
  };

  template<typename PAT_T>
  struct ParserFactory;

  template<typename T, template<typename> typename ADAPT_T, bool enable>
  struct ConditionalAdapter {
    using type = T;
  };

  template<typename T, template<typename> typename ADAPT_T>
  struct ConditionalAdapter<T, ADAPT_T, true> {
    using type = ADAPT_T<T>;
  };

  template<typename T, template<typename> typename ADAPT_T, bool enable>
  using ConditionalAdapter_t = typename ConditionalAdapter<T, ADAPT_T, enable>::type;
  
  template<typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
  struct AdaptedParserFactory {
    static auto create(CTX_T ctx, DST_T dst, PAT_T const& pat) {

      using raw_factory = ParserFactory<PAT_T>;

      constexpr bool apply_atomic_adapter = REQ_T::ATOMIC && !raw_factory::ATOMIC;
      constexpr bool apply_clean_failure_adapter = REQ_T::FAILS_CLEANLY && !raw_factory::FAILS_CLEANLY;

      using a = raw_factory;
      using b = ConditionalAdapter_t<a, AtomicFactoryAdapter, apply_atomic_adapter>;
      using c = ConditionalAdapter_t<b, CleanFailureFactoryAdapter, apply_clean_failure_adapter>;

      //TODO: Apply skipper here.

      using parser_type = typename c::type<CTX_T, DST_T, REQ_T>;
      return parser_type(ctx, dst, pat);
    }
  };
}  // namespace ABULAFIA_NAMESPACE

#endif