//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PARSERS_CORO_REPEAT_H_INCLUDED
#define ABULAFIA_PARSERS_CORO_REPEAT_H_INCLUDED

#include <vector>

#include "abulafia/parsers/coro/context.h"
#include "abulafia/parsers/coro/operation.h"
#include "abulafia/patterns.h"

namespace abu::coro {

namespace repeat_ {
template <typename T, std::size_t Min, std::size_t Max>
struct value : public std::type_identity<std::vector<T>> {};

template <std::size_t Min, std::size_t Max>
struct value<char, Min, Max>
    : public std::type_identity<std::basic_string<char>> {};

template <typename T, std::size_t Min, std::size_t Max>
using value_t = typename value<T, Min, Max>::type;

}  // namespace repeat_

template <ForwardContext Ctx, Pattern Operand, std::size_t Min, std::size_t Max>
struct parse_op<Ctx, pat::repeat<Operand, Min, Max>> {
  using value_type =
      repeat_::value_t<typename parse_op<Tok, Operand>::value_type, Min, Max>;

  using checkpoint_type = typename Ctx::checkpoint_type;

  parse_op(Ctx& ctx) : checkpoint_(ctx.checkpoint()), child_op_(ctx) {}

  constexpr status on_tokens(Ctx& ctx, const Cb& complete) {
    while (true) {
      auto res = child_op.on_tokens(
          ctx, [&](auto v) { result_.push_back(std::move(v)); });

      switch (res) {
        case success:
          if (Max != 0 && result_.size() == Max) {
            return finish_(ctx, complete);
          } else {
            child_op.reset(ctx);
          }
          break;
        case failure:
          return finish_(ctx, complete);
        default:
          return res;
      }
    }
  }

  template <typename Cb>
  constexpr final_status on_end(Ctx& ctx, const Cb& complete) {
    auto res =
        child_op.on_end(ctx, [&](auto v) { result_.push_back(std::move(v)); });

    return finish_(ctx, complete);
  }

 private:
  template <typename Cb>
  constexpr final_status finish_(Ctx& ctx, const Cb& complete) {
    if (result_.size() >= Min) {
      complete(std::move(result_));
      return success;
    } else {
      ctx.rollback(checkpoint_);
      return failure;
    }
  }

  checkpoint_type checkpoint_;
  value_type result_;
  child_op<Ctx, Operand> child_op_;
};

}  // namespace abu::coro

#endif