
### Tests

Running the test suite looks like this:
```
cmake path/to/abulafia
make
ctest
```

### Code coverage

You can run code coverage on the test suite. This is here as a tool. Do not take a 100% result at face value.

```
cmake -DCODE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug path/to/abulafia
make ccov-abulafia_tests
```
