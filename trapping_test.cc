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

#include "test_support.h"
#include "trapping.h"

using namespace integers;
using namespace std;

namespace {

using i8 = int8_t;
using u8 = uint8_t;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;

constexpr i8 i8_max = numeric_limits<i8>::max();
constexpr u8 u8_max = numeric_limits<u8>::max();
constexpr i16 i16_max = numeric_limits<i16>::max();
constexpr u16 u16_max = numeric_limits<u16>::max();
constexpr i32 i32_max = numeric_limits<i32>::max();
constexpr u32 u32_max = numeric_limits<u32>::max();
constexpr i64 i64_max = numeric_limits<i64>::max();
constexpr u64 u64_max = numeric_limits<u64>::max();

constexpr i8 i8_min = numeric_limits<i8>::min();
constexpr u8 u8_min = numeric_limits<u8>::min();
constexpr i16 i16_min = numeric_limits<i16>::min();
constexpr u16 u16_min = numeric_limits<u16>::min();
constexpr i32 i32_min = numeric_limits<i32>::min();
constexpr u32 u32_min = numeric_limits<u32>::min();
constexpr i64 i64_min = numeric_limits<i64>::min();
constexpr u64 u64_min = numeric_limits<u64>::min();

// This construction comes to us from Alidair Meredith
// (https://twitter.com/AlisdairMered/status/1433994491204153345). We use it to
// reduce repetitive code and to increase the total coverage of type template
// permutations. See e.g. `MinSub1`.
//
//   template <typename T>
//   void Foo() {
//     cout << std::hex << numeric_limits<T>::max() << "\n";
//   }
//
//   template <class ...T>
//   void CallFoo() {
//      (Foo<T>(), ...);
//   }
//
// This template invocation generates a call to `Foo` for each `T` given in the
// list, and is equivalent to writing out all those `Foo<T>()` invocations by
// hand.
//
//   CallFoo<i8, i16, i32, i64>();

// Other tests are written under these convenient assumptions.
void TestBasicAssumptions() {
  EXPECT(i8_max == 0x7F);
  EXPECT(u8_max == 0xFF);
  EXPECT(i16_max == 0x7FFF);
  EXPECT(u16_max == 0xFFFF);
  EXPECT(i32_max == 0x7FFFFFFF);
  EXPECT(u32_max == 0xFFFFFFFF);
  EXPECT(i64_max == 0x7FFFFFFFFFFFFFFF);
  EXPECT(u64_max == 0xFFFFFFFFFFFFFFFF);

  EXPECT(i8_min == static_cast<i8>(0x80));
  EXPECT(u8_min == 0x0);
  EXPECT(i16_min == static_cast<i16>(0x8000));
  EXPECT(u16_min == 0x0);
  EXPECT(i32_min == static_cast<i32>(0x80000000));
  EXPECT(u32_min == 0x0);
  EXPECT(i64_min == static_cast<i64>(0x8000000000000000));
  EXPECT(u64_min == 0x0);
}

void TestAdd() {
  EXPECT_DEATH((trapping_add<i32, i32, i32>(i32_max, 1)));
  EXPECT_DEATH((trapping_add<i32, i32, i16>(i32_max, 0)));
  EXPECT_DEATH((trapping_add<u32, u32, u32>(u32_max, 1)));
  EXPECT_DEATH((trapping_add<u32, u32, u16>(u32_max, 0)));
  EXPECT_DEATH((trapping_add<i16, i16, i16>(i16_max, 1)));
  EXPECT_DEATH((trapping_add<i16, i16, i8>(i16_max, 0)));
  EXPECT_DEATH((trapping_add<u16, u16, u16>(u16_max, 1)));
  EXPECT_DEATH((trapping_add<u16, u16, u8>(u16_max, 0)));

  const i64 expected = trapping_cast<u32, i64>(u32_max) + 1;
  EXPECT(expected == (trapping_add<u32, u32, i64>(u32_max, 1)));
  EXPECT(expected == (trapping_add<u32, u16, i64>(u32_max, 1)));
}

void TestCastTruncate() {
  // Expect narrowing casts out of range of `R` to truncate:
  {
    i8 result;
    EXPECT((cast_truncate<i16, i8>(i16_max, &result)));
    EXPECT((cast_truncate<i32, i8>(i16_max, &result)));
    EXPECT((cast_truncate<i64, i8>(i16_max, &result)));
  }
  {
    u8 result;
    EXPECT((cast_truncate<i16, u8>(i16_max, &result)));
    EXPECT((cast_truncate<i32, u8>(i16_max, &result)));
    EXPECT((cast_truncate<i64, u8>(i16_max, &result)));
  }

  // Expect narrowing casts in range of `R` not to truncate:
  {
    i8 result;
    EXPECT(!(cast_truncate<i16, i8>(i8_max, &result)));
    EXPECT(!(cast_truncate<i32, i8>(i8_max, &result)));
    EXPECT(!(cast_truncate<i64, i8>(i8_max, &result)));
  }
  {
    u8 result;
    EXPECT(!(cast_truncate<i16, u8>(i8_max, &result)));
    EXPECT(!(cast_truncate<i32, u8>(i8_max, &result)));
    EXPECT(!(cast_truncate<i64, u8>(i8_max, &result)));
  }

  // Expect negative values to truncate when cast to unsigned:
  {
    u32 result;
    EXPECT((cast_truncate<i16, u32>(-1, &result)));
    EXPECT((cast_truncate<i32, u32>(-1, &result)));
    EXPECT((cast_truncate<i64, u32>(-1, &result)));
  }

  // Expect positive, in-range values to truncate when cast to unsigned:
  {
    u32 result;
    EXPECT(!(cast_truncate<i16, u32>(i16_max, &result)));
    EXPECT(!(cast_truncate<i32, u32>(i16_max, &result)));
    EXPECT(!(cast_truncate<i64, u32>(i16_max, &result)));
  }
}

void TestAddOverflow() {
  {
    i32 result;
    EXPECT((add_overflow<i32, i32, i32>(i32_max, 1, &result)));
  }
  {
    i16 result;
    EXPECT((add_overflow<i32, i32, i16>(i32_max, 0, &result)));
    EXPECT((add_overflow<i16, i16, i16>(i16_max, 1, &result)));
  }
  {
    u32 result;
    EXPECT((add_overflow<u32, u32, u32>(u32_max, 1, &result)));
  }
  {
    u16 result;
    EXPECT((add_overflow<u32, u32, u16>(u32_max, 0, &result)));
    EXPECT((add_overflow<u16, u16, u16>(u16_max, 1, &result)));
  }
  {
    i8 result;
    EXPECT((add_overflow<i16, i16, i8>(i16_max, 0, &result)));
  }
  {
    u8 result;
    EXPECT((add_overflow<u16, u16, u8>(u16_max, 0, &result)));
    EXPECT(!(add_overflow<u16, u16, u8>(u16_min, 0, &result)));
  }

  {
    const i64 expected = trapping_cast<u32, i64>(u32_max) + 1;
    i64 result;
    EXPECT(!(add_overflow<u32, u32, i64>(u32_max, 1, &result)));
    EXPECT(expected == result);
    EXPECT(!(add_overflow<u32, u16, i64>(u32_max, 1, &result)));
    EXPECT(expected == result);
  }
}

void TestSubOverflow() {
  {
    i32 result;
    EXPECT((sub_overflow<i32, i32, i32>(i32_min, 1, &result)));
  }
  {
    i16 result;
    EXPECT((sub_overflow<i32, i32, i16>(i32_min, 0, &result)));
    EXPECT((sub_overflow<i16, i16, i16>(i16_min, 1, &result)));
  }
  {
    u32 result;
    EXPECT((sub_overflow<u32, u32, u32>(u32_min, 1, &result)));
  }
  {
    u16 result;
    EXPECT(!(sub_overflow<u32, u32, u16>(u32_min, 0, &result)));
    EXPECT((sub_overflow<u16, u16, u16>(u16_min, 1, &result)));
  }
  {
    i8 result;
    EXPECT((sub_overflow<i16, i16, i8>(i16_min, 0, &result)));
  }
  {
    u8 result;
    EXPECT(!(sub_overflow<u16, u16, u8>(u16_min, 0, &result)));
  }

  {
    const i64 expected = trapping_cast<u32, i64>(u32_min) - 1;
    i64 result;
    EXPECT(!(sub_overflow<u32, u32, i64>(u32_min, 1, &result)));
    EXPECT(expected == result);
    EXPECT(!(sub_overflow<u32, u16, i64>(u32_min, 1, &result)));
    EXPECT(expected == result);
  }
}

void TestMulOverflow() {
  // TODO: Should generate this form of test for all types, using the Meredith
  // construction.
  {
    i32 result;
    EXPECT(!(mul_overflow<i32, i32, i32>(i32_max, 0, &result)));
    EXPECT(!(mul_overflow<i32, i32, i32>(i32_max, 1, &result)));
    EXPECT((mul_overflow<i32, i32, i32>(i32_min, 2, &result)));
  }
  {
    i16 result;
    EXPECT(!(mul_overflow<i32, i32, i16>(i32_max, 0, &result)));
    EXPECT(!(mul_overflow<i16, i16, i16>(i16_max, 1, &result)));
    EXPECT((mul_overflow<i16, i16, i16>(i16_max, 2, &result)));
  }
  {
    u32 result;
    EXPECT(!(mul_overflow<u32, u32, u32>(u32_max, 0, &result)));
    EXPECT(!(mul_overflow<u32, u32, u32>(u32_max, 1, &result)));
    EXPECT((mul_overflow<u32, u32, u32>(u32_max, 2, &result)));
  }
  {
    u16 result;
    EXPECT(!(mul_overflow<u32, u32, u16>(u32_max, 0, &result)));
    EXPECT(!(mul_overflow<u16, u16, u16>(u16_max, 1, &result)));
    EXPECT((mul_overflow<u16, u16, u16>(u16_max, 2, &result)));
  }
  {
    i8 result;
    EXPECT(!(mul_overflow<i16, i16, i8>(i16_max, 0, &result)));
    EXPECT((mul_overflow<i16, i16, i8>(i16_max, 1, &result)));
    EXPECT((mul_overflow<i16, i16, i8>(i16_max, 2, &result)));
  }
  {
    u8 result;
    EXPECT(!(mul_overflow<u16, u16, u8>(u16_max, 0, &result)));
    EXPECT(!(mul_overflow<u16, u16, u8>(u16_min, 0, &result)));
  }

  {
    const i64 expected = trapping_cast<u32, i64>(u32_max) * 1;
    i64 result;
    EXPECT(!(mul_overflow<u32, u32, i64>(u32_max, 1, &result)));
    EXPECT(expected == result);
    EXPECT(!(mul_overflow<u32, u16, i64>(u32_max, 1, &result)));
    EXPECT(expected == result);
  }
}

void TestDivOverflow() {
  // TODO: Should generate this form of test for all types, using the Meredith
  // construction.
  {
    i32 result;
    EXPECT((div_overflow<i32, i32, i32>(i32_max, 0, &result)));
    EXPECT(!(div_overflow<i32, i32, i32>(i32_max, 1, &result)));
    EXPECT(!(div_overflow<i32, i32, i32>(i32_min, 2, &result)));
    EXPECT((div_overflow<i32, i32, i32>(i32_min, -1, &result)));
  }
  {
    i16 result;
    EXPECT((div_overflow<i32, i32, i16>(i32_max, 0, &result)));
    EXPECT(!(div_overflow<i16, i16, i16>(i16_max, 1, &result)));
    EXPECT(!(div_overflow<i16, i16, i16>(i16_max, 2, &result)));
    EXPECT(!(div_overflow<i16, i16, i16>(i16_max, -1, &result)));
  }
  {
    u32 result;
    EXPECT((div_overflow<u32, u32, u32>(u32_max, 0, &result)));
    EXPECT(!(div_overflow<u32, u32, u32>(u32_max, 1, &result)));
    EXPECT(!(div_overflow<u32, u32, u32>(u32_max, 2, &result)));
    EXPECT(!(div_overflow<u32, u32, u32>(u32_max, u32_max, &result)));
  }
  {
    u16 result;
    EXPECT((div_overflow<u32, u32, u16>(u32_max, 0, &result)));
    EXPECT(!(div_overflow<u16, u16, u16>(u16_max, 1, &result)));
    EXPECT(!(div_overflow<u16, u16, u16>(u16_max, 2, &result)));
    EXPECT(!(div_overflow<u16, u16, u16>(u16_max, u16_max, &result)));
  }
  {
    i8 result;
    EXPECT((div_overflow<i16, i16, i8>(i16_max, 0, &result)));
    EXPECT((div_overflow<i16, i16, i8>(i16_max, 1, &result)));
    EXPECT((div_overflow<i16, i16, i8>(i16_max, 2, &result)));
    EXPECT(!(div_overflow<i16, i16, i8>(i16_max, -1, &result)));
  }
  {
    u8 result;
    EXPECT((div_overflow<u16, u16, u8>(u16_max, 0, &result)));
    EXPECT((div_overflow<u16, u16, u8>(u16_min, 0, &result)));
  }

  {
    const i64 expected = trapping_cast<u32, i64>(u32_max) / 1;
    i64 result;
    EXPECT(!(div_overflow<u32, u32, i64>(u32_max, 1, &result)));
    EXPECT(expected == result);
    EXPECT(!(div_overflow<u32, u16, i64>(u32_max, 1, &result)));
    EXPECT(expected == result);
  }
}

void TestModOverflow() {
  // TODO: Should generate this form of test for all types, using the Meredith
  // construction.
  {
    i32 result;
    EXPECT((mod_overflow<i32, i32, i32>(i32_max, 0, &result)));
    EXPECT(!(mod_overflow<i32, i32, i32>(i32_max, 1, &result)));
    EXPECT(!(mod_overflow<i32, i32, i32>(i32_min, 2, &result)));
    EXPECT((mod_overflow<i32, i32, i32>(i32_min, -1, &result)));
  }
  {
    i16 result;
    EXPECT((mod_overflow<i32, i32, i16>(i32_max, 0, &result)));
    EXPECT(!(mod_overflow<i16, i16, i16>(i16_max, 1, &result)));
    EXPECT(!(mod_overflow<i16, i16, i16>(i16_max, 2, &result)));
    EXPECT(!(mod_overflow<i16, i16, i16>(i16_max, -1, &result)));
  }
  {
    u32 result;
    EXPECT((mod_overflow<u32, u32, u32>(u32_max, 0, &result)));
    EXPECT(!(mod_overflow<u32, u32, u32>(u32_max, 1, &result)));
    EXPECT(!(mod_overflow<u32, u32, u32>(u32_max, 2, &result)));
    EXPECT(!(mod_overflow<u32, u32, u32>(u32_max, u32_max, &result)));
  }
  {
    u16 result;
    EXPECT((mod_overflow<u32, u32, u16>(u32_max, 0, &result)));
    EXPECT(!(mod_overflow<u16, u16, u16>(u16_max, 1, &result)));
    EXPECT(!(mod_overflow<u16, u16, u16>(u16_max, 2, &result)));
    EXPECT(!(mod_overflow<u16, u16, u16>(u16_max, u16_max, &result)));
  }
  {
    i8 result;
    EXPECT((mod_overflow<i16, i16, i8>(i16_max, 0, &result)));
    EXPECT(!(mod_overflow<i16, i16, i8>(i16_max, 1, &result)));
    EXPECT(!(mod_overflow<i16, i16, i8>(i16_max, 2, &result)));
    EXPECT(!(mod_overflow<i16, i16, i8>(i16_max, -1, &result)));
  }
  {
    u8 result;
    EXPECT((mod_overflow<u16, u16, u8>(u16_max, 0, &result)));
    EXPECT((mod_overflow<u16, u16, u8>(u16_min, 0, &result)));
  }

  {
    const i64 expected = trapping_cast<u32, i64>(u32_max) % 1;
    i64 result;
    EXPECT(!(mod_overflow<u32, u32, i64>(u32_max, 1, &result)));
    EXPECT(expected == result);
    EXPECT(!(mod_overflow<u32, u16, i64>(u32_max, 1, &result)));
    EXPECT(expected == result);
  }
}

void TestMul() {
  EXPECT_DEATH((trapping_mul<i32, i32, i32>(i32_max, 2)));
  EXPECT_DEATH((trapping_mul<i32, i32, i16>(i32_max, 1)));
  EXPECT_DEATH((trapping_mul<u32, u32, u32>(u32_max, 2)));
  EXPECT_DEATH((trapping_mul<u32, u32, u16>(u32_max, 1)));
  EXPECT_DEATH((trapping_mul<i16, i16, i16>(i16_max, 2)));
  EXPECT_DEATH((trapping_mul<i16, i16, i8>(i16_max, 1)));
  EXPECT_DEATH((trapping_mul<u16, u16, u16>(u16_max, 2)));
  EXPECT_DEATH((trapping_mul<u16, u16, u8>(u16_max, 1)));

  const i64 expected = trapping_cast<u32, i64>(u32_max) * 2;
  EXPECT(expected == (trapping_mul<u32, u32, i64>(u32_max, 2)));
  EXPECT(expected == (trapping_mul<u32, u16, i64>(u32_max, 2)));
}

void TestSub() {
  EXPECT_DEATH((trapping_sub<i32, i32, i32>(i32_min, 1)));
  EXPECT_DEATH((trapping_sub<i32, i32, i16>(i32_min, 0)));
  EXPECT_DEATH((trapping_sub<u32, u32, u32>(u32_min, 1)));
  EXPECT_DEATH((trapping_sub<u32, u32, u16>(u32_min, 1)));
  EXPECT_DEATH((trapping_sub<i16, i16, i16>(i16_min, 1)));
  EXPECT_DEATH((trapping_sub<i16, i16, i8>(i16_min, 0)));
  EXPECT_DEATH((trapping_sub<u16, u16, u16>(u16_min, 1)));
  EXPECT_DEATH((trapping_sub<u16, u16, u8>(u16_min, 1)));

  const i64 expected = trapping_cast<u32, i64>(u32_min) - 1;
  EXPECT(expected == (trapping_sub<u32, u32, i64>(u32_min, 1)));
  EXPECT(expected == (trapping_sub<u32, u16, i64>(u32_min, 1)));
}

void TestDiv() {
  EXPECT_DEATH((trapping_div<i32, i32, i16>(i32_max, 2)));
  EXPECT_DEATH((trapping_div<u32, u32, u16>(u32_max, 2)));
  EXPECT_DEATH((trapping_div<i16, i16, i8>(i16_max, 1)));
  EXPECT_DEATH((trapping_div<u16, u16, u8>(u16_max, 1)));

  {
    const i64 expected = trapping_cast<u32, i64>(u32_max) / 2;
    EXPECT(expected == (trapping_div<u32, u32, i64>(u32_max, 2)));
    EXPECT(expected == (trapping_div<u32, u16, i64>(u32_max, 2)));
  }

  {
    const i32 expected = trapping_cast<u32, i32>(u32_max / 2);
    EXPECT(expected == (trapping_div<u32, u32, i32>(u32_max, 2)));
    EXPECT(expected == (trapping_div<u32, u16, i32>(u32_max, 2)));
  }

  // TODO: These are getting repetitive; generate them for all types with the
  // template trick.
  {
    const i16 expected = trapping_cast<u16, i16>(u16_max / 2);
    EXPECT(expected == (trapping_div<u16, u16, i32>(u16_max, 2)));
  }

  {
    // And so on: ...
    // EXPECT(expected ==
    //       (trapping_div<u32, u32, u32>(u32_max, 2)));
  }
}

void TestMod() {
  // TODO: Generate all possible type combos with the Meredith construct.
  EXPECT(1 == (trapping_mod<i32, i32, i16>(i32_max, 2)));
  EXPECT(1 == (trapping_mod<u32, u32, u16>(u32_max, 2)));
  EXPECT(0 == (trapping_mod<i16, i16, i8>(i16_max, 1)));
  EXPECT(1 == (trapping_mod<u16, u16, u16>(u16_max, 2)));
  EXPECT(0 == (trapping_mod<u16, u16, u8>(u16_max, 1)));

  // TODO: Generate all possible type combos with the Meredith construct.
  {
    const i16 expected = trapping_cast<u16, i16>(u16_max % 2);
    EXPECT(expected == (trapping_mod<u16, u16, i32>(u16_max, 2)));
  }
  // TODO: Other dividends too, that do not result in 0.

  {
    const i64 expected = trapping_cast<u32, i64>(u32_max) % 2;
    EXPECT(expected == (trapping_mod<u32, u32, i64>(u32_max, 2)));
    EXPECT(expected == (trapping_mod<u32, u16, i64>(u32_max, 2)));
  }

  {
    const i32 expected = trapping_cast<u32, i32>(u32_max % 2);
    EXPECT(expected == (trapping_mod<u32, u32, i32>(u32_max, 2)));
    EXPECT(expected == (trapping_mod<u32, u16, i32>(u32_max, 2)));
  }

  {
    // And so on: ...
    // EXPECT(expected ==
    //       (trapping_mod<u32, u32, u32>(u32_max, 2)));
  }
}

void TestConstructorDefault() {
  // TODO: See comments in trapping.h. Someday, we can do this:
  // trapping<int> x;
  // EXPECT(0 == x);
}

void TestConstructorT() {
  // TODO
}

void TestCast() {
  // Thanks to Steve Checkoway for this test case. The cast has already happened
  // by the time the constructor gets to check the value. There may be nothing
  // we can do about this. TODO: Maybe all we can do is document this problem?
  // EXPECT_DEATH(trapping<unsigned int>{(unsigned
  // int)std::numeric_limits<int>::min()});
  // trapping<i16>{(i16)std::numeric_limits<i32>::min()};

  // TODO: `-Wsign-conversion` catches this. But we can't expect that real-world
  // callers will use that warning (let alone use `-Werror`). So, we probably
  // need to build multiple times, once with `-Weverything -Werror` for our own
  // hygeine, and then again with no warnings to emulate real-world callers. And
  // then tests like this should still pass.
  // EXPECT_DEATH(trapping<unsigned int>{std::numeric_limits<int>::min()});

  {
    i32 x = 0x0EADBEEF;
    i32 y = trapping_cast<i32, i32>(x);
    EXPECT(x == y);
    EXPECT(y == 0x0EADBEEF);
  }

  {
    u32 x = 0xDEADBEEF;
    u32 y = trapping_cast<u32, u32>(x);
    EXPECT(x == y);
    EXPECT(y == 0xDEADBEEF);
  }

  {
    u16 x = 0xBEEF;
    i32 y = trapping_cast<u16, i32>(x);
    EXPECT(x == y);
    EXPECT(y == 0xBEEF);
  }

  {
    i32 x = 42;
    i16 y = trapping_cast<i32, i16>(x);
    EXPECT(x == y);
    EXPECT(y == 42);

    x = 0x0EADBEEF;
    EXPECT_DEATH((y = trapping_cast<i32, i16>(x)));
  }

  // TODO: Use Meredith here.
  {
    u64 x = u64_max;
    i64 y = 0;
    EXPECT_DEATH((y = trapping_cast<u64, i64>(x)));
  }

  // TODO: Use Meredith here.
  {
    i32 x = i32_min;
    i64 y = trapping_cast<i32, i64>(x);
    EXPECT(y == i32_min);
    EXPECT(y == x);
  }
}

template <typename T>
void MaxAdd1() {
  trapping<T> x = numeric_limits<T>::max();
  EXPECT_DEATH(x += 1);
}

template <class... T>
void CallMaxAdd1() {
  (MaxAdd1<T>(), ...);
}

template <typename T>
void MinAdd1() {
  trapping<T> x = numeric_limits<T>::min();
  x += 1;
  // TODO: Consider implementing overrides such that the explicit cast/ctor here
  // is not necessary.
  EXPECT(x == T{numeric_limits<T>::min() + 1});
}

template <class... T>
void CallMinAdd1() {
  (MinAdd1<T>(), ...);
}

void TestOperatorAdd() {
  CallMaxAdd1<i8, u8, i16, u16, i32, u32, i64, u64>();
  CallMinAdd1<i8, u8, i16, u16, i32, u32, i64, u64>();
}

template <typename T>
void MinSub1() {
  trapping<T> x = numeric_limits<T>::min();
  EXPECT_DEATH(x -= 1);
}

template <class... T>
void CallMinSub1() {
  (MinSub1<T>(), ...);
}

template <typename T>
void MaxSub1() {
  trapping<T> x = numeric_limits<T>::max();
  x -= 1;
  // TODO: Consider implementing overrides such that the explicit cast/ctor here
  // is not necessary.
  EXPECT(x == T{numeric_limits<T>::max() - 1});
  // TODO: Parameterize this. Or get rid of it.
  // EXPECT(x == 0xFFFFFFFE);
}

template <class... T>
void CallMaxSub1() {
  (MaxSub1<T>(), ...);
}

void TestOperatorSub() {
  CallMaxSub1<i8, u8, i16, u16, i32, u32, i64, u64>();
  CallMinSub1<i8, u8, i16, u16, i32, u32, i64, u64>();

  {
    // TODO BUG: Figure this out. "call to implicitly deleted copy constructor".
    // trapping<u16> x = u16_min;
    // EXPECT_DEATH(x = x - u16{1});
  }
}

void TestOperatorMul() {
  CallMaxAdd1<i8, u8, i16, u16, i32, u32, i64, u64>();
  CallMinAdd1<i8, u8, i16, u16, i32, u32, i64, u64>();
}

void TestOperatorDiv() {
  // TODO
}

void TestOperatorMod() {
  // TODO
}

void TestOperatorOr() {
  // TODO
}

void TestOperatorAnd() {
  // TODO
}

void TestOperatorXor() {
  // TODO
}

void TestOperatorLeftShift() {
  {
    trapping<i32> x = 1;
    x <<= 1;
    EXPECT(x == 2);
  }
  {
    trapping<i32> x = 1;
    EXPECT_DEATH(x <<= 31);
  }
}

void TestOperatorRightShift() {
  // TODO
}

void TestOperatorLessThan() {
  // TODO
}

void TestOperatorGreaterThan() {
  // TODO
}

void TestOperatorLessThanOrEqual() {
  // TODO
}

void TestOperatorGreaterThanOrEqual() {
  // TODO
}

void TestOperatorEqual() {
  // TODO
}

void TestOperatorNotEqual() {
  // TODO
}

void TestOperatorIncrement() {
  // TODO
}

void TestOperatorDecrement() {
  // TODO
}

void TestOperatorT() {
  // TODO
}

void TestOperatorU() {
  // TODO
}

void TestMultiOperatorOverflow() {
  // TODO: Parameterize this for u8, u16, u32.
  {
    trapping<u16> x = i16_max;
    // I.e. we expect headroom in a u16 above the *signed* 16-bit max:
    x *= 2;
    EXPECT(x == static_cast<u16>(0xFFFE));
    x += 1;
    EXPECT(x == static_cast<u16>(0xFFFF));
    EXPECT_DEATH(x += 1);
  }
}

}  // namespace

int main() {
  TestBasicAssumptions();

  TestCastTruncate();

  TestAddOverflow();
  TestSubOverflow();
  TestMulOverflow();
  TestDivOverflow();
  TestModOverflow();

  TestCast();

  TestAdd();
  TestSub();
  TestMul();
  TestDiv();
  TestMod();

  TestConstructorDefault();
  TestConstructorT();

  TestOperatorAdd();
  TestOperatorSub();
  TestOperatorMul();
  TestOperatorDiv();
  TestOperatorMod();

  TestOperatorOr();
  TestOperatorAnd();
  TestOperatorXor();

  TestOperatorRightShift();
  TestOperatorLeftShift();

  TestOperatorLessThan();
  TestOperatorGreaterThan();
  TestOperatorLessThanOrEqual();
  TestOperatorGreaterThanOrEqual();
  TestOperatorEqual();
  TestOperatorNotEqual();

  TestOperatorIncrement();
  TestOperatorDecrement();

  TestOperatorT();
  TestOperatorU();

  TestMultiOperatorOverflow();
}
