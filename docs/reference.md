## Character Sets:
pattern                        | Macthes                      |
-------------------------------|------------------------------|
`any<T>`                       | any character                |
`single(v)`                    | ` == v`                      |
`range(v_1, v_2)`              | `v_1 <=  <= v_2`             |
`delegated(cb)`                | `cb()`                       |
`not(charset)`                 | `!charset.has()`             |
`~charset`                     | `not(charset)`               |
`or(cs_1, cs_2)`               | `cs_1.has(v) || cs_2.has(v)` |
`cs_1 | cs_2`                  | `or(CS_1, CS_2)`             |
`set("abc")`                   | `a | b | c`                  |
`set(ITE, ITE)`                | any character contained      |


## Patterns

### Character Patterns
pattern                         | compatible dst      | Behavior                                                   
--------------------------------|---------------------|------------------------------------------------------------
`char_()`                       | CHAR_T              | Matches any single character.
`char_(charset<T>)`             | CHAR_T              | Matches a single character belonging to the character set.
`char_('c')`                    | CHAR_T              | Matches that one specific character.
`char_("string")`               | CHAR_T              | Matches a single character found in the string

### Numeric Patterns
pattern                         | compatible dst      | Behavior                                                   
--------------------------------|---------------------|------------------------------------------------------------
`Int<BASE, MIN_DIG, MAX_DIG>()` | integer-like        | Matches a signed number in base BASE, having between MIN_DIG and MAX_DIG (0 means no limit) digits.
`int_`                          | integer-like        | Shorthand for Int<10, 1, 0>().
`UInt<BASE, MIN_DIG, MAX_DIG>()`| integer-like        | Matches an unsigned number in base BASE, having between MIN_DIG and MAX_DIG (0 means no limit) digits.
`uint_`                         | integer-like        | Shorthand for UInt<10, 1, 0>().

### Literal Patterns
pattern                         | compatible dst      | Behavior                                                   
--------------------------------|---------------------|------------------------------------------------------------
`lit('c')`                      | Nil                 | Matches and ignores a specific character.
`lit("str")`                    | Nil                 | Matches and ignores a specific string of characters.

### Symbol Patterns
pattern                         | compatible dst      | Behavior                                                   
--------------------------------|---------------------|------------------------------------------------------------
`symbol(map<CHAR_T, VAL>)`      | VAL                 | Matches character based on map key, and emits matching value.
`symbol(map<string, VAL>)`      | VAL                 | Matches string based on map key, and emits matching value.

### Misc Patterns
pattern                         | compatible dst      | Behavior                                                   
--------------------------------|---------------------|------------------------------------------------------------
`eoi`                           | Nil                 | Matches the end of input.
`pass`                          | Nil                 | Always succeeds.
`fail`                          | Nil                 | Always fails.

### Unary Patterns
Name   | operator     | compatible dst      | Behavior                                                   
-------|--------------|---------------------|------------------------------------------------------------
Not    | `!pat`       | Nil                 | Fails if the operand matches, does nothing otherwise.
Repeat | `*pat`       | `vector<pat_dst>`   | Matches pat zero or more times.
Repeat | `+pat`       | `vector<pat_dst>`   | Matches pat one or more times.
Action | `pat[act]`   | `decltype(act())`   | Executes Act if pat succeeds.

### Binary Patterns
Name   | operator     | compatible dst      | Behavior                                                   
-------|--------------|---------------------|------------------------------------------------------------
Except | `op - neg`   | `op_dst`            | Delegates to `op` if neg fails.
List   | `op % sep`   | `vector<op_dst>`    | Matches a list of `sep` seprated `op`s (0->inf).

### Nary Patterns
Name        | operator        | compatible dst      | Behavior                                                   
------------|-----------------|---------------------|------------------------------------------------------------
Alternative | `A | B | ...`   | **see below**       | Matches the first non-failing child pattern
Sequence    | `A >> B >> ...` | **see below**       | Matches each sub-pattern sequentially

**Alternative dst**: Any type that can accept an atomic assignment of any of A, B, ...

**Sequence dst**:

- If all child patterns are `Nil`: `Nil`
- If all child patterns except `pat` are `Nil` patterns: Anything `pat` is compatible with.
- if one child is `vector<T>, and all childs are `T` or `vector<T>`: `vector<T>`.
- Otherwise: dst must be a type that can be constructed with each of the non-Nil child patterns

### Recur

```c++
auto create_pattern() {
  abu::Recur<struct MyRecur> my_recur;

  auto pat = <pattern making use of my_recur>;

  // must be in same scope as my_recur
  ABU_Recur_Define(MyRecur, my_recur, pat);

  return my_recur;
}
```

## Data Sources

# SingleForward

# ContainerSequence
