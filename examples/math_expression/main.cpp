//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"

#include <iostream>


constexpr int parse_int(std::string_view data) {
  auto pattern = abu::uint_;
  int result = 0;

  abu::parse(data, pattern, result);
  return result;
}
int main(int, const char* []) {
  constexpr int result = parse_int(std::string_view("12", 2));
  std::cout <<  result << std::endl;

  return result;
}
