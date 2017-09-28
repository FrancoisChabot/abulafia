//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_skipper, basic_test) {
  auto pattern = int_;
  auto skip_pat = char_(' ');

  std::string data = " 12";

  SingleForwardContext<std::string::iterator> ctx(std::begin(data),
                                                  std::end(data));
  SkipperAdapter<decltype(ctx), decltype(skip_pat)> skipping_ctx(ctx, skip_pat);

  int dst;
  auto parser = make_parser_(skipping_ctx, dst, pattern);

  auto success = parser.consume(skipping_ctx, dst, pattern);

  EXPECT_EQ(result::SUCCESS, success);
  EXPECT_EQ(12, dst);
}

TEST(test_skipper, multi_skip) {
  auto pattern = int_;
  auto skip_pat = char_(' ');

  std::string data = "     12";

  SingleForwardContext<std::string::iterator> ctx(std::begin(data),
                                                  std::end(data));
  SkipperAdapter<decltype(ctx), decltype(skip_pat)> skipping_ctx(ctx, skip_pat);

  int dst;
  auto parser = make_parser_(skipping_ctx, dst, pattern);

  auto success = parser.consume(skipping_ctx, dst, pattern);

  EXPECT_EQ(result::SUCCESS, success);
  EXPECT_EQ(12, dst);
}

TEST(test_skipper, multi_skip_complex_pattern) {
  auto pattern = int_;
  auto skip_pat = lit("-*") >> lit("*-");

  std::string data = "-**--**-12";

  SingleForwardContext<std::string::iterator> ctx(std::begin(data),
                                                  std::end(data));
  SkipperAdapter<decltype(ctx), decltype(skip_pat)> skipping_ctx(ctx, skip_pat);

  int dst;
  auto parser = make_parser_(skipping_ctx, dst, pattern);

  auto success = parser.consume(skipping_ctx, dst, pattern);

  EXPECT_EQ(result::SUCCESS, success);
  EXPECT_EQ(12, dst);
}

TEST(test_skipper, test_list_skipping) {
  auto pattern = int_ % ',';
  auto skip_pat = lit(' ');

  std::string data = "1, 3, 4, 5 , 6";

  SingleForwardContext<std::string::iterator> ctx(std::begin(data),
                                                  std::end(data));
  SkipperAdapter<decltype(ctx), decltype(skip_pat)> skipping_ctx(ctx, skip_pat);

  std::vector<int> dst;
  auto parser = make_parser_(skipping_ctx, dst, pattern);

  auto success = parser.consume(skipping_ctx, dst, pattern);

  EXPECT_EQ(result::SUCCESS, success);
  EXPECT_EQ(std::vector<int>({1, 3, 4, 5, 6}), dst);
}

TEST(test_skipper, test_api) {
  auto pattern = int_;
  auto skip_pat = char_(' ');

  auto skipping_pat = apply_skipper(pattern, skip_pat);

  std::string data = " 12";
  int dst;

  auto success = parse(skipping_pat, data, dst);
  EXPECT_EQ(result::SUCCESS, success);
  EXPECT_EQ(12, dst);
}