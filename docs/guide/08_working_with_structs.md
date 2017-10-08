# Working with user types

Abulafia allows you to map patterns to arbitrary types as long as these types have the appropriate constructor as well as a default constructor.

This is done by using the `Construct` patterm, which can be conveniently invoked by usage of the `as()` method on any parser. 

`as<T...>()` will create a `std::tuple<T...>` into which the attached pattern will be parsed. Upon successfull completion of the child parse operation, `std::construct_from_tuple()` will be invoked on whatever type was passed as the DST.

```c++

#include "abulafia/abulafia.h"

struct Rectangle {
  Rectangle() = default;
  Rectangle(float in_w, float in_h) 
    : w(in_w), h(in_h) {}
  
  float w;
  float h;
};

int main() {
  auto rect_pattern = (abu::int_ >> ',' >> abu::int_).as<int, int>();
      
  Rectangle rect_a = abu::decode<Rectangle>("1,45", rect_pattern);
  
  return 0;
}

```

