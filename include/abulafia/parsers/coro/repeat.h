
//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_COROD_REPEAT_H_INCLUDED
#define ABULAFIA_PARSERS_COROD_REPEAT_H_INCLUDED

#include "abulafia/parsers/coro/parser.h"
#include "abulafia/patterns.h"

namespace abu::coro {

template <PatternTemplate<pat::repeat> auto pattern,
          Policies auto policies,
          DataSource Data>
class parser<pattern, policies, Data> {
 public:
  using value_type = parsed_value_t<decltype(pattern), policies, Data>;
  using operand_parser = parser<pattern.operand, policies, Data>;
  using op_value_type =
      parsed_value_t<decltype(pattern.operand), policies, Data>;
  

  constexpr parser(Data& data) : child_op_(data) {}

  template <ParseCallback<pattern, policies, Data> CbT>
  constexpr op_result on_tokens(Data& data, const CbT& cb) {
    while (true) {
      auto res = child_op_.on_tokens(
          data, [this](op_value_type&& v) { result_.push_back(std::move(v)); });

      if (res.is_partial()) {
        return partial_result;
      }

      if (res.is_match_failure()) {
        return finish_(cb);
      }

      abu_assume(res.is_success());

      if (pattern.max != 0 && result_.size() == pattern.max) {
        return finish_(cb);
      } else {
        child_op_ = operand_parser{data};
      }
    }
  }

  template <ParseCallback<pattern, policies, Data> CbT>
  constexpr op_result on_end(Data& data, const CbT& cb) {
    while (true) {
      auto res = child_op_.on_end(data,
          [this](op_value_type&& v) { result_.push_back(std::move(v)); });
      if (res.is_match_failure()) {
        return finish_(cb);
      }

      abu_assume(res.is_success());

      if (pattern.max != 0 && result_.size() == pattern.max) {
        return finish_(cb);
      } else {
        child_op_ = operand_parser{data};
      }
    }
  }

 private:
  template <ParseCallback<pattern, policies, Data> CbT>
  constexpr op_result finish_(const CbT& cb) {
    if (result_.size() >= pattern.min) {
      cb(std::move(result_));
      return success;
    } else {
      return failure_t{};
    }
  }

  value_type result_;
  operand_parser child_op_;
};

}  // namespace abu::coro

#endif
