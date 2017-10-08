//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_readme, the_readme) {
  // opening bracket, followed by a comma-delimited list of integers, followed
  // by a closing bracket.
  auto with_brackets = '[' >> (abu::int_ % ',') >> ']';

  // ignore whitespace
  auto ignoring_whitespace =
      abu::apply_skipper(with_brackets, abu::char_(" \t\r\n"));

  std::vector<int> dst;
  auto status = abu::parse("[1, 2, 3, 4, 5]", ignoring_whitespace, dst);

  EXPECT_EQ(status, abu::Result::SUCCESS);
  EXPECT_EQ(dst[0], 1);
  EXPECT_EQ(dst[1], 2);
  EXPECT_EQ(dst[2], 3);
  EXPECT_EQ(dst[3], 4);
  EXPECT_EQ(dst[4], 5);
}

struct Rectangle {
  Rectangle() = default;
  Rectangle(int in_w, int in_h) 
    : x(0), y(0), w(in_w), h(in_h) {}
  Rectangle(int in_x, int in_y, int in_w, int in_h) 
    : x(in_x), y(in_y), w(in_w), h(in_h) {}
  
  int x;
  int y;
  int w;
  int h;
};


TEST(test_readme, rectangle) {
  auto space = abu::char_(" \t\r\n");
  auto rect = 
      ('[' >> abu::int_ >> ',' >> abu::int_ >> ']').as<int, int>() |
      ('[' >> abu::int_ >> ',' >> abu::int_ >> ',' >> abu::int_ >> ',' >> abu::int_ >> ']').as<int, int, int, int>();
 
   auto pattern = abu::apply_skipper(rect, space);

  Rectangle rect_a = abu::decode<Rectangle>("[1, 45]", pattern);
  Rectangle rect_b = abu::decode<Rectangle>("[1, 45, 1, 1]", pattern);

  EXPECT_EQ(0, rect_a.x);
  EXPECT_EQ(0, rect_a.y);
  EXPECT_EQ(1, rect_a.w);
  EXPECT_EQ(45, rect_a.h);

  EXPECT_EQ(1, rect_b.x);
  EXPECT_EQ(45, rect_b.y);
  EXPECT_EQ(1, rect_b.w);
  EXPECT_EQ(1, rect_b.h);
  
}
