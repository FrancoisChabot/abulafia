//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_single_forward_context, create_from_string) {
  std::string data = "1234567890";

  SingleForwardDataSource<std::string::iterator> ctx(std::begin(data),
                                                     std::end(data));
  EXPECT_FALSE(ctx.empty());

  EXPECT_EQ('1', ctx.next());
  EXPECT_FALSE(ctx.empty());
  ctx.advance();
  EXPECT_EQ('2', ctx.next());
  ctx.advance();
  EXPECT_EQ('3', ctx.next());
  ctx.advance();
  EXPECT_EQ('4', ctx.next());
  ctx.advance();
  EXPECT_EQ('5', ctx.next());
  ctx.advance();
  EXPECT_EQ('6', ctx.next());
  ctx.advance();
  EXPECT_EQ('7', ctx.next());
  ctx.advance();
  EXPECT_EQ('8', ctx.next());
  ctx.advance();
  EXPECT_EQ('9', ctx.next());
  ctx.advance();
  EXPECT_EQ('0', ctx.next());
  EXPECT_FALSE(ctx.empty());
  ctx.advance();
  EXPECT_TRUE(ctx.empty());
}

TEST(test_single_forward_context, perform_rollback_to_start) {
  std::string data = "1234567890";

  SingleForwardDataSource<std::string::iterator> ctx(std::begin(data),
                                                     std::end(data));

  ctx.prepare_rollback();
  ctx.advance();
  ctx.advance();
  ctx.advance();
  ctx.commit_rollback();
  EXPECT_EQ('1', ctx.next());
}

TEST(test_single_forward_context, perform_rollback_to_middle) {
  std::string data = "1234567890";

  SingleForwardDataSource<std::string::iterator> ctx(std::begin(data),
                                                     std::end(data));
  ctx.advance();
  ctx.advance();

  ctx.prepare_rollback();
  ctx.advance();
  ctx.advance();
  ctx.advance();
  ctx.commit_rollback();
  EXPECT_EQ('3', ctx.next());
}

TEST(test_single_forward_context, perform_rollback_from_empty) {
  std::string data = "1234567890";

  SingleForwardDataSource<std::string::iterator> ctx(std::begin(data),
                                                     std::end(data));

  ctx.prepare_rollback();
  while (!ctx.empty()) {
    ctx.advance();
  }
  ctx.commit_rollback();
  EXPECT_EQ('1', ctx.next());
}

TEST(test_single_forward_context, immediate_rollback) {
  std::string data = "1234567890";

  SingleForwardDataSource<std::string::iterator> ctx(std::begin(data),
                                                     std::end(data));

  ctx.prepare_rollback();
  ctx.commit_rollback();
  EXPECT_EQ('1', ctx.next());
}

TEST(test_single_forward_context, perform_multiple_rollback) {
  std::string data = "1234567890";

  SingleForwardDataSource<std::string::iterator> ctx(std::begin(data),
                                                     std::end(data));

  ctx.prepare_rollback();
  ctx.prepare_rollback();
  ctx.advance();
  ctx.advance();
  ctx.prepare_rollback();

  ctx.advance();
  ctx.advance();

  EXPECT_EQ('5', ctx.next());
  ctx.commit_rollback();
  EXPECT_EQ('3', ctx.next());
  ctx.commit_rollback();
  EXPECT_EQ('1', ctx.next());
  ctx.commit_rollback();
  EXPECT_EQ('1', ctx.next());
}

TEST(test_single_forward_context, cancel_rollback) {
  std::string data = "1234567890";

  SingleForwardDataSource<std::string::iterator> ctx(std::begin(data),
                                                     std::end(data));

  ctx.prepare_rollback();
  ctx.advance();
  ctx.advance();
  ctx.prepare_rollback();

  ctx.advance();
  ctx.advance();

  EXPECT_EQ('5', ctx.next());
  ctx.cancel_rollback();
  EXPECT_EQ('5', ctx.next());
  ctx.commit_rollback();
  EXPECT_EQ('1', ctx.next());
}
