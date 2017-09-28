Many grammars need to include themselves. There are many examples of this: Handling simple mathematical expressions, JSON, XML, etc...

In order to handle this, Abulafia provides the `Recur<>`[^1] pattern. It's by far the most complex stock pattern the library has to offer, but it's extremely versatile.

## How Recur Works

`Recur<>` works by delaying the definition of the pattern through forward-declaration shenanigans. Using it requires a few steps:

1. Forward declare a struct type, say `MyRecur`.
2. Declare an instance of `Recur<MyRecur>`
3. Define a pattern, using that instance as if it was fully defined already.
4. Define MyRecur as struct depending on the type of the resulting pattern.
5. Assign the pattern to the Recur<MyRecur> instance.

## How to use Recur

This 5 step process is extremely convoluted. Thankfully, abulafia provides the `ABU_Recur_Define()` macro that gets rid of most of that boilerplate.

Here's what it looks like in action:

```c++
auto create_pattern() {
  // steps #1 and #2
  abu::Recur<struct MyRecur> my_recur;

  // step #3
  auto pat = int_ | ( '(' >> my_recur % ',' >> ')');

  // steps #4 and #5
  ABU_Recur_Define(MyRecur, my_recur, pat);

  return my_recur; // Could also be: return pat;
}
```
## Declaring a rule pattern.


[^1]:
 spirit::qi handles this through the `Rule<>` parser. Which is a bit of a kitchen sink entry point that was a necessary evil due to the lack of type inference. `abu::Recur<>` does only one thing: handle recursion.