//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_make_parser, simple_usage) {
  std::string data("12");
  SingleForwardContext<std::string::iterator> ctx(data.begin(), data.end());

  int dst = 0;
  auto parser = make_parser(int_, ctx, dst);

  auto status = parser.consume();
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ(12, dst);
}


TEST(test_make_parser, default_into_nil) {
  std::string data("12");
  SingleForwardContext<std::string::iterator> ctx(data.begin(), data.end());

  auto parser = make_parser(int_, ctx);

  auto status = parser.consume();
  EXPECT_EQ(status, result::SUCCESS);
}