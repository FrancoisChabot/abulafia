Actions are a means to attach arbitrary pieces of code to patterns. Actions are executed upon successful execution of the attached pattern

## The Basics

Actions are functors that may take an argument and may return a value. They can be attached to parsers using the indexing operator: `[]`.

Example:
```c++
auto action = [](){std::cout << "pattern has matched!" << std::endl;};
auto pattern = *char_()[action];
```

## Taking parameters

If the functor takes a parameter, it will behave as the destination of the attached parser.[^1]

Example:
```c++
auto action = [](char c){std::cout << "found a letter: " << c << std::endl;};
auto pattern = *char_()[action];

```
## Returning a value

If the functor returns a value, that value will be assigned to whatever destination is provided to the action.

Example:
```c++
auto action = [](unsigned char c){return std::toupper(c);};
auto pattern = *char_()[action];

```
[^1]:
  In order to do that, Abulafia needs to create a temporary instance of the parameter for the parser to decode into. This means that the type of the parameter has to be default-constructible.