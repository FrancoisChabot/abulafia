//  Copyright 2017-2021 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_TOKEN_H_INCLUDED
#define ABULAFIA_TOKEN_H_INCLUDED

#include <concepts>

namespace abu {

// While tokens are only required to be regular, it's possible for additional
// requirements to be added by individual patterns.
template <typename T>
concept Token = std::copyable<T>;

// TODO: this should be constrained somewhat, but it's trickier than it looks...
template <typename PredT>
struct token_set {
  using predicate = PredT;
  explicit constexpr token_set(predicate pred) : pred_(std::move(pred)) {}

  template <Token T>
  constexpr bool operator()(const T& t) const {
    return pred_(t);
  }

  constexpr const predicate& pred() const { return pred_; }

 private:
  [[no_unique_address]] predicate pred_;
};

template <typename T>
concept TokenSet = requires(T x) {
  { token_set(x) } -> std::same_as<T>;
};

template <TokenSet T>
constexpr auto operator~(const T& arg) {
  return token_set([p = arg.pred()](const auto& t) { return !p(t); });
}

template <TokenSet LhsT, TokenSet RhsT>
constexpr auto operator-(const LhsT& lhs, const RhsT& rhs) {
  return token_set([pl = lhs.pred(), pr = rhs.pred()](const auto& t) {
    return !pr(t) && pl(t);
  });
}

}  // namespace abu
#endif