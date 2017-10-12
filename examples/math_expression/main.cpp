//  Copyright 2017 Francois Chabot
//  (francois.chabot.dev@gmail.com)
//
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include "abulafia/abulafia.h"

#include <iostream>

// This simple example shows how to build a simple operator precedence parser
// using abulafia.
// This parser will evaluate the expression as its being parsed, storing the
// result directly in the dst. Building an AST is a very similar process, which
// you can see in (insert example reference here)
auto make_expr_pattern(abu::RecurMemoryPool& pool) {
  // Simply assign a bound dst's value to the child parser's result
  auto assign = [](int v, auto p) -> void { p.bound_dst.get() = v; };

  // create a Binary operation, using the bound dst's current value
  // as lhs, the child's first value as operator
  // and the child's second value as rhs
  auto make_binop = [](std::tuple<char, int> v, auto p) -> void {

    auto lhs = p.bound_dst.get();
    auto rhs = std::get<1>(v);

    switch (std::get<0>(v)) {
      case '+':
        p.bound_dst.get() = (int)lhs + rhs;
        break;
      case '-':
        p.bound_dst.get() = (int)lhs - rhs;
        break;
      case '*':
        p.bound_dst.get() = (int)lhs * rhs;
        break;
      case '/':
        p.bound_dst.get() = (int)lhs / rhs;
        break;
    }
  };

  abu::Recur<struct expr_t, int> expr(pool);
  auto primary = abu::int_ | ('(' >> expr >> ')');

  auto muldiv = abu::bind_dst(primary[assign] >>
                              *(abu::char_("*/") >> primary)[make_binop]);

  auto addsub = abu::bind_dst(muldiv[assign] >>
                              *(abu::char_("+-") >> muldiv)[make_binop]);

  ABU_Recur_define(expr, expr_t, addsub);

  return abu::apply_skipper(expr, abu::lit(' '));
}

int main(int, const char* []) {
  abu::RecurMemoryPool pool;
  auto pattern = make_expr_pattern(pool);

  while (1) {
    std::cout << "enter a mathematical expression:\n";
    std::string expr;
    std::getline(std::cin, expr);
    if (expr.empty()) {
      break;
    }
    int dst = 0;
    auto status = abu::parse(expr.begin(), expr.end(), pattern, dst);

    if (abu::Result::SUCCESS == status) {
      std::cout << dst << "\n";
    } else {
      std::cout << "expression parse failure\n";
    }
  }
  return 0;
}
