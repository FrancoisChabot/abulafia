# Abulafia
A C++ parsing framework

[![build][badge.build]][build]
[![build_win][badge.build_win]][build_win]
[![coverage][badge.coverage]][coverage]
[![on-godbolt][badge.on-godbolt]][on-godbolt]

[badge.build]: https://travis-ci.org/FrancoisChabot/abulafia.svg?branch=master
[badge.build_win]: https://ci.appveyor.com/api/projects/status/8013l0w05xgj76a0/branch/master?svg=true
[badge.coverage]: https://coveralls.io/repos/github/FrancoisChabot/abulafia/badge.svg?branch=master
[badge.on-godbolt]: https://img.shields.io/badge/on-godbolt-376666.svg

[build]: https://travis-ci.org/FrancoisChabot/abulafia
[build_win]: https://ci.appveyor.com/project/FrancoisChabot/abulafia/branch/master
[coverage]: https://coveralls.io/github/FrancoisChabot/abulafia?branch=master
[on-godbolt]: https://godbolt.org/g/Zq7Eb1

Abulafia is a C++ expression templates library that generates parsers. The idea is to let programs contain ABNF-like grammars embedded directly within the code. It's heavily inspired by Boost::spirit::QI, but differs fundamentally from it in a number of respects.

**Quick links**
- [Documention](https://francoischabot.github.io/abulafia/)
- [Cheatsheet](https://francoischabot.github.io/abulafia/reference/)
- [Single Header](https://raw.githubusercontent.com/FrancoisChabot/abulafia/single_header/include/abulafia/abulafia_all.h)

**Warning**

Abulafia is still in early development. While many features work as intended, there are still a LOT of work to be done before it can 
be considered usable in a real environment.

At the moment, validating parsers (parsers with `Nil` as the destination type) "should" work fine accross the board. Feel free to report any and all bugs you encounter related to them. 

Parsing to dst still needs a lot of love. The sequence pattern in particular only handles very basic cases right now.

## Requirements

Abulafia currently requires a C++17 compiler. The level of C++17 compatibility required will be kept at the lowest bar set by the current release of the following compilers:

- Microsoft VC++ (2017, update 3)
- Clang (5.0.0)
- GCC (7.2)

Not only will abulafia compile successfully on these compilers, but any code using abulafia's API will as well. This is why, for example, there are no template deduction guides, even if they would make for a nicer interface when dealing with character sets.

## Installation

Abulafia is a headers-only library. In order to use it, you just have to make the contents of the `include` directory available to your compiler. This can be done by either copying its contents to your project's source tree, or setting your compiler's include directories.

## Features

- **No external dependencies**: The only requirement is a valid C++ compiler.
- **Header-based**: There is no building or linking involved, just use the headers in your project.
- **Type safety**: Abulafia goes to a lot of trouble to ensure its parsers are valid at compile-time. If your code compiles, the parsers are valid.
- **Performance**: Parsers are heavily optimized during compilation, resulting in parsing code that's often faster than hand-written parsers.
- **Resumable parsers**: Parsing can be done gradually, as data becomes available. There is no need to accumulate the full source before beginning parsing. 
- **Parser library**: Abulafia comes with a full library of core parsers that can be composed using overloaded operators. 
- **Custom parsers**: In the unlikely event that the provided parsers cannot fullfill a project's needs, extending the library with new parsers is fairly straightforward.

## Usage

With abulafia, you can write surprisingly complex parsers in a few, very readable, lines of code:
[comment]: <If you change this code, make sure tests/documentation/test_readme.cpp is updated to match.>
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
  return 0;
}
```

## Documentation

You can find up-to-date documention [here](https://francoischabot.github.io/abulafia/).

### Building the documentation

In order to build the documentation, you will need [mkdocs](http://www.mkdocs.org/) and the [material theme](http://squidfunk.github.io/mkdocs-material/). Simply run `mkdocs serve` from the project directory to serve the documentation locally (useful for previewing documentation updates before pushing).
