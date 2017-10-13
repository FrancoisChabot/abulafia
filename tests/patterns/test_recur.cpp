//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"
#include "test_utils.h"

using namespace abu;

// Test basic syntax without any recursivity involved.
TEST(test_recur, simple_test) {
  RecurMemoryPool pool;

  Recur<struct abc, std::vector<char>> recur(pool);

  ABU_Recur_define(recur, abc, *char_());

  testPatternSuccess("abc", recur, std::string("abc"));
}

// Test with simple recursivity
TEST(test_recur, recur_test) {
  RecurMemoryPool pool;

  Recur<struct abc> recur(pool);

  auto recur_pat = Int<10, 2, 2>() >> *recur;

  ABU_Recur_define(recur, abc, recur_pat);

  testPatternSuccess("123456", recur, nil);
}

TEST(test_recur, node_generation) {
  RecurMemoryPool pool;

  struct Node {
    char c;
    std::vector<Node> n;

    bool operator==(Node const& rhs) const { return c == rhs.c && n == rhs.n; }
  };

  auto make_node = [](std::tuple<char, std::vector<Node>> v) {
    return Node{std::get<0>(v), std::get<1>(v)};
  };

  Recur<struct abc, Node> recur(pool);

  auto recur_pat = (char_() >> '(' >> *recur >> ')')[make_node];

  ABU_Recur_define(recur, abc, recur_pat);

  Node expected;
  expected.c = 'a';
  expected.n.resize(2);
  expected.n[0].c = 'b';
  expected.n[1].c = 'c';

  testPatternSuccess("a(b()c())", recur, expected);
}

TEST(test_recur, multi_recur_test) {
  RecurMemoryPool pool;

  struct SubNode {
    int c;
    std::vector<SubNode> s;

    bool operator==(SubNode const& rhs) const {
      return c == rhs.c && s == rhs.s;
    }
  };

  struct Node {
    int c;
    std::vector<Node> n;
    std::vector<SubNode> s;

    bool operator==(Node const& rhs) const {
      return c == rhs.c && s == rhs.s && n == rhs.n;
    }
  };

  auto make_node =
      [](std::tuple<int, std::vector<Node>, std::vector<SubNode>> p) {
        return Node{std::get<0>(p), std::get<1>(p), std::get<2>(p)};
      };

  auto make_sub_node = [](std::tuple<int, std::vector<SubNode>> p) {
    return SubNode{std::get<0>(p), std::get<1>(p)};
  };

  Recur<struct abc, Node> recur(pool);
  Recur<struct def, SubNode> sub_recur(pool);

  auto sub_recur_pat = (uint_ >> '(' >> *sub_recur >> ')')[make_sub_node];
  auto recur_pat =
      (uint_ >> '(' >> *recur >> ')' >> '[' >> *sub_recur >> ']')[make_node];

  ABU_Recur_define(recur, abc, recur_pat);
  ABU_Recur_define(sub_recur, def, sub_recur_pat);

  Node expected;

  expected.c = 1;
  expected.s.resize(2);
  expected.s[0].c = 2;
  expected.s[1].c = 3;
  expected.s[1].s.resize(1);
  expected.s[1].s[0].c = 4;

  testPatternSuccess("1()[2()3(4())]", recur, expected);
}

// Reproduces https://github.com/FrancoisChabot/abulafia/issues/17
template <typename PAT_T>
auto as_recur(PAT_T const& p, RecurMemoryPool& pool) {
  Recur<struct as_recur_t> as_recur(pool);
  ABU_Recur_define(as_recur, as_recur_t, p);
  return as_recur;
}

TEST(test_recur, recur_delegate_to_template) {
  RecurMemoryPool pool;
  auto pat = Int<10, 2, 2>();

  testPatternSuccess("12", as_recur(pat, pool), 12);
}
