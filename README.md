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

Examples:
```
constexpr bool valid_email_char(char c) { 
    return (c >= 'a' && c <= 'z') || 
           (c >= 'A' && c <= 'Z') || 
           (c >= '0' && c <= '9') || 
           c == '_' || c == '-' || c == '~'; 
}

constexpr auto e_char = abu::tok(valid_email_char); 
constexpr auto domain = abu::raw(+e_char % '.');
constexpr auto email = +e_char >> '@` >> domain >> abu::eoi;
```

### Immediate matching and parsing

If all the data to parse is available immediately, use the `abu::imm` parse functions.
These will immediately perform the complete parse operation at once.

```
// Check to see if data matches the pattern
abu::imm::match<pattern>(begin, end) -> bool

// Extract value(s) from data according to pattern (throws no_match_error on failure)
abu::imm::parse<pattern>(begin, end) -> auto
```

Examples:
```
assert(abu::match<email>("bob@example.com"));

auto value = abu::parse<pattern>("bob@example.com"); 
assert(std::get<0>(value) == "bob");
assert(std::get<1>(value) == "example.com");
```


### Resumable matching and parsing

If data comes in chunks, `abu::parser` and `abu::matcher` will let
you feed data as it becomes available.

```
using chunk_type = std::vector<char>;

// A parser is good for a single parse operation.
abu::parser<pattern, chunk_type> parser();

while(!parser.status().is_partial()) {
    chunk_type next = get_some_data();
    parser.advance(std::move(next));

    if(no_more_data()) {
        parser.finish();
    }
}

if(parser.status().is_success()) {
    std::cout << parser.value() << "\n">>;
}
else {
    std::cerr << "parse failure: " << parser.failure() << "\n">>;
}
```

Notes:

- Data chunks will be destroyed if the parser is doine with them.