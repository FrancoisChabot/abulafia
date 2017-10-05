//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_CHAR_SET_ANY_H_
#define ABULAFIA_CHAR_SET_ANY_H_

#include "abulafia/config.h"

#include "abulafia/char_set/char_set.h"

namespace ABULAFIA_NAMESPACE {
namespace char_set {

template <typename CHAR_T>
struct Any : public CharacterSet {
  using char_t = CHAR_T;

  bool is_valid(char_t const &) const { return true; }
};

template <typename CHAR_T>
Any<CHAR_T> any;

}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE
#endif