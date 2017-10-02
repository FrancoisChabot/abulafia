//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_ADAPTER_ATOMIC_H_
#define ABULAFIA_PARSERS_COROUTINE_ADAPTER_ATOMIC_H_

#include "abulafia/config.h"
#include "abulafia/dst_wrapper/select_wrapper.h"

namespace ABULAFIA_NAMESPACE {

  // This is to be used to enforce ATOMIC on a parser
  // that does not naturally meet that requirement
  template<typename CTX_T, typename DST_T, typename REQ_T, typename PARSER_FACTORY_T>
  class AtomicAdapter {
  public:
    using pat_t = typename PARSER_FACTORY_T::pat_t;
    using buffer_t = typename DST_T::dst_type;

    struct adapted_reqs_t : public REQ_T {
      enum { ATOMIC = false };
    };

    using adapted_dst_t = typename SelectDstWrapper<buffer_t>::type;
    using child_parser_t = typename PARSER_FACTORY_T:: template type<CTX_T, adapted_dst_t, adapted_reqs_t>;

    AtomicAdapter(CTX_T ctx, DST_T, pat_t const& pat)
      : adapted_parser_(ctx, adapted_dst_t(buffer_), pat) {
    }

    Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
      auto status = adapted_parser_.consume(ctx, adapted_dst_t(buffer_), pat);
      if (status == Result::SUCCESS) {
        dst = buffer_;
      }
      return status;
    }

  private:
    buffer_t buffer_;
    child_parser_t adapted_parser_;
  };


  template<typename FACTORY_T>
  struct AtomicFactoryAdapter {
    static_assert(!FACTORY_T::ATOMIC);

    using pat_t = typename FACTORY_T::pat_t;

    enum {
      ATOMIC = true,
      FAILS_CLEANLY = FACTORY_T::FAILS_CLEANLY,
    };

    
    template<typename CTX_T, typename DST_T, typename REQ_T>
    using type = AtomicAdapter<CTX_T, DST_T, REQ_T, FACTORY_T>;
  };

}  // namespace ABULAFIA_NAMESPACE

#endif