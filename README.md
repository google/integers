# Integers

`integers` is a package for C++ that implements integer types with explicit,
well-defined behavior (of your choice!) on overflow, underflow, division by 0,
casting, and shifting too far.

`integers` is developed and maintained by members of the Fuchsia Security Team,
but it is not an official Google product.

Defined behaviors are trapping (`trapping<T>` and associated stand-alone helper
functions), wrapping (`wrapping<T>` and helpers), or clamping/saturating
(`clamping<T>` and helpers). You choose your preferred policy/behavior by using
the stand-alone template helper functions and/or the template classes.

There is also a `ranged<T>` template class for situations where you need a type
that constrains integers to a specific range of values.

The main goals of this library are correctness and usability. Ideally, you can
simply drop in the right type for your situation, and the rest of your code
works as expected — the template classes should be fully compatible with the
built-in primitive types. (If you find any gaps in functionality, that’s a bug!
Please file an issue or send a pull request.)

Another goal is that this library should be easy to import and use, without any
transitive dependencies or other complications. `integers` is a headers-only
library. (See [Installation](#installation).)

`integers` is tested to work with C++17 and C++20.

## Usage

An example use case for `integers` is file format parsers and deserializers and
so on, which are constantly calculating and re-calculating offsets and indices
into files. It is all too easy to have mistakes in that kind of code, with the
classic C integer semantics.

You can see a simple example of 4 ways to use the trapping helper functions and
the `trapping` template class in demo.cc. It shows a simple example of code that
is vulnerable to integer overflow, and ways to fix it.

For full documentation, see the Markdown comments in the header files.

## Installation

To install, simply copy the header files into a place that your compiler can see
them. You can do this manually, or you can edit the Makefile to set your desired
`INSTALL_DIR`, and then run `make install`.

## Acknowledgements

Special thanks to Jan Wilken Dörrie and Dana Jansens for the help in
implementation and general C++ advice.

## TODO

TODO: Consider providing some operations that don’t exist (such as rotate) for
standard C and C++ integers.

`integers` will have a complete test suite. That’s a TODO in progress, along
with the rest of the implementation work. Currently only `trapping<T>` and its
helper functions are implemented and tested.

For comments, constructive criticism, patches, help, et c., please feel free to
file a GitHub issue or send a pull request! See [docs/contributing.md] and
[docs/code-of-conduct.md].
