//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_LEAF_CHAR_SYMBOL_H_
#define ABULAFIA_PATTERNS_LEAF_CHAR_SYMBOL_H_

#include "abulafia/config.h"

#include "abulafia/parser.h"
#include "abulafia/pattern.h"

#include <map>
#include <optional>
#include <stdexcept>

namespace ABULAFIA_NAMESPACE {

template <typename CHAR_T, typename VAL_T>
class CharSymbol : public Pattern<CharSymbol<CHAR_T, VAL_T>> {
  std::map<CHAR_T, VAL_T> mapping_;

 public:
  CharSymbol(std::map<CHAR_T, VAL_T> const& vals) : mapping_(vals) {}

  std::map<CHAR_T, VAL_T> const& mapping() const { return mapping_; }
};

template <typename CHAR_T, typename VAL_T, typename RECUR_TAG>
struct pattern_traits<CharSymbol<CHAR_T, VAL_T>, RECUR_TAG>
    : public default_pattern_traits {
  enum {
    ATOMIC = true,
    BACKTRACKS = true,
    PEEKABLE = true,
    FAILS_CLEANLY = true,
    MAY_NOT_CONSUME = false,
  };
};

template <typename CHAR_T, typename VAL_T, typename CTX_T>
struct pat_attr_t<CharSymbol<CHAR_T, VAL_T>, CTX_T> {
  using attr_type = VAL_T;
};

}  // namespace ABULAFIA_NAMESPACE

#endif
