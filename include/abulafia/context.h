//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_CONTEXT_H_
#define ABULAFIA_CONTEXT_H_

#include "abulafia/config.h"

#include <type_traits>

#include "abulafia/patterns/leaf/fail.h"

namespace ABULAFIA_NAMESPACE {
template <typename DATASOURCE_T, typename SKIPPER_T>
struct Context {
  using datasource_t = DATASOURCE_T;
  using skip_pattern_t = SKIPPER_T;

  Context(datasource_t& ds, skip_pattern_t const& skip)
      : data_(ds), skipper_(skip) {}

  template <typename T>
  using set_skipper_t = Context<datasource_t, T>;

  enum {
    IS_RESUMABLE = DATASOURCE_T::IS_RESUMABLE,
    HAS_SKIPPER = !std::is_same<Fail, skip_pattern_t>::value,
  };

  DATASOURCE_T& data() { return data_; }

 private:
  DATASOURCE_T& data_;
  SKIPPER_T const& skipper_;
};
}  // namespace ABULAFIA_NAMESPACE

#endif