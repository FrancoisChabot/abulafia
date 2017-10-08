//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROUTINE_CONSTRUCT_H_
#define ABULAFIA_PARSERS_COROUTINE_CONSTRUCT_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/patterns/unary/construct.h"

namespace ABULAFIA_NAMESPACE {

template <typename CTX_T, typename DST_T, typename REQ_T, typename CHILD_PAT_T, typename... ARGS_T>
class ConstructImpl {
  using pat_t = Construct<CHILD_PAT_T, ARGS_T...>;

  using buffer_t = std::tuple<ARGS_T...>;

  using child_dst_t = typename SelectDstWrapper<buffer_t>::type;

  struct childs_reqs_t : public REQ_T {
    enum { ATOMIC = false };
  };


  using child_parser_t = Parser<CTX_T, child_dst_t, childs_reqs_t, CHILD_PAT_T>;

  buffer_t buffer_;
  child_parser_t parser_;

 public:
  ConstructImpl(CTX_T ctx, DST_T, pat_t const& pat)
      : parser_(ctx, child_dst_t(buffer_), pat.child_pattern()) {
  }

  Result consume(CTX_T ctx, DST_T dst, pat_t const& pat) {
    auto status = parser_.consume(ctx, child_dst_t(buffer_), pat.child_pattern());

    if( status == Result::SUCCESS) {
      dst = std::make_from_tuple<typename DST_T::dst_type>(buffer_);
    }
    return status;
  }

};

template <typename CHILD_PAT_T, typename... ARGS_T>
struct ParserFactory<Construct<CHILD_PAT_T, ARGS_T...>> {
  using pat_t = Construct<CHILD_PAT_T, ARGS_T...>;

  static constexpr DstBehavior dst_behavior() { return DstBehavior::VALUE; }

  enum {
    ATOMIC = true,
    FAILS_CLEANLY = true,
  };

  template <typename CTX_T, typename DST_T, typename REQ_T>
  using type = ConstructImpl<CTX_T, DST_T, REQ_T, CHILD_PAT_T, ARGS_T...>;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
