# Abulafia
A C++ parsing framework

[![Build Status](https://travis-ci.org/FrancoisChabot/abulafia.svg?branch=master)](https://travis-ci.org/FrancoisChabot/abulafia) https://ci.appveyor.com/api/projects/status/8013l0w05xgj76a0

Abulafia is a C++ expression templates library that generates parsers. The idea is to let programs contain ABNF-like grammars embedded directly within the code. It's heavily inspired by Boost::spirit::QI, but differs fundamentally from it in a number of respects.

## Requirements

Abulafia currently requires a C++17 compiler. The level of C++17 compatibility required will be kept at the lowest bar set by the current release of the following compilers:

- Microsoft VC++ (2017, update 3)
- Clang (5.0.0)
- GCC (7.2)

Not only will abulafia compile successfully on these compilers, but any code using abulafia's API will as well. This is why, for example, there are no template deduction guides, even if they would make for a nicer interface when dealing with character sets.

## Installation

Abulafia is a headers-only library. In order to use it, you just have to make the contents of the `include` directory available to your compiler. This can be done by either copying its contents to your project's source tree, or setting your compiler's include directories.

## Features

- **Type-safety**: Abulafia goes to a lot of trouble to ensure its parsers are valid at compile-time. If your code compiles, the parsers are valid.
- **Performance**: Parsers are heavily optimized during compilation, resulting in parsing code that's often faster than hand-written parsers.
- **Resumable parsers**: Parsing can be done gradually, as data becomes available. There is no need to accumulate the full source before beginning parsing. 
- **Parser library**: Abulafia comes with a full library of core parsers that can be composed using overloaded operators. 
- **Custom parsers**: In the unlikely event that the provided parsers cannot fullfill a project's needs, extending the library with new parsers is fairly straightforward.


## Usage

With abulafia, you can write surprisingly complex parsers in a few, very readable, lines of code:
```c++

#include "abulafia/abulafia.h"

#include <iostream>

int main() {
  // opening bracket, followed by a comma-delimited list of integers, followed by a closing bracket.
  auto with_brackets = '[' >> (abu::int_ % ',') >> ']';
  
  // ignore whitespace
  auto ignoring_whitespace = abu::apply_skipper(with_brackets, abu::char_(" \t\r\n"));

  std::vector<int> dst;
  auto status = abu::parse(ignoring_whitespace, "[1, 2, 3, 4, 5]", dst);

  if(status == abu::result::SUCCESS) {
    for(auto v : dst) {
      std::cout << v << std::endl;
    }
  }
}
```
