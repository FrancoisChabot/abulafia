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

### Creating Patterns

```cpp

struct domain {
    email(std::vector<std::string> parts) {}
};

struct email {
    email(std::string name, domain dom) {}
};

constexpr auto e_char = abu::tok([](char c)]{
    return (c >= 'a' && c <= 'z') || 
           (c >= 'A' && c <= 'Z') || 
           (c >= '0' && c <= '9') || 
           c == '_' || c == '-' || c == '+' || c == '~'; 
}); 

// A repetition of at least 1 echar
// - Parses as std::string 
constexpr auto identifier = +echar; 

// A comma-delimited list of identifiers
// - Parses as std::vector<std::string>
// - used to construct a domain 
constexpr auto domain_pat = (identifier % '.').as<domain>();

// an identifier followed by @ followed by a domain.
// - Parses as std::tuple<std::string, domain>
// - used to construct an email (by applying the tuple)
constexpr auto email_pat = (identifier >> '@' >> domain_pat).as<email>();



```

### Immediate matching and parsing

```cpp
assert(abu::match<email_pat>("bob@example.com"));

try {
    email value = abu::parse<email_pat>("bob@example.com");
}
catch(abu::match_failure& e) {
    std::cerr << e.what();
}
```

### View parsers

```cpp
std::ifstream emails_file("emails.txt");

abu::view<(*abu::lit(' ') >> email)> emails(
    std::istreambuf_iterator<char>(emails_file), {});

for(const auto& v: emails) {
    std::cout << v << "\n";
}
```

### Resumable matching and parsing

```cpp
using chunk_type = std::vector<char>;

abu::parser<pattern, chunk_type> parser();

while(has_more_data() && !parser.is_done()) {
    parser.advance(get_some_data());
}
parser.finish();

if(parser.status().is_success()) {
    std::cout << parser.value() << "\n">>;
}
else {
    std::cerr << "parse failure: " << parser.status().failure() << "\n">>;
}
```

