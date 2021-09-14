# Integers

`integers` is a package for C++ that implements integer types with explicit,
well-defined behavior (of your choice!) on overflow, underflow, division by 0,
casts, and shifting too far.

Defined behaviors are wrapping, trapping, or clamping/saturating. You choose
your preferred policy/behavior by using the stand-alone template helper
functions and/or the template classes. There is also a `ranged<T>` template
class for situations where you need a type that constrains integers to a
specific range of values.

The goal of this library is that you can simply drop in the right type for your
situation, and the rest of your code works as expected — the template classes
should be fully compatible with the built-in primitive types.

An example use-case is file format parsers and deserializers and so on, which
are constantly calculating and re-calculating offsets and indices into files. It
is all too easy to have mistakes in that kind of code, with the classic C
integer semantics.

For full documentation, see the Markdown comments in the header files.

TODO: Consider providing some operations that don’t exist (such as rotate) for
standard C and C++ integers, and a 128-bit type. Consider also: providing ranged
integers, e.g. `ranged<15, 100>` meaning “an integer that can hold values in the
range [15, 100) and which traps if arithmetic sends it out of range.” Find a way
to let the caller specify trapping, wrapping, or clamping.

`integers` will have a complete test suite. That’s a TODO in progress, along
with the rest of the implementation work. Currently only `trapping<T>` and its
helper functions are even started.

For comments, constructive criticism, patches, help, et c., please feel free to
file a GitHub issue! See [docs/contributing.md] and [docs/code-of-conduct.md].
