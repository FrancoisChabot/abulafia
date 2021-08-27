# Abulafia
(A-bu-la-fee-a)

A standalone C++ parsing framework

[![CI](https://github.com/FrancoisChabot/abulafia/actions/workflows/ci.yml/badge.svg?branch=v2)](https://github.com/FrancoisChabot/abulafia/actions/workflows/ci.yml)


# Quick Start:

```cpp
#include <iostream>
#include <cctype>

#include "abulafia/abulafia.h"

int main() {
    // Any token predicate is a character set.
    constexpr auto letter = abu::tok(std::isgraph);
    constexpr auto blank = abu::tok(std::isblank);
    constexpr auto words = *(discard(*blank) >> +letter);

    auto result = abu::parse("this is my data", words);
    for(const auto & word : *result) {
        std::cout << word << "\n";
    }
}
```


## V2

A major V2 rewrite is in progress.

Progress:

- [x] Basic repo setup
- [x] Automated testing
- [ ] token sets
- [ ] basic pattern library
- [ ] semantic actions
- [ ] recursive patterns
- [ ] input range rollback
- [ ] streams

## Usage

Using Abulafia is a two stage process: Creating patterns and parsing data

### Create Patterns

```
```

### Parse Data

- `parse(range, pattern)` Will parse and extract data
- `check(range, pattern)` Will only validate the data agains `pattern`


