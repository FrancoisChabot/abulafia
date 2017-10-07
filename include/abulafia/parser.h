//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSER_H_
#define ABULAFIA_PARSER_H_

#include "abulafia/config.h"

#include "abulafia/parsers/coroutine/parser_factory.h"
#include "abulafia/patterns/leaf/fail.h"
#include "abulafia/result.h"
#include "abulafia/support/nil.h"
#include "abulafia/support/type_traits.h"

namespace ABULAFIA_NAMESPACE {
template <typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
using Parser =
    decltype(AdaptedParserFactory<CTX_T, DST_T, REQ_T, PAT_T>::create(
        std::declval<CTX_T>(), std::declval<DST_T>(), std::declval<PAT_T>()));

template <typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
struct Parser_t
    : decltype(AdaptedParserFactory<CTX_T, DST_T, REQ_T, PAT_T>::create(
          std::declval<CTX_T>(), std::declval<DST_T>(),
          std::declval<PAT_T>())) {
  using real_parser_t =
      decltype(AdaptedParserFactory<CTX_T, DST_T, REQ_T, PAT_T>::create(
          std::declval<CTX_T>(), std::declval<DST_T>(), std::declval<PAT_T>()));
  using real_parser_t::real_parser_t;
};

template <typename CTX_T, typename DST_T, typename REQ_T, typename PAT_T>
auto make_parser_(CTX_T ctx, DST_T dst, REQ_T, PAT_T const& pat) {
  return AdaptedParserFactory<CTX_T, DST_T, REQ_T, PAT_T>::create(ctx, dst,
                                                                  pat);
}

}  // namespace ABULAFIA_NAMESPACE

#endif
