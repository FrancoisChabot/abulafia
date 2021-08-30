//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

namespace {
struct node {
  node(std::vector<std::unique_ptr<node>> childs)
      : childs_(std::move(childs)) {}

  std::vector<std::unique_ptr<node>> childs_;
};
}  // namespace

struct node_parser : public abu::rule<> {};

TEST(recur, simple_tree) {
  abu::forward<struct node_parser> pat;

  auto make_node = [](auto p) { return std::make_unique<node>(std::move(p)); };
  struct node_parser {
    static constexpr auto pattern = ('{' >> (pat % ',') >> '}')[make_node];
  };

  //  {{},{}}
  auto pat_impl = ('{' >> (pat % ',') >> '}')[make_node];

  auto tmp = parse("{{{}},{}}", pat);
  EXPECT_EQ((*tmp).childs.size(), 2);
  EXPECT_EQ((*tmp).childs[0].size(), 1);
  EXPECT_EQ((*tmp).childs[1].size(), 0);
}