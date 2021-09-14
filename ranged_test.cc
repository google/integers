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

#include "expectations.h"
#include "ranged.h"

using namespace std;
using namespace integers;

int main() {
  {
    ranged<int, 0, 256> goat{42};
    EXPECT(42 == goat);
    cout << goat << "\n";
  }
  {
    ranged<int, 0, 256> goat;
    //EXPECT_DEATH(goat{512});
  }
}
