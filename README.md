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

## Efficiency

[Dan Luu reports some general time efficiency
numbers](https://danluu.com/integer-overflow/).

This implementation is intentionally naive, so that it is easy to understand and
maintain. It might not be as efficient as absolutely possible. However, it
should be in the ballpark of Microsoft’s SafeInt, Chromium’s numerics, and what
you could do by hand.

Separate from run-time speed, adding integer overflow checks (as `trapping<T>`,
`trapping_mul`, etc. do) increases object code size proportional to how many
checking call sites you have. `integers` aims to reduce the magnitude of the
code size increase in `NDEBUG` builds. (Note the implementation in trap.h.)

## Acknowledgements

Special thanks to Jan Wilken Dörrie and Dana Jansens for the help in
implementation and general C++ advice.

## See Also

These functions and classes were inspired in part by Rust’s
[`std::num::Wrapping`](https://doc.rust-lang.org/std/num/struct.Wrapping.html),
[`std::intrinsics::wrapping_add`](https://doc.rust-lang.org/std/intrinsics/fn.wrapping_add.html),
and
[`std::intrinsics::add_with_overflow`](https://doc.rust-lang.org/std/intrinsics/fn.add_with_overflow.html).
This library generalizes the idea and brings it all into a single package for
C++.

There are many efforts to provide better integer semantics for C++. You might
like to check them out:

* [Chromium’s //base/numerics library](https://source.chromium.org/chromium/chromium/src/+/main:base/numerics/README.md)
* [SafeInt](https://github.com/dcleblanc/SafeInt)
* [type\_safe](https://github.com/foonathan/type_safe)
* [Boost Safe Numerics](https://www.boost.org/doc/libs/1_77_0/libs/safe_numerics/doc/html/introduction.html)

## TODO

TODO: Consider providing some operations that don’t exist (such as rotate) for
standard C and C++ integers.

`integers` will have a complete test suite. That’s a TODO in progress, along
with the rest of the implementation work. Currently only `trapping<T>` and its
helper functions are implemented and tested.

For comments, constructive criticism, patches, help, et c., please feel free to
file a GitHub issue or send a pull request! See
[Contributing](docs/contributing.md) and [the Code Of
Conduct](docs/code-of-conduct.md).
