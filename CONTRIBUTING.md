## Before you submit a pull request:

### Add tests if useful.

If you are fixing a bug, please add a unit test reproducing the bug. If you are adding a 
feature, please add a reasonable amount of test coverage.

### Run all tests.

We won't look at a pull request unless all existing unit tests pass

### run clang-format on all your changes.

If you are lazy, you can simply clang-format everything:

    find include -iname *.h | xargs clang-format -i -style=file
    find tests -iname *.h -o -iname *.cpp | xargs clang-format -i -style=file
    find examples -iname *.h -o -iname *.cpp | xargs clang-format -i -style=file

## Coverage testing

Automated coverage testing has been enabled, but please do not realy too much on it. However, due to the highly templated nature of Abulafia, its important not to rely on it too much.