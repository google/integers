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

#ifndef IN_RANGE_H_
#define IN_RANGE_H_

#include <limits>
#include <type_traits>

namespace std {

//#if __cplusplus < 202002L
// Polyfill of C++20 `std::in_range` to C++17.
template <typename R, typename T>
constexpr bool in_range(T value) noexcept {
  static_assert(std::is_integral_v<T>, "`T` must be an integral type.");
  static_assert(std::is_integral_v<R>, "`R` must be an integral type.");

  constexpr R kMin = std::numeric_limits<R>::min();
  constexpr R kMax = std::numeric_limits<R>::max();

  if constexpr (std::is_signed_v<T> == std::is_signed_v<R>) {
    return kMin <= value && value <= kMax;
  } else if constexpr (std::is_signed_v<T>) {
    return 0 <= value && std::make_unsigned_t<T>(value) <= kMax;
  } else {
    return value <= std::make_unsigned_t<R>(kMax);
  }
}
//#endif

}  // namespace std

#endif  // IN_RANGE_H_
