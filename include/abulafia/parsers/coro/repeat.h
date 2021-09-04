
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

namespace details_ {
template <DataSource Data, bool rollback_enabled = true>
class maybe_checkpoint {
 public:
  using checkpoint_type = Data::checkpoint_type;

  maybe_checkpoint(Data& data) : checkpoint_(data.make_checkpoint()) {}

  constexpr void reset(Data& data) {
    checkpoint_ = data.make_checkpoint();
  }
  constexpr void rollback(Data& data) { data.rollback(std::move(checkpoint_)); }

 private:
  checkpoint_type checkpoint_;
};

template <DataSource Data>
class maybe_checkpoint<Data, false> {
 public:
  constexpr maybe_checkpoint(Data& data) {}
  constexpr void reset(Data& data) {}
  constexpr void rollback(Data& data){};
};

template <PatternTemplate<pat::repeat> auto pattern,
          Policies auto policies,
          DataSource Data,
          bool parser>
class basic_repeat {
  using checkpoint_type =
      maybe_checkpoint<Data, pattern.operand.consumes_on_failure()>;
  using child_op_type =
      std::conditional_t<parser,
                         coro::parser<pattern.operand, policies, Data>,
                         coro::matcher<pattern.operand, policies, Data>>;

  using token_type = typename Data::token_type;
  using value_type = parsed_value_t<decltype(pattern), token_type, policies>;
  using result_type =
      std::conditional_t<parser, value_type, abu::details_::null_vector_sink_t>;

 public:
  basic_repeat(Data& data) : checkpoint_(data), child_op(data) {}

  template <typename Cb>
  constexpr bool on_tokens_done(Data& data,
                                op_result child_res,
                                const Cb& final_cb,
                                op_result& out_result) {
    if (child_res.is_partial()) {
      return false;
    }

    if (child_res.is_match_failure()) {
      out_result = finalize_(final_cb);
      if (out_result.is_success()) {
        checkpoint_.rollback(data);
      }
      return false;
    }

    abu_assume(child_res.is_success());
    return handle_child_success(data, final_cb, out_result);
  }

  template <typename Cb>
  constexpr bool on_end_done(Data& data,
                             op_result child_res,
                             const Cb& final_cb,
                             op_result& out_result) {
    if (child_res.is_match_failure()) {
      out_result = finalize_(final_cb);
      if (out_result.is_success()) {
        checkpoint_.rollback(data);
      }
      return false;
    }
    abu_assume(child_res.is_success());
    return handle_child_success(data, final_cb, out_result);
  }

  template <typename Cb>
  constexpr bool handle_child_success(Data& data,
                                      const Cb& cb,
                                      op_result& out_result) {
    
    if (pattern.max != 0 && result.size() == pattern.max) {
      out_result = finalize_(cb);
      return false;
    }

    checkpoint_.reset(data);
    child_op = child_op_type{data};
    return true;
  }

  template <typename Cb>
  constexpr op_result finalize_(const Cb& cb) {
    if (result.size() >= pattern.min) {
      cb(std::move(result));
      return success;
    } else {
      return failure_t{};
    }
  }

  [[no_unique_address]] checkpoint_type checkpoint_;
  child_op_type child_op;
  result_type result;
};
}  // namespace details_

// ***** parser<repeat> *****
template <PatternTemplate<pat::repeat> auto pattern,
          Policies auto policies,
          DataSource Data>
class parser<pattern, policies, Data> {
  using token_type = typename Data::token_type;
  details_::basic_repeat<pattern, policies, Data, true> base_;

 public:
  constexpr parser(Data& data) : base_(data) {}

  template <ParseCallback<pattern, token_type, policies> CbT>
  constexpr op_result on_tokens(Data& data, const CbT& cb) {
    op_result result = partial_result;
    bool keep_going = true;
    while (keep_going) {
      auto child_res = base_.child_op.on_tokens(
          data, [&](auto v) { base_.result.push_back(std::move(v)); });
      keep_going = base_.on_tokens_done(data, child_res, cb, result);
    }
    return result;
  }

  template <ParseCallback<pattern, token_type, policies> CbT>
  constexpr op_result on_end(Data& data, const CbT& cb) {
    op_result result = partial_result;
    bool keep_going = true;
    while (keep_going) {
      auto child_res = base_.child_op.on_end(
          data, [&](auto v) { base_.result.push_back(std::move(v)); });
      keep_going = base_.on_end_done(data, child_res, cb, result);
    }
    return result;
  }
};

// ***** matcher<repeat> *****
template <PatternTemplate<pat::repeat> auto pattern,
          Policies auto policies,
          DataSource Data>
class matcher<pattern, policies, Data> {
  details_::basic_repeat<pattern, policies, Data, false> base_;

 public:
  constexpr matcher(Data& data) : base_(data) {}

  constexpr op_result on_tokens(Data& data) {
    op_result result = partial_result;
    bool keep_going = true;
    while (keep_going) {
      auto child_res = base_.child_op.on_tokens(data);
      if (child_res.is_success()) {
        base_.result.add();
      }
      keep_going =
          base_.on_tokens_done(data, child_res, abu::details_::noop, result);
    }
    return result;
  }

  constexpr op_result on_end(Data& data) {
    op_result result = partial_result;
    bool keep_going = true;
    while (keep_going) {
      auto child_res = base_.child_op.on_end(data);
      if (child_res.is_success()) {
        base_.result.add();
      }

      keep_going =
          base_.on_end_done(data, child_res, abu::details_::noop, result);
    }
    return result;
  }
};

}  // namespace abu::coro

#endif
