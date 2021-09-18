// Copyright 2021 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef TRAPPING_H_
#define TRAPPING_H_

#include <stdint.h>
#include <stdlib.h>

#include <algorithm>
#include <limits>
#include <type_traits>

#include "trap.h"

namespace {

// Returns true if `divisor` is signed, is -1, and if `dividend` is the minimum
// value for its type. Such division is UB, so must be avoided. This function is
// used in `div_overflow` and `mod_overflow`.
template <typename T, typename U>
bool divide_min_by_negative_1(T dividend, U divisor) {
  // As of C++17, we can assume 2’s complement. (See section 6.8.1 of
  // https://isocpp.org/files/papers/N4860.pdf.)
  return std::is_signed_v<U> && dividend == std::numeric_limits<T>::min() &&
         divisor == -1;
}

}  // namespace

/// # `integers`
///
/// This is a library of template functions and template classes that implement
/// well-defined behavior for integer overflow, underflow, division by zero,
/// narrowing conversions, and over-shifting. You can choose the appropriate
/// behavior by using the functions and classes that are right for your
/// application (e.g. `wrapping<T>`, `trapping_add`, and so on).
///
/// These functions and classes were inspired in part by Rust’s
/// [`std::num::Wrapping`](https://doc.rust-lang.org/std/num/struct.Wrapping.html),
/// [`std::intrinsics::wrapping_add`](https://doc.rust-lang.org/std/intrinsics/fn.wrapping_add.html),
/// and
/// [`std::intrinsics::add_with_overflow`](https://doc.rust-lang.org/std/intrinsics/fn.add_with_overflow.html).
/// This library generalizes the idea and brings it all into a single package
/// for C++.
///
/// This library is also inspired by [Chromium’s //base/numerics
/// library](https://source.chromium.org/chromium/chromium/src/+/main:base/numerics/README.md).
///
/// The implementation is intentionally naive, so that it is easy to understand
/// and maintain. It might therefore not be quite as efficient as possible.
/// However, it should hopefully be within the ballpark of [the numbers Dan Luu
/// reports](https://danluu.com/integer-overflow/).

namespace integers {

/// ## Primitive Checking Operations
///
/// ### `cast_truncate`
///
/// Converts `T`s to `R`s, and returns true if `R` cannot hold the full `value`.
/// (This can happen on some narrowing conversions, and if `value` is signed and
/// < 0 and `R` is unsigned.)
//
// NOTE TODO: When/if we can require callers to use C++20, `std::in_range` and
// `std::cmp_*` could be useful here, e.g.:
//
// template <typename T, typename R>
// constexpr bool cast_truncate(T value, R* result) {
//   if (std::in_range<R>(value)) {
//     *result = static_cast<R>(value);
//     return false;
//   }
//   return true;
// }
template <typename T, typename R>
bool cast_truncate(T value, R* result) {
  // Yes, this implementation looks like A Lot, but most of it is `constexpr`
  // and should be resolved at compile time.

  static_assert(std::is_integral_v<T>, "`T` must be an integral type.");
  static_assert(std::is_integral_v<R>, "`R` must be an integral type.");

  constexpr bool T_signed = std::is_signed_v<T>;
  constexpr bool R_signed = std::is_signed_v<R>;
  constexpr size_t T_size = sizeof(T);
  constexpr size_t R_size = sizeof(R);

#define STATIC_CAST_AND_RETURN     \
  *result = static_cast<R>(value); \
  return false

  // In an earlier version of this function, I tried to be smort and handled
  // some special easy cases early. It did not work (ended up hitting the
  // `NOTREACHED` at the bottom). So, let's just be maximally clear and handle
  // all possible cases with this brute-force if/else if/else tree. All branches
  // except for the correct one for the call site's `T` and `R`, including the
  // `NOTREACHED`s, should be optimized away.

  if constexpr (T_signed && R_signed) {
    if (T_size == R_size) {
      // std::is_same_v<T, R>
      STATIC_CAST_AND_RETURN;
    } else if constexpr (T_size > R_size) {
      if (value > std::numeric_limits<R>::max()) {
        return true;
      }
      STATIC_CAST_AND_RETURN;
    } else if constexpr (T_size < R_size) {
      STATIC_CAST_AND_RETURN;
    } else {
      NOTREACHED();
    }
  } else if constexpr (!T_signed && !R_signed) {
    if (T_size == R_size) {
      // std::is_same_v<T, R>
      STATIC_CAST_AND_RETURN;
    } else if constexpr (T_size > R_size) {
      if (value > std::numeric_limits<R>::max()) {
        return true;
      }
      STATIC_CAST_AND_RETURN;
    } else if constexpr (T_size < R_size) {
      STATIC_CAST_AND_RETURN;
    } else {
      NOTREACHED();
    }
  } else if constexpr (T_signed && !R_signed) {
    if (value < 0) {
      return true;
    }
    if constexpr (T_size == R_size) {
      // This is safe because at this point, we know `value >= 0` and that `R`,
      // which is unsigned, can hold any possible value of `value`.
      STATIC_CAST_AND_RETURN;
    } else if constexpr (T_size > R_size) {
      using U = typename std::make_unsigned<T>::type;
      U unsigned_value = static_cast<U>(value);
      if (unsigned_value > std::numeric_limits<R>::max()) {
        return true;
      }
      STATIC_CAST_AND_RETURN;
    } else if constexpr (T_size < R_size) {
      STATIC_CAST_AND_RETURN;
    } else {
      NOTREACHED();
    }
  } else if constexpr (!T_signed && R_signed) {
    if (value > std::numeric_limits<R>::max()) {
      return true;
    }
    if constexpr (T_size == R_size) {
      STATIC_CAST_AND_RETURN;
    } else if constexpr (T_size > R_size) {
      STATIC_CAST_AND_RETURN;
    } else if constexpr (T_size < R_size) {
      STATIC_CAST_AND_RETURN;
    } else {
      NOTREACHED();
    }
  }

#undef STATIC_CAST_AND_RETURN
}

// NOTE: For `*_overflow`, `Result<R> { R, bool }` instead of returning `bool`
// and writing to an `R*` out-parameter might be more readable and/or may result
// in better object code. Preliminary testing shows that at -O0, the `Result`
// version produces much worse code; at -O1, `Result` is marginally better; and
// at -O2 and higher, both get inlined into oblivion. So, we should decide on
// the basis of readability, since for builders who care about optimization it
// seems essentially not to matter.
//
// Note that `std::optional` would seem to be the right type to use here, but
// since it introduces new UB — the opposite of what we are trying to achieve
// with this library — it is not fit for our purposes. We could use Abseil's
// `optional` and enable Abseil's hardening mode (which defines the UB to
// crash), but we want for this library to be a simple, stand-alone dependency
// that introduces no further dependencies. We could also write our own
// `optional`, but that could be a significant undertaking. For now, we just
// need to get the basic integer functionality implemented and tested, and this
// out-parameter implementation works just fine and produces good enough object
// code.

/// ### `add_overflow`
///
/// Adds `x` to `y` and stores the result in `result` (which can be a pointer to
/// `x`, `y`, or another object). Returns true if the operation overflowed.
template <typename T, typename U, typename R>
bool add_overflow(T x, U y, R* result) {
  static_assert(std::is_integral_v<T>, "`T` must be an integral type.");
  static_assert(std::is_integral_v<U>, "`U` must be an integral type.");
  static_assert(std::is_integral_v<R>, "`R` must be an integral type.");
#if __has_builtin(__builtin_add_overflow)
  return __builtin_add_overflow((x), (y), (result));
#else
#error Use your compiler's intrinsic here.
#endif
}

/// ### `sub_overflow`
///
/// Subtracts `y` from `x` and stores the result in `result` (which can be a
/// pointer to `x`, `y`, or another object). Returns true if the operation
/// overflowed.
///
/// Note: Subtracting 0 does **not** return true. (See `cast_truncate`.)
template <typename T, typename U, typename R>
bool sub_overflow(T x, U y, R* result) {
  static_assert(std::is_integral_v<T>, "`T` must be an integral type.");
  static_assert(std::is_integral_v<U>, "`U` must be an integral type.");
  static_assert(std::is_integral_v<R>, "`R` must be an integral type.");
#if __has_builtin(__builtin_sub_overflow)
  return __builtin_sub_overflow((x), (y), (result));
#else
#error Use your compiler's intrinsic here.
#endif
}

/// ### `mul_overflow`
///
/// Multiplies `x` and `y` and stores the result in `result` (which can be a
/// pointer to `x`, `y`, or another object). Returns true if the operation
/// overflowed.
template <typename T, typename U, typename R>
bool mul_overflow(T x, U y, R* result) {
  static_assert(std::is_integral_v<T>, "`T` must be an integral type.");
  static_assert(std::is_integral_v<U>, "`U` must be an integral type.");
  static_assert(std::is_integral_v<R>, "`R` must be an integral type.");
#if __has_builtin(__builtin_mul_overflow)
  return __builtin_mul_overflow((x), (y), (result));
#else
#error Use your compiler's intrinsic here.
#endif
}

/// ### `div_overflow`
///
/// Divides `dividend` by `divisor` and stores the quotient in `result` (which
/// can be a pointer to `dividend`, `divisor`, or another object). Returns true
/// if the operation overflowed.
///
/// Adapted from
/// https://stackoverflow.com/questions/30394086/integer-division-overflows.
/// Thanks, chux!
template <typename T, typename U, typename R>
bool div_overflow(T dividend, U divisor, R* result) {
  static_assert(std::is_integral_v<T>, "`T` must be an integral type.");
  static_assert(std::is_integral_v<U>, "`U` must be an integral type.");
  static_assert(std::is_integral_v<R>, "`R` must be an integral type.");

  if (divisor == 0) {
    return true;
  }
  if (divide_min_by_negative_1<T, U>(dividend, divisor)) {
    // Instead of generating a floating-point exception, as
    // `std::numeric_limits<T>::min() / -1` can/does.
    return true;
  }
  return cast_truncate(dividend / divisor, result);
}

/// ### `mod_overflow`
///
/// Divides `dividend` by `divisor` and stores the remainder in `result` (which
/// can be a pointer to `dividend`, `divisor`, or another object). Returns true
/// if the operation overflowed.
///
/// Adapted from
/// https://stackoverflow.com/questions/30394086/integer-division-overflows.
/// Thanks, chux!
template <typename T, typename U, typename R>
bool mod_overflow(T dividend, U divisor, R* result) {
  static_assert(std::is_integral_v<T>, "`T` must be an integral type.");
  static_assert(std::is_integral_v<U>, "`U` must be an integral type.");
  static_assert(std::is_integral_v<R>, "`R` must be an integral type.");

  if (divisor == 0) {
    return true;
  }
  if (divide_min_by_negative_1<T, U>(dividend, divisor)) {
    // Instead of generating a floating-point exception, as
    // `std::numeric_limits<T>::min() / -1` can/does.
    return true;
  }
  return cast_truncate(dividend % divisor, result);
}

/// ## Trapping Operations
///
/// ### `trapping_cast`
///
/// Converts `T`s to `R`s, and traps if `R` cannot hold the full `value`. (This
/// can happen on some narrowing conversions, and if `value` is signed and < 0
/// and `R` is unsigned.)
template <typename T, typename R>
R trapping_cast(T value) {
  R result = 0;
  if (cast_truncate(value, &result)) {
    trap();
  }
  return result;
}

/// ### `trapping_add`
///
/// Adds `x` and `y` and returns the result. If the operation overflows, or
/// cannot fit into type `R`, this function will `trap`.
template <typename T, typename U, typename R>
R trapping_add(T x, U y) {
  static_assert(std::is_integral_v<T>, "`T` must be an integral type.");
  static_assert(std::is_integral_v<U>, "`U` must be an integral type.");
  static_assert(std::is_integral_v<R>, "`R` must be an integral type.");

  R result = 0;
  if (add_overflow(x, y, &result)) {
    trap();
  }
  return result;
}

/// ### `trapping_mul`
///
/// Multiplies `x` and `y` and returns the result. If the operation
/// overflows, or cannot fit into type `R`, this function will `trap`.
template <typename T, typename U, typename R>
R trapping_mul(T x, U y) {
  static_assert(std::is_integral_v<T>, "`T` must be an integral type.");
  static_assert(std::is_integral_v<U>, "`U` must be an integral type.");
  static_assert(std::is_integral_v<R>, "`R` must be an integral type.");

  R result = 0;
  if (mul_overflow(x, y, &result)) {
    trap();
  }

  return result;
}

/// ### `trapping_sub`
///
/// Subtracts `y` from `x` and returns the result. If the operation
/// overflows, or cannot fit into type `R`, this function will `trap`.
template <typename T, typename U, typename R>
R trapping_sub(T x, U y) {
  static_assert(std::is_integral_v<T>, "`T` must be an integral type.");
  static_assert(std::is_integral_v<U>, "`U` must be an integral type.");
  static_assert(std::is_integral_v<R>, "`R` must be an integral type.");

  R result = 0;
  if (sub_overflow(x, y, &result)) {
    trap();
  }

  return result;
}

/// ### `trapping_div`
///
/// Divides `dividend` by `divisor` and returns the quotient. If the operation
/// overflows, or cannot fit into type `R`, this function will `trap`.
template <typename T, typename U, typename R>
R trapping_div(T dividend, U divisor) {
  static_assert(std::is_integral_v<T>, "`T` must be an integral type.");
  static_assert(std::is_integral_v<U>, "`U` must be an integral type.");
  static_assert(std::is_integral_v<R>, "`R` must be an integral type.");

  R result = 0;
  if (div_overflow(dividend, divisor, &result)) {
    trap();
  }

  return result;
}

/// ### `trapping_mod`
///
/// Divides `dividend` by `divisor` and returns the remainder. If the operation
/// overflows, or cannot fit into type `R`, this function will `trap`.
template <typename T, typename U, typename R>
R trapping_mod(T dividend, U divisor) {
  static_assert(std::is_integral_v<T>, "`T` must be an integral type.");
  static_assert(std::is_integral_v<U>, "`U` must be an integral type.");
  static_assert(std::is_integral_v<R>, "`R` must be an integral type.");

  R result = 0;
  if (mod_overflow(dividend, divisor, &result)) {
    trap();
  }

  return result;
}

/// ## `trapping<T>`
///
/// This template class implements integer types with well-defined behavior on
/// overflow, underflow, bit-shifting too far, division by 0, and narrowing
/// conversions. For each of those phenomena, this implementation will trap.
///
/// For guaranteed wrapping behavior, see the companion template class
/// `wrapping<T>`.
///
/// Implementation guided by the fine advice at
/// https://en.cppreference.com/w/cpp/language/operators.
template <typename T>
class trapping {
  static_assert(std::is_integral_v<T>, "`T` must be an integral type.");

  using Self = trapping<T>;

 public:
  // We comment this version out, because it breaks `std::is_trivial`, which we
  // want for good code generation.
  //
  // ### `trapping`
  //
  // Constructs and initializes `value_` to 0.
  // trapping() : value_(0) {}

  /// ### `trapping`
  ///
  /// The default constructor. The contents of the object are undefined. 😕
  /// Best practice is to use `-ftrivial-auto-var-init=zero` or to
  /// explicitly initialize the object.
  trapping() = default;

  /// ### `trapping`
  ///
  /// Constructs and initializes `value_` to `value`.
  trapping(T value) : value_(value) {}

  // If we un-comment these `operator=`s, `std::is_trivial` will no longer be
  // true. We really want for it to be true, because clang++ generates
  // significantly better code when it is true. However, even with them
  // commented out, we can still do
  //
  //   trapping<int> x{42};
  //   trapping<int> y = x;
  //
  // because the compiler generates the code necessary — and that does not break
  // `is_trivial`. So, it would seem the best thing to do is to let the compiler
  // do that, rather than manuall define these.

  // ### `operator=`
  //
  // Copy assignment.
  // Self& operator=(Self other) {
  //   if (this == &other) {
  //     return *this;
  //   }
  //   value_ = other.value_;
  //   return *this;
  // }

  // ### `operator=`
  //
  // Move assignment. Does not modify `other`.
  // Self& operator=(Self&& other) noexcept {
  //   if (this == &other) {
  //     return *this;
  //   }
  //   value_ = other.value_;
  //   return *this;
  // }

  // ### `operator=`
  //
  // Copy assignment (copy-and-swap idiom). `other` will be destructed.
  // Self& operator=(Self other) noexcept {
  //   std::swap(value_, other.value_);
  //   return *this;
  // }

  // TODO: This might not be the right way to go about it and/or possible, but
  // we need some way to ensure that assigning a `U` to `this` incurs a
  // `trapping_cast` check. By whatever means is appropriate.
  // Self& operator=(U other) noexcept { }

  /// ### `operator+=`
  ///
  /// Increments by `x`, `trap`ping on overflow.
  Self& operator+=(T x) {
    value_ = trapping_add<T, T, T>(value_, x);
    return *this;
  }

  /// ### `operator+`
  ///
  /// Adds `rhs` to `lhs`, assigns the result to `lhs`, and returns it.
  /// `trap`s on overflow.
  friend Self operator+(Self lhs, Self rhs) {
    lhs += rhs;
    return lhs;
  }

  /// ### `operator+`
  ///
  /// Does nothing. (But it’s explicit about it!)
  Self& operator+() { return *this; }

  /// ### `operator-=`
  ///
  /// Subtracts `x`, `trap`ping on overflow.
  Self& operator-=(T x) {
    value_ = trapping_sub<T, T, T>(value_, x);
    return *this;
  }

  /// ### `operator-`
  ///
  /// Subtracts `rhs` from `lhs`, assigns the result to `lhs`, and returns
  /// it. `trap`s on overflow.
  friend Self operator-(Self lhs, Self rhs) {
    lhs -= rhs;
    return lhs;
  }

  /// ### `operator-`
  ///
  /// Reverses the value’s sign. (For unsigned `T`s, flips the bits just as
  /// if `T` were signed.) However, if `T` is signed and is the minimum
  /// value, which cannot be represented in the positive range of `T`, this
  /// function will `trap`.
  Self& operator-() {
    if (std::is_signed_v<T> && value_ == std::numeric_limits<T>::min()) {
      trap();
    }
    value_ = -value_;
    return *this;
  }

  /// ### `operator*=`
  ///
  /// Multiplies by `x`, `trap`ping on overflow.
  Self& operator*=(T x) {
    value_ = trapping_mul<T, T, T>(value_, x);
    return *this;
  }

  /// ### `operator*`
  ///
  /// Multiplies `lhs` by `rhs`, assigns the result to `lhs`, and returns
  /// it. `trap`s on overflow.
  friend Self operator*(Self lhs, Self rhs) {
    lhs *= rhs;
    return lhs;
  }

  /// ### `operator/=`
  ///
  /// Divides by `divisor`, storing the quotient in `*this`, and `trap`ping
  /// on overflow or if `divisor` is 0.
  Self& operator/=(T divisor) {
    value_ = trapping_div<T, T, T>(value_, divisor);
    return *this;
  }

  /// ### `operator/`
  ///
  /// Divides `dividend` by `divisor`, storing the quotient in `dividend`, and
  /// returns `dividend`. `trap`s on overflow or if `divisor` is 0.
  friend Self operator/(Self dividend, Self divisor) {
    dividend /= divisor;
    return dividend;
  }

  /// ### `operator%=`
  ///
  /// Divides by `divisor`, storing the remainder in `*this`, and `trap`ping on
  /// overflow or if `divisor` is 0.
  Self& operator%=(T divisor) {
    value_ = trapping_mod<T, T, T>(value_, divisor);
    return *this;
  }

  /// ### `operator%`
  ///
  /// Divides `dividend` by `divisor`, storing the remainder in `dividend`, and
  /// returns `dividend`. `trap`s on overflow or if `divisor` is 0.
  friend Self operator%(Self dividend, Self divisor) {
    dividend %= divisor;
    return dividend;
  }

  /// ### `operator|=`
  ///
  /// Takes the bitwise `|` of the value and `x`, and assigns it to `value_`.
  /// Returns `*this`.
  Self& operator|=(T x) {
    value_ |= x.value_;
    return *this;
  }

  /// ### `operator|`
  ///
  /// Takes the bitwise `|` of `lhs` and `rhs`, assigns it to `lhs`, and returns
  /// it.
  friend Self operator|(Self lhs, Self rhs) {
    lhs |= rhs;
    return lhs;
  }

  /// ### `operator&=`
  ///
  /// Takes the bitwise `&` of the value and `x`, and assigns it to `value_`.
  /// Returns `*this`.
  Self& operator&=(T x) {
    value_ &= x.value_;
    return *this;
  }

  /// ### `operator&`
  ///
  /// Takes the bitwise `&` of `lhs` and `rhs`, assigns it to `lhs`, and returns
  /// it.
  friend Self operator&(Self lhs, Self rhs) {
    lhs &= rhs;
    return lhs;
  }

  /// ### `operator^=`
  ///
  /// Takes the bitwise `^` of the value and `x`, and assigns it to `value_`.
  /// Returns `*this`.
  Self& operator^=(T x) {
    value_ ^= x.value_;
    return *this;
  }

  /// ### `operator^`
  ///
  /// Takes the bitwise `&` of `lhs` and `rhs`, assigns it to `lhs`, and returns
  /// it.
  friend Self operator^(Self lhs, Self rhs) {
    lhs |= rhs;
    return lhs;
  }

  /// ### `operator>>=`
  ///
  /// Shifts the value right by `x` bits, and assigns the result to `value_`.
  /// Returns `*this`. `trap`s if `x` is more than there are bits in the value.
  Self& operator>>=(T x) {
    if (x < 1 || x > (CHAR_BIT * sizeof(T) - 1)) {
      trap();
    }
    value_ = value_ >> x;
    return *this;
  }

  /// ### `operator>>`
  ///
  /// Shifts `lhs` right by `x` bits, assigns the result to `lhs`, and returns
  /// it.. `trap`s if `x` is more than there are bits in the value.
  friend Self operator>>(Self lhs, Self rhs) {
    lhs >>= rhs;
    return lhs;
  }

  /// ### `operator<<=`
  ///
  /// Shifts the value left by `x` bits, and assigns the result to `value_`.
  /// Returns `*this`. `trap`s if `x` is more than there are bits in the value
  /// or if bits ‘fall off’ the left side (i.e. the shift overflows).
  Self& operator<<=(T x) {
    if (x < 1 || static_cast<size_t>(x) > (CHAR_BIT * sizeof(T) - 1U)) {
      trap();
    }

    // Check that we aren’t about to shift left by more than we have room left
    // for — i.e. check for overflow.
    using U = typename std::make_unsigned<T>::type;
    U unsigned_x = static_cast<U>(x);
    const int leading_zeros = __builtin_clz(unsigned_x);
    if (x >= leading_zeros) {
      trap();
    }

    const T y = value_ << x;

    // E.g. (1 << 31UL) results in "warning: signed shift result (0x80000000)
    // sets the sign bit of the shift expression's type ('int') and becomes
    // negative [-Wshift-sign-overflow]". So, check for this condition: change
    // in sign if `T` `is_signed`.
    if (std::is_signed_v<T> && ((x < 0 && y >= 0) || (x >= 0 && y < 0))) {
      trap();
    }

    value_ = y;
    return *this;
  }

  /// ### `operator<<`
  ///
  /// Shifts `lhs` left by `x` bits, and assigns the result to `lhs`, and
  /// returns it.. `trap`s if `x` is more than there are bits in the value or if
  /// bits ‘fall off’ the left side (i.e. the shift overflows).
  friend Self operator<<(Self lhs, Self rhs) {
    lhs <<= rhs;
    return lhs;
  }

  /// ### `operator<`
  ///
  /// Returns true if `lhs` is less than `rhs`.
  friend bool operator<(Self lhs, Self rhs) { return lhs.value_ < rhs.value_; }

  /// ### `operator<`
  ///
  /// Returns true if `lhs` is less than `rhs`.
  friend bool operator<(Self lhs, T rhs) { return lhs.value_ < rhs; }

  /// ### `operator>`
  ///
  /// Returns true if `lhs` is greater than `rhs`.
  friend bool operator>(Self lhs, Self rhs) { return rhs < lhs; }

  /// ### `operator>`
  ///
  /// Returns true if `lhs` is greater than `rhs`.
  friend bool operator>(Self lhs, T rhs) { return rhs < lhs; }

  /// ### `operator<=`
  ///
  /// Returns true if `lhs` is less than or equal to `rhs`.
  friend bool operator<=(Self lhs, Self rhs) { return !(lhs > rhs); }

  /// ### `operator<=`
  ///
  /// Returns true if `lhs` is less than or equal to `rhs`.
  friend bool operator<=(Self lhs, T rhs) { return !(lhs > rhs); }

  /// ### `operator>=`
  ///
  /// Returns true if `lhs` is greater than or equal to `rhs`.
  friend bool operator>=(Self lhs, Self rhs) { return !(rhs > lhs); }

  /// ### `operator>=`
  ///
  /// Returns true if `lhs` is greater than or equal to `rhs`.
  friend bool operator>=(Self lhs, T rhs) { return !(rhs > lhs); }

  /// ### `operator==`
  ///
  /// Returns true if `lhs` is equal to `rhs`.
  friend bool operator==(Self lhs, Self rhs) {
    return lhs.value_ == rhs.value_;
  }

  /// ### `operator==`
  ///
  /// Returns true if `lhs` is equal to `rhs`.
  friend bool operator==(Self lhs, T rhs) { return lhs.value_ == rhs; }

  /// ### `operator!=`
  ///
  /// Returns true if `lhs` is not equal to `rhs`.
  friend bool operator!=(Self lhs, Self rhs) { return !(lhs == rhs); }

  /// ### `operator!=`
  ///
  /// Returns true if `lhs` is not equal to `rhs`.
  friend bool operator!=(Self lhs, T rhs) { return !(lhs == rhs); }

  /// ### `operator++`
  ///
  /// Prefix increment. Increments the value and returns `*this` with the new
  /// value.
  Self& operator++() {
    *this += 1;
    return *this;
  }

  /// ### `operator++`
  ///
  /// Postfix increment. Increments the value and returns an object containing
  /// the previous value.
  Self operator++(int) {
    Self previous = *this;
    *this += 1;
    return previous;
  }

  /// ### `operator--`
  ///
  /// Prefix decrement. Decrements the value and returns `*this` with the new
  /// value.
  Self& operator--() {
    *this -= 1;
    return *this;
  }

  /// ### `operator--`
  ///
  /// Postfix decrement. Decrements the value and returns an object containing
  /// the previous value.
  Self operator--(int) {
    Self previous = *this;
    *this -= 1;
    return previous;
  }

  /// ### `operator T`
  ///
  /// Returns the plain `T` value of `*this`.
  operator T() const { return value_; }

  // ### `operator U`
  //
  // Using `trapping_cast<T, U>`, casts `T` to `U`.
  //
  // TODO.
  //
  // Ideally, we'd be able to require that all conversions always go through
  // `trapping_cast<T, U>` — no implicit conversions. But, this seems to never
  // be called.
  //
  // TODO NOTE: With C++20, we can implement `operator U` as:
  //
  //   template <typename U>
  //   require (!std::same_as<T, U>)
  //   operator U() const { ... }
  template <typename U, typename = std::enable_if<!std::is_same_v<T, U>>>
  operator U() const {
    return trapping_cast<T, U>(value_);
  }

 private:
  T value_;
};

static_assert(std::is_trivial_v<trapping<int>>,
              "`trapping<T>` must be trivial");
static_assert(sizeof(trapping<int8_t>) == sizeof(int8_t),
              "sizeof(trapping<int8_t>) must == sizeof(int8_t)");
static_assert(sizeof(trapping<int16_t>) == sizeof(int16_t),
              "sizeof(trapping<int16_t>) must == sizeof(int16_t)");
static_assert(sizeof(trapping<int32_t>) == sizeof(int32_t),
              "sizeof(trapping<int32_t>) must == sizeof(int32_t)");
static_assert(sizeof(trapping<int64_t>) == sizeof(int64_t),
              "sizeof(trapping<int64_t>) must == sizeof(int64_t)");

}  // namespace integers

#endif  // TRAPPING_H_
