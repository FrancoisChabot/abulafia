//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef ABULAFIA_PATTERNS_STRING_SYMBOL_H_
#define ABULAFIA_PATTERNS_STRING_SYMBOL_H_

#include "abulafia/config.h"

#include "abulafia/patterns/pattern.h"

#include <map>
#include <memory>
#include <optional>
#include <stdexcept>

namespace ABULAFIA_NAMESPACE {

template <typename CHAR_T, typename VAL_T>
class Symbol : public Pattern<Symbol<CHAR_T, VAL_T>> {
  // symbols->value map will be stored as a trie
  struct Node {
    std::map<CHAR_T, Node> child;
    std::optional<VAL_T> val;
  };

  std::shared_ptr<Node> root_;

 public:
  using node_t = Node;

  Symbol(std::map<std::basic_string<CHAR_T>, VAL_T> const& vals)
      : root_(std::make_shared<Node>()) {
    for (auto const& entry : vals) {
      node_t* next = root_.get();

      for (auto const& chr : entry.first) {
        next = &next->child[chr];
      }

      if (next->val) {
        throw std::runtime_error("cannot have two symbols with the same value");
      }

      next->val = entry.second;
    }
  }

  Node const* root() const { return root_.get(); }
};

template <typename CHAR_T, typename VAL_T>
auto symbol(std::map<std::basic_string<CHAR_T>, VAL_T> const& vals) {
  return Symbol<CHAR_T, VAL_T>(vals);
}

}  // namespace ABULAFIA_NAMESPACE

#endif
