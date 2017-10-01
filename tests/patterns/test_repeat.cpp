//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

TEST(test_repeat, string_test) {
  auto pattern = *char_();

  std::string dst;
  auto status = parse(pattern, std::string("abc"), dst);  
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ("abc", dst);
}

TEST(test_repeat, simple_test) {
  auto pattern = *Uint<10, 2, 2>();

  std::vector<unsigned int> dst;

  auto status = parse(pattern, "2345", dst);
  EXPECT_EQ(status, result::SUCCESS);
  ASSERT_EQ(2U, dst.size());
  EXPECT_EQ(23U, dst[0]);
  EXPECT_EQ(45U, dst[1]);

  status = parse(pattern, "abc");
  EXPECT_EQ(status, result::SUCCESS);

  status = parse(pattern, "");
  EXPECT_EQ(status, result::SUCCESS);
}

TEST(test_repeat, test_plus) {
  auto pattern = +Uint<10, 2, 2>();

  std::vector<unsigned int> dst;

  auto status = parse(pattern, "23", dst);
  EXPECT_EQ(status, result::SUCCESS);
  ASSERT_EQ(1U, dst.size());
  EXPECT_EQ(23U, dst[0]);

  status = parse(pattern, "abc");
  EXPECT_EQ(status, result::FAILURE);

  status = parse(pattern, "");
  EXPECT_EQ(status, result::FAILURE);
}

TEST(test_repeat, test_max_count) {
  auto pattern = repeat<0, 3>(Uint<10, 1, 1>());

  std::vector<unsigned int> dst;

  auto status = parse(pattern, "1234", dst);
  EXPECT_EQ(status, result::SUCCESS);
  ASSERT_EQ(3U, dst.size());
  EXPECT_EQ(1U, dst[0]);
  EXPECT_EQ(2U, dst[1]);
  EXPECT_EQ(3U, dst[2]);
}

TEST(test_repeat, test_chained_repeat_with_min) {
  auto pattern = *(repeat<2, 2>(char_('a', 'z')));

  std::string dst;

  auto status = parse(pattern, "aabbcc", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ("aabbcc", dst);
/*
  std::vector<std::string> dst_as_vector_of_string;
  status = parse(pattern, "aabbcc", dst_as_vector_of_string);
  EXPECT_EQ(status, result::SUCCESS);
  ASSERT_EQ(3, dst_as_vector_of_string.size());
  EXPECT_EQ("aa", dst_as_vector_of_string[0]);
  EXPECT_EQ("bb", dst_as_vector_of_string[1]);
  EXPECT_EQ("cc", dst_as_vector_of_string[2]);
  */
  // make sure that failures do not affect result
  status = parse(pattern, "aabbccd", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ("aabbcc", dst);
}
/*
TEST(test_repeat, test_chained_repeat_with_min_double) {
  // this will cause us to insert() on a wrapper
  auto pattern = *+(repeat<2, 2>(char_('a', 'z')));

  std::string dst;

  auto status = parse(pattern, "aabbcc", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ("aabbcc", dst);

  // make sure that failures do not affect result
  status = parse(pattern, "aabbccd", dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ("aabbcc", dst);
}
*/