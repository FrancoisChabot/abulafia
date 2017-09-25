//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_CHAR_SET_CHAR_SET_H_
#define ABULAFIA_CHAR_SET_CHAR_SET_H_

#include "abulafia/config.h"

#include <type_traits>

namespace ABULAFIA_NAMESPACE {
namespace char_set {

template <typename T>
struct is_char_set : public std::false_type {};

}  // namespace char_set
}  // namespace ABULAFIA_NAMESPACE
#endif