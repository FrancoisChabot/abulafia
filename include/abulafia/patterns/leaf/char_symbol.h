//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_LEAF_CHAR_SYMBOL_H_
#define ABULAFIA_PATTERNS_LEAF_CHAR_SYMBOL_H_

#include "abulafia/config.h"

#include "abulafia/patterns/leaf/leaf_pattern.h"

#include <map>

namespace ABULAFIA_NAMESPACE {

template <typename CHAR_T, typename VAL_T>
class CharSymbol : public LeafPattern<CharSymbol<CHAR_T, VAL_T>> {
  std::map<CHAR_T, VAL_T> mapping_;

 public:
  CharSymbol(std::map<CHAR_T, VAL_T> vals) : mapping_(std::move(vals)) {}

  std::map<CHAR_T, VAL_T> const& mapping() const { return mapping_; }
};

template <typename CHAR_T, typename VAL_T>
auto symbol(std::map<CHAR_T, VAL_T> const& vals) {
  return CharSymbol<CHAR_T, VAL_T>(vals);
}

}  // namespace ABULAFIA_NAMESPACE

#endif
