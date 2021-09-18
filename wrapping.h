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

#ifndef WRAPPING_H_
#define WRAPPING_H_

#include <stdint.h>
#include <stdlib.h>

#include <algorithm>
#include <limits>
#include <type_traits>

namespace integers {

/// ## Wrapping Operations
///
/// ### `wrapping_add`
///
/// Adds `x` and `y` and returns the result. If the operation overflows, or
/// cannot fit into type `R`, this function will wrap.
template <typename T, typename U, typename R>
R wrapping_add(const T& x, const U& y) {
  // TODO
  (void)x;
  (void)y;
  return 0;
}

/// ### `wrapping_mul`
///
/// Multiplies `x` and `y` and returns the result. If the operation overflows,
/// or cannot fit into type `R`, this function will wrap.
template <typename T, typename U, typename R>
R wrapping_mul(const T& x, const U& y) {
  // TODO
  (void)x;
  (void)y;
  return 0;
}

/// ### `wrapping_sub`
///
/// Subtracts `y` from `x` and returns the result. If the operation overflows,
/// or cannot fit into type `R`, this function will wrap.
template <typename T, typename U, typename R>
R wrapping_sub(const T& x, const U& y) {
  // TODO
  (void)x;
  (void)y;
  return 0;
}

/// ### `wrapping_div`
///
/// Divides `dividend` by `divisor` and returns the quotient. If the operation
/// overflows, or cannot fit into type `R`, this function will wrap.
template <typename T, typename U, typename R>
R wrapping_div(const T& dividend, const U& divisor) {
  // TODO
  (void)dividend;
  (void)divisor;
  return 0;
}

/// ### `wrapping_mod`
///
/// Divides `dividend` by `divisor` and returns the remainder. If the operation
/// overflows, or cannot fit into type `R`, this function will wrap.
template <typename T, typename U, typename R>
R wrapping_mod(const T& dividend, const U& divisor) {
  // TODO
  (void)dividend;
  (void)divisor;
  return 0;
}

/// ## `wrapping<T>`
///
/// This template class implements integer types with well-defined behavior on
/// overflow, underflow, bit-shifting too far, and narrowing conversions. For
/// each of those phenomena, this implementation will wrap.
///
/// This implementation works by casting `T` to the `unsigned` equivalent (using
/// `make_unsigned`), which the C++ standard defines to not overflow (see
/// section 6.8.1 of https://isocpp.org/files/papers/N4860.pdf). Combined with
/// the guarantee that integers are represented using 2’s complement (6.8.1
/// again), that suggests wrapping on overflow. Our tests assert this.
///
/// For guaranteed trapping behavior, see the companion template class
/// `trapping<T>`.
///
/// Implementation guided by the fine advice at
/// https://en.cppreference.com/w/cpp/language/operators.
template <typename T>
class wrapping {
  static_assert(std::is_integral_v<T>, "`T` must be an integral type.");

  using Self = wrapping<T>;

 public:
 private:
  T value_;
};

static_assert(sizeof(wrapping<int8_t>) == sizeof(int8_t),
              "sizeof(wrapping<int8_t>) must == sizeof(int8_t)");
static_assert(sizeof(wrapping<int16_t>) == sizeof(int16_t),
              "sizeof(wrapping<int16_t>) must == sizeof(int16_t)");
static_assert(sizeof(wrapping<int32_t>) == sizeof(int32_t),
              "sizeof(wrapping<int32_t>) must == sizeof(int32_t)");
static_assert(sizeof(wrapping<int64_t>) == sizeof(int64_t),
              "sizeof(wrapping<int64_t>) must == sizeof(int64_t)");

}  // namespace integers

#endif  // WRAPPING_H_
