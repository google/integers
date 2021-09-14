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

#ifndef RANGED_H_
#define RANGED_H_

#include <stdint.h>
#include <stdlib.h>

#include <algorithm>
#include <limits>
#include <type_traits>

#include "expectations.h"

namespace integers {

template <typename T, T Min, T Max>
void assert_in_range(const T& value) {
  if (value < Min || value > Max) {
    trap();
  }
}

/// ## `ranged<T>`
///
/// TODO: Documentation.
template <typename T, T Min, T Max>
class ranged {
 public:
  ranged() : value_(0) { assert_in_range<T, Min, Max>(value_); }

  ranged(const T& value) : value_(value) {
    assert_in_range<T, Min, Max>(value_);
  }

  operator T() const { return value_; }

 private:
  T value_;
};

}  // namespace integers

#endif  // RANGED_H_
