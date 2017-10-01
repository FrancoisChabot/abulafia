//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_ADAPTER_ATOMIC_H_
#define ABULAFIA_PARSERS_COROUTINE_ADAPTER_ATOMIC_H_

#include "abulafia/config.h"

namespace ABULAFIA_NAMESPACE {

  // This is to be used to enforce ATOMIC on a parser
  // that does not naturally meet that requirement
  template<typename PARSER_T>
  class AtomicAdapter {
  public:
    using ctx_t = typename PARSER_T::ctx_t;
    using dst_t = typename PARSER_T::dst_t;
    using pat_t = typename PARSER_T::pat_t;
    using buffer_t = typename dst_t::dst_type;

    using adapted_dst_t = ValueWrapper<buffer_t>;
    using adapted_parser_t = typename PARSER_T::template change_dst_t<adapted_dst_t>;

    AtomicAdapter(ctx_t ctx, dst_t, pat_t const& pat)
      : adapted_parser_(ctx, adapted_dst_t(buffer_), pat) {
    }

    result consume(ctx_t ctx, dst_t dst, pat_t const& pat) {
      auto status = adapted_parser_.consume(ctx, adapted_dst_t(buffer_), pat);
      if (status == result::SUCCESS) {
        dst = buffer_;
      }
      return status;
    }

  private:
    buffer_t buffer_;
    adapted_parser_t adapted_parser_;
  };

}  // namespace ABULAFIA_NAMESPACE

#endif