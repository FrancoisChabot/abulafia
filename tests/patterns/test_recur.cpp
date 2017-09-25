//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"
#include "gtest/gtest.h"

using namespace abu;

// Test basic syntax with any recursivity involved.
TEST(test_recur, attr_lookup_simple) {
  using ctx_t = SingleForwardContext<std::string::iterator>;

  struct abc;
  Recur<abc, std::vector<char>> recur;

  // Even if it won't be usefull, there is no reason attr lookup cannot work
  // before definition
  expect_same<std::vector<char>, attr_t<decltype(recur), ctx_t>>();
  ABU_Recur_define(recur, abc, *char_());

  expect_same<std::vector<char>, attr_t<decltype(recur), ctx_t>>();
}

TEST(test_recur, attr_lookup_recur) {
  using ctx_t = SingleForwardContext<std::string::iterator>;

  struct abc;
  Recur<abc, std::vector<char>> recur;

  auto loop = *recur;
  // Even if it won't be usefull, there is no reason attr lookup cannot work
  // before definition

  ABU_Recur_define(recur, abc, loop);

  expect_same<std::vector<char>, attr_t<decltype(loop), ctx_t>>();
  expect_same<std::vector<char>, attr_t<decltype(recur), ctx_t>>();
}

// Test basic syntax with any recursivity involved.
TEST(test_recur, simple_test) {
  struct abc;
  Recur<abc, std::vector<char>> recur;

  ABU_Recur_define(recur, abc, *char_());

  std::string dst;
  auto status = parse(recur, std::string("abc"), dst);
  EXPECT_EQ(status, result::SUCCESS);
  EXPECT_EQ("abc", dst);
}

// Test with simple recursivity
TEST(test_recur, recur_test) {
  Recur<struct abc> recur;

  auto recur_pat = Int<10, 2, 2>() >> *recur;

  ABU_Recur_define(recur, abc, recur_pat);

  //  std::vector<int> dst;
  auto status = parse(recur, std::string("123456"));

  EXPECT_EQ(status, result::SUCCESS);
  // EXPECT_EQ(std::vector<int>({12,34,56}), dst);
}

/*

TEST(test_recur, sub_recur_test) {
  Recur<struct abc> recur;
  Recur<struct def> sub_recur;

  auto sub_recur_pat = ( uint_ >> '(' >> *sub_recur >> ')' );
  auto recur_pat = ( uint_ >> '(' >> *recur >> ')' >> '[' >> *sub_recur >> ']'
);
  

  ABU_Recur_define(recur, abc, recur_pat);
  ABU_Recur_define(sub_recur, def, sub_recur_pat);
  


  auto status = parse(recur, std::string("1()[2()3(4())]"));
  EXPECT_EQ(status, result::SUCCESS);
}
TEST(test_recur, chained_recur_test) {
  

  Recur<struct abc> recur;
  Recur<struct def> sub_recur;


  auto sub_recur_pat = uint_ >>
                        '(' >> *recur >> ')' >>
                        '[' >> *sub_recur >> ']';

  auto recur_pat = uint_ >>
                     '(' >> *recur >> ')' >>
                     '[' >> *sub_recur >> ']';
  

  ABU_Recur_define(recur, abc, recur_pat);
  ABU_Recur_define(sub_recur, def, sub_recur_pat);
  


  auto status = parse(recur, std::string("1()[2()[]3()[4()[]]]"));
  EXPECT_EQ(status, result::SUCCESS);
}*/