# Skippers

## Skipping whitespace is a common task

A very common pattern is allowing arbitrary white space between elements of a given grammar.

This could be acheived like this:
```c++
// a list of 0 to N characters in the whitespace char set
// that emits nothing to the destination.
// ows stands for Optional White Space
auto ows = ignore(
  *char_(" \r\n\t")
  );

// a list of integers, that ignores any whitespace before it.
auto pattern = *(ows >> int_);
```

However, this can quickly become tedious, and makes parsers hard to read:
```c++
auto alnum = char_set::range('a', 'z') | 
             char_set::range('A', 'Z') | 
             char_set::range('0', '9');

auto ows = ignore(*char_(" \r\n\t"));

auto name = *char_(alnum);

auto player = ows >> name >> // first name
              ows >> name >> // last name
              ows >> uint_;  // score

parse(player, "John Doe    123");
```
## Introducing Skippers

Thankfully Abulafia provides us with **skippers**, which automate the task of applying whitespace before 
each sub-parser.

```c++
auto alnum = char_set::range('a', 'z') | 
             char_set::range('A', 'Z') | 
             char_set::range('0', '9');

auto name = lexeme(*char_(alnum));

auto whitespace = char_(" \r\n\t");
auto player = name >> name >> uint_;  // first name, last name, score

parse(apply_skipper(player, whitespace), "John Doe    123");
```

`apply_skipper()` will tell abulafia that every parser underneath it will be prefixed with an ignored repetition of the skipper pattern (`whitespace` in our example).

On the other side of the coin, `lexeme()` tells abulafia that the parser is to be considered as a logical unit, effectively disabling the skipper. Without it, the first name would be parsed as "JohnDoe123", since the white space between the letters would get skipped.
