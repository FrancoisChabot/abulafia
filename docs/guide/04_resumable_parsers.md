# Resumable Parsers

Abulafia parsers have the option[^1] of running as coroutines. That is, they are capable of pausing their execution when starved for data, and resume once data is available, all without the need for multithreading and synchronisation.

## ContainerSequenceContext

The `Container Sequence Context` is the prefered way of dealing with data chunks. It's fast, flexible, and does as little memory management work as possible at the cost of possibly holding on to chunks of memory longer than absolutely necessary.[^2]

```c++
#include <vector>

#include "abulafia/abulafia.h"

int main() {
  auto pattern = *abu::UInt<10,2,2>; 
  std::vector<unsigned short> destination;

  // Our resumable context will be provided with vectors of char
  abu::ContainerSequenceContext<std::string> context;

  auto parser = abu::make_parser(pattern, context, destination);

  constexpr int CHUNK_SIZE = 256;
  std::ifstream file("data.txt");

  abu::Result final_result = abu::Result::PARTIAL;
  while(final_result == abu::Result::PARTIAL) {
    auto buffer = std::make_unique<std::vector<char>>(CHUNK_SIZE);     

    bool read_status = file.read(buffer->data(), CHUNK_SIZE);

    buffer->resize(file.gcount());

    // The context takes ownership of the data, and will let go of it as soon
    // as it can prove it won't need it again. No memory copies are involved.
    context.addBuffer(std::move(data), read_status ? abu::IsFinal::NOT_FINAL : abu::IsFinal::FINAL);
  
    final_result = parser.consume();
  }

  // Deal with the status/destination here...

  return 0;
}
```

The most important takeaway here is that `consume()` can return 3 possible values:

Value                  | Meaning
-----------------------|----------------------------------------
`abu::Result::SUCCESS` | The parser has successfully completed
`abu::Result::FAILURE` | The parser has failed
`abu::Result::PARTIAL` | The parser needs more data

Once you have provided a buffer with the `abu::IsFinal::FINAL` flag, the next consume operation will either succeed or fail.

[^1]:
  In classic C++ fashion, you only pay for the overhead of this feature if you actually use it. Using abulafia with single-buffer data sources will result in simple recursive-descent parsers.
[^2]:
 Specifically, if the parser needs to be able to roll back to the very last byte of a buffer, it will keep the entire buffer around.