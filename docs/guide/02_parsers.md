# Parsers

At its core, Abulafia builds parsers by combining three components:

- A Pattern
- A Context
- A Destination (or `nil`)

## Patterns

Patterns describe the expected structure of the data we want to parse. They can be as simple as expecting a single character, or describe grammars as complex as C++ itself. Most importantly, many parser classes provided by Abulafia take other parsers as arguments and compose them into more complicated behaviors.

Because composed patterns generate very complicated types through templates, they are generally stored in type-infered variables:

```c++
auto my_pattern = int_ >> ',' >> *char('a', 'z');
```

Furthermore, patterns are composed using **value semantics**. This means that writing reusable grammars is mostly a matter of creating a function that returns the proper pattern. This is the prefered way to create custom pattern types.[^1]
```c++
// Parses hexadecimal numbers, expecting them to be prefixed by the standard 0x
template<std::size_t MIN_DIGITS, std::size_t MAX_DIGITS>
auto prefixed_hex() {
  return (lit("0x") | lit("0X") >> UInt<16, MIN_DIGITS, MAX_DIGITS>();
}
```

## Contexts

Contexts are primarily used to specify how the data is being fed to the parsers. 

The simplest of these is `SingleForwardContext<>`. It takes a pair of iterators, and assumes that they represent the entire totality of the data the parser will consume.

## Destinations

Quite simply where the parser will store any and all extracted data. A parser will accept any type that are compatible with the assingments being performed in the parser.

For example:

```c++
// A list of integers
auto pattern = *int_;

// These are all valid dst for our pattern
std::vector<int> dst_1;
std::vector<float> dst_2;
std::list<std::uint64_t> dst_3;
```

### The Nil Destination

Abulafia provides a special type: `Nil`, with a matching constant: `nil`, that represents the concept of nothingness. Every pattern will always accept `nil` as a valid destination. Its use causes all parsers to stop generating any data[^2].


## Putting it all together

```c++
#include <string>
#include <vector>
#include <cassert>
#include "abulafia/abulafia.h"

int main() {
  std::string data = "123456";

  // pattern
  // A list of two-digits decimal-10 unsigned integers
  auto pattern = *abu::UInt<10,2,2>();

  // context
  // Consume a pair of `std::string` iterators all at once
  abu::SingleForwardContext<std::string::const_iterator> context(data.begin(), 
                                                                 data.end());
 
  // destination
  std::vector<unsigned short> destination;
 

  // Together, they make a parser
  auto parser = abu::make_parser(pattern, context, destination);

  auto status = parser.consume();

  assert(status == abu::Result::SUCCESS);
  assert(destination.size() == 3);
  assert(destination[0] == 12);
  assert(destination[1] == 34);
  assert(destination[2] == 56);

  return 0;
}
```

## Making it cleaner
This is all fairly verbose. Since parsing a container all at once is such a common pattern, Abulafia provides a shorthand method.
Simple patterns can also be inlined directly into the parse call.

```c++
#include <string>
#include <vector>
#include <cassert>

#include "abulafia/abulafia.h"

int main() {
  std::vector<unsigned short> destination;

  auto status = abu::parse(*abu::UInt<10, 2, 2>,
                           "123456",
                           destination);

  assert(status == abu::result::SUCCESS);

  return 0;
}
```

[^1]:
    Abulafia itself uses this approach when it makes sense. Have a look at `abulafia/patterns/leaf/char_literal.h` for example.
[^2]:
    This is a bit of a oversimplification. The parsers still generate data, but `Nil`'s assignments are all no-ops, so the compiler optimizes 
    away the entire assignment routine of the parsers.
