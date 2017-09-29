//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_CHAR_SET_RANGE_H_
#define ABULAFIA_CHAR_SET_RANGE_H_

#include "abulafia/config.h"

#include "abulafia/char_set/char_set.h"
#include "abulafia/support/assert.h"

namespace ABULAFIA_NAMESPACE {
namespace char_set {

template <typename CHAR_T>
struct Range : public CharacterSet {
  using char_t = CHAR_T;

  Range(CHAR_T b, CHAR_T e) : begin_(b), end_(e) { 
    assert(b <= e); 
  }

  bool is_valid(char_t const& token) const {
    return token >= begin_ && token <= end_;
  }

 private:
  CHAR_T begin_;
  CHAR_T end_;
};

template <typename CHAR_T>
inline auto range(CHAR_T b, CHAR_T e) {
  return Range<CHAR_T>(b, e);
}

}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE
#endif