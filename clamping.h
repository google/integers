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

#ifndef INTEGERS_H_
#define INTEGERS_H_

#include <stdint.h>
#include <stdlib.h>

#include <algorithm>
#include <limits>
#include <type_traits>

#include "expectations.h"

namespace integers {

/// ## Clamping Operations

// TODO

/// ## `clamping<T>`
///
/// TODO: Documentation.
template <typename T>
class clamping {
  static_assert(std::is_integral<T>::value, "`T` must be an integral type.");

  using Self = clamping<T>;
  using value_type = std::remove_cv_t<T>;
  using pointer = T*;
  using reference = T&;

 public:
 private:
  T value_;
};

static_assert(sizeof(clamping<int8_t>) == sizeof(int8_t),
              "sizeof(clamping<int8_t>) must == sizeof(int8_t)");
static_assert(sizeof(clamping<int16_t>) == sizeof(int16_t),
              "sizeof(clamping<int16_t>) must == sizeof(int16_t)");
static_assert(sizeof(clamping<int32_t>) == sizeof(int32_t),
              "sizeof(clamping<int32_t>) must == sizeof(int32_t)");
static_assert(sizeof(clamping<int64_t>) == sizeof(int64_t),
              "sizeof(clamping<int64_t>) must == sizeof(int64_t)");

}  // namespace integers

#endif  // INTEGERS_H_
