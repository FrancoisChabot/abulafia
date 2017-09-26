# Installation

## Requirements

In order to use Abulafia, all you need is a (partially) compliant C++17 compiler, and matching standard library. As of right now, the following environments should be supported:

- GCC (7.2)
- clang (5.0.0)
- Visual Studio 2017 (update 3)

## Installation

Since Abulafia is a header-only library, all you need to do is get the source code from [github](https://github.com/FrancoisChabot/abulafia), and make the contents of the `include` directory available to your compiler.

This can be as simple as copying the include/abulafia directory to your own source tree.

## Configuration

Since Abulafia is a template library, most configuration options are passed as template arguments, which allows multiple configurations of Abulafia to live side-by-side in the same application.

However, some configuration options cannot be exposed this way. These can be found in the `abulafia/config` file, and can be manipulated by preprocessor macros (or outright editing of the config.h file).

As of right now, here are the available configuration options:

Option name       | default value | description
------------------|---------------|--------------------------------------------------
ABULAFIA_NAMESPACE| abu           | The namespace under which all abulafia is defined
