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

#include <iostream>
#include <limits>

#include "expectations.h"
#include "wrapping.h"

using namespace integers;
using namespace std;

// TODO: Remove this from *_test.cc and put it in expectations.h?
#if defined(NDEBUG)
#error Tests must be built with `NDEBUG` undefined.
#endif

namespace {

// TODO: Remove this from *_test.cc and put it in expectations.{cc,h}?
bool failure = false;

}  // namespace

int main() {
  if (failure) {
    cerr << "Welp, some `EXPECT_DEATH` tests failed. Dang!\n";
  }
}
