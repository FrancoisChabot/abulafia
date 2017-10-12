//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"

#include <iostream>


constexpr int twelve() {
  int result = 0;

  abu::parse(std::string_view("?12?", 2), '?' >> abu::uint_ >> '?', result);
  return result;
} 


int foo() {
  return twelve();
}

int main() {return foo();}