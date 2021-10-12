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
// permutations. See e.g. `GenericTestMulOverflow`.
//
//   template <typename T>
//   void Foo() {
//     cout << std::hex << numeric_limits<T>::max() << "\n";
//   }
//
//   template <class... T>
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
  EXPECT_DEATH((trapping_add<i16, i32, i32>(i32_max, 0)));
  EXPECT_DEATH((trapping_add<u32, u32, u32>(u32_max, 1)));
  EXPECT_DEATH((trapping_add<u16, u32, u32>(u32_max, 0)));
  EXPECT_DEATH((trapping_add<i16, i16, i16>(i16_max, 1)));
  EXPECT_DEATH((trapping_add<i8, i16, i16>(i16_max, 0)));
  EXPECT_DEATH((trapping_add<u16, u16, u16>(u16_max, 1)));
  EXPECT_DEATH((trapping_add<u8, u16, u16>(u16_max, 0)));

  const i64 expected = trapping_cast<i64>(u32_max) + 1;
  EXPECT(expected == (trapping_add<i64, u32, u32>(u32_max, 1)));
  EXPECT(expected == (trapping_add<i64, u32, u16>(u32_max, 1)));
}

void TestCastTruncate() {
  // Expect narrowing casts out of range of `R` to truncate:
  {
    i8 result;
    EXPECT((cast_truncate<i8, i16>(i16_max, &result)));
    EXPECT((cast_truncate<i8, i32>(i16_max, &result)));
    EXPECT((cast_truncate<i8, i64>(i16_max, &result)));
  }
  {
    u8 result;
    EXPECT((cast_truncate<u8, i16>(i16_max, &result)));
    EXPECT((cast_truncate<u8, i32>(i16_max, &result)));
    EXPECT((cast_truncate<u8, i64>(i16_max, &result)));
  }

  // Expect narrowing casts in range of `R` not to truncate:
  {
    i8 result;
    EXPECT(!(cast_truncate<i8, i16>(i8_max, &result)));
    EXPECT(!(cast_truncate<i8, i32>(i8_max, &result)));
    EXPECT(!(cast_truncate<i8, i64>(i8_max, &result)));
  }
  {
    u8 result;
    EXPECT(!(cast_truncate<u8, i16>(i8_max, &result)));
    EXPECT(!(cast_truncate<u8, i32>(i8_max, &result)));
    EXPECT(!(cast_truncate<u8, i64>(i8_max, &result)));
  }

  // Expect negative values to truncate when cast to unsigned:
  {
    u32 result;
    EXPECT((cast_truncate<u32, i16>(-1, &result)));
    EXPECT((cast_truncate<u32, i32>(-1, &result)));
    EXPECT((cast_truncate<u32, i64>(-1, &result)));
  }

  // Expect positive, in-range values to truncate when cast to unsigned:
  {
    u32 result;
    EXPECT(!(cast_truncate<u32, i16>(i16_max, &result)));
    EXPECT(!(cast_truncate<u32, i32>(i16_max, &result)));
    EXPECT(!(cast_truncate<u32, i64>(i16_max, &result)));
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
    const i64 expected = trapping_cast<i64>(u32_max) + 1;
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
    const i64 expected = trapping_cast<i64>(u32_min) - 1;
    i64 result;
    EXPECT(!(sub_overflow<u32, u32, i64>(u32_min, 1, &result)));
    EXPECT(expected == result);
    EXPECT(!(sub_overflow<u32, u16, i64>(u32_min, 1, &result)));
    EXPECT(expected == result);
  }
}

template <typename T>
void GenericTestMulOverflow() {
  T result;
  EXPECT(!(mul_overflow<T, T, T>(numeric_limits<T>::min(), 0, &result)));
  EXPECT(!(mul_overflow<T, T, T>(numeric_limits<T>::max(), 0, &result)));
  EXPECT(!(mul_overflow<T, T, T>(numeric_limits<T>::min(), 1, &result)));
  EXPECT(!(mul_overflow<T, T, T>(numeric_limits<T>::max(), 1, &result)));
  EXPECT(is_signed_v<T> ==
         (mul_overflow<T, T, T>(numeric_limits<T>::min(), 2, &result)));
  EXPECT((mul_overflow<T, T, T>(numeric_limits<T>::max(), 2, &result)));
}

template <class... T>
void CallGenericTestMulOverflow() {
  (GenericTestMulOverflow<T>(), ...);
}

void TestMulOverflow() {
  CallGenericTestMulOverflow<i8, u8, i16, u16, i32, u32, i64, u64>();

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
    const i64 expected = trapping_cast<i64>(u32_max) * 1;
    i64 result;
    EXPECT(!(mul_overflow<u32, u32, i64>(u32_max, 1, &result)));
    EXPECT(expected == result);
    EXPECT(!(mul_overflow<u32, u16, i64>(u32_max, 1, &result)));
    EXPECT(expected == result);
  }
}

template <typename T>
void GenericTestDivOverflow() {
  {
    T result;
    constexpr T max = numeric_limits<T>::max();
    constexpr T min = numeric_limits<T>::min();
    EXPECT((div_overflow<T, T, T>(max, 0, &result)));
    EXPECT(!(div_overflow<T, T, T>(max, 1, &result)));
    EXPECT(!(div_overflow<T, T, T>(min, 2, &result)));
    if constexpr (is_signed_v<T>) {
      EXPECT((div_overflow<T, T, T>(min, -1, &result)));
    }
    EXPECT(!(div_overflow<T, T, T>(max, max, &result)));
  }
}

template <class... T>
void CallGenericTestDivOverflow() {
  (GenericTestDivOverflow<T>(), ...);
}

void TestDivOverflow() {
  CallGenericTestDivOverflow<i8, u8, i16, u16, i32, u32, i64, u64>();

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
    EXPECT((div_overflow<i16, i16, i8>(i16_max, -1, &result)));
  }
  {
    u8 result;
    EXPECT((div_overflow<u16, u16, u8>(u16_max, 0, &result)));
    EXPECT((div_overflow<u16, u16, u8>(u16_min, 0, &result)));
  }

  {
    const i64 expected = trapping_cast<i64>(u32_max) / 1;
    i64 result;
    EXPECT(!(div_overflow<u32, u32, i64>(u32_max, 1, &result)));
    EXPECT(expected == result);
    EXPECT(!(div_overflow<u32, u16, i64>(u32_max, 1, &result)));
    EXPECT(expected == result);
  }
}

template <typename T>
void GenericTestModOverflow() {
  {
    T result;
    constexpr T max = numeric_limits<T>::max();
    constexpr T min = numeric_limits<T>::min();
    EXPECT((mod_overflow<T, T, T>(max, 0, &result)));
    EXPECT(!(mod_overflow<T, T, T>(max, 1, &result)));
    EXPECT(!(mod_overflow<T, T, T>(min, 2, &result)));
    if constexpr (is_signed_v<T>) {
      EXPECT((mod_overflow<T, T, T>(min, -1, &result)));
    }
    EXPECT(!(mod_overflow<T, T, T>(max, max, &result)));
  }
}

template <class... T>
void CallGenericTestModOverflow() {
  (GenericTestModOverflow<T>(), ...);
}

void TestModOverflow() {
  CallGenericTestModOverflow<i8, u8, i16, u16, i32, u32, i64, u64>();

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
    const i64 expected = trapping_cast<i64>(u32_max) % 1;
    i64 result;
    EXPECT(!(mod_overflow<u32, u32, i64>(u32_max, 1, &result)));
    EXPECT(expected == result);
    EXPECT(!(mod_overflow<u32, u16, i64>(u32_max, 1, &result)));
    EXPECT(expected == result);
  }
}

void TestMul() {
  EXPECT_DEATH((trapping_mul<i32, i32, i32>(i32_max, 2)));
  EXPECT_DEATH((trapping_mul<i16, i32, i32>(i32_max, 1)));
  EXPECT_DEATH((trapping_mul<u32, u32, u32>(u32_max, 2)));
  EXPECT_DEATH((trapping_mul<u16, u32, u32>(u32_max, 1)));
  EXPECT_DEATH((trapping_mul<i16, i16, i16>(i16_max, 2)));
  EXPECT_DEATH((trapping_mul<i8, i16, i16>(i16_max, 1)));
  EXPECT_DEATH((trapping_mul<u16, u16, u16>(u16_max, 2)));
  EXPECT_DEATH((trapping_mul<u8, u16, u16>(u16_max, 1)));

  const i64 expected = trapping_cast<i64>(u32_max) * 2;
  EXPECT(expected == (trapping_mul<i64, u32, u32>(u32_max, 2)));
  EXPECT(expected == (trapping_mul<i64, u32, u16>(u32_max, 2)));
}

void TestSub() {
  EXPECT_DEATH((trapping_sub<i32, i32, i32>(i32_min, 1)));
  EXPECT_DEATH((trapping_sub<i16, i32, i32>(i32_min, 0)));
  EXPECT_DEATH((trapping_sub<u32, u32, u32>(u32_min, 1)));
  EXPECT_DEATH((trapping_sub<u16, u32, u32>(u32_min, 1)));
  EXPECT_DEATH((trapping_sub<i16, i16, i16>(i16_min, 1)));
  EXPECT_DEATH((trapping_sub<i8, i16, i16>(i16_min, 0)));
  EXPECT_DEATH((trapping_sub<u16, u16, u16>(u16_min, 1)));
  EXPECT_DEATH((trapping_sub<u8, u16, u16>(u16_min, 1)));

  const i64 expected = trapping_cast<i64>(u32_min) - 1;
  EXPECT(expected == (trapping_sub<i64, u32, u32>(u32_min, 1)));
  EXPECT(expected == (trapping_sub<i64, u32, u16>(u32_min, 1)));
}

template <typename T>
void GenericTestDiv() {
  using U = typename make_unsigned<T>::type;
  constexpr U u_max = numeric_limits<U>::max();
  const T expected = trapping_cast<T>(u_max / 2);
  EXPECT(expected == (trapping_div<T, U, U>(u_max, 2)));
}

template <class... T>
void CallGenericTestDiv() {
  (GenericTestDiv<T>(), ...);
}

void TestDiv() {
  EXPECT_DEATH((trapping_div<i16, i32, i32>(i32_max, 2)));
  EXPECT_DEATH((trapping_div<u16, u32, u32>(u32_max, 2)));
  EXPECT_DEATH((trapping_div<i8, i16, i16>(i16_max, 1)));
  EXPECT_DEATH((trapping_div<u8, u16, u16>(u16_max, 1)));

  {
    const i64 expected = trapping_cast<i64>(u32_max) / 2;
    EXPECT(expected == (trapping_div<i64, u32, u32>(u32_max, 2)));
    EXPECT(expected == (trapping_div<i64, u32, u16>(u32_max, 2)));
  }

  {
    const i32 expected = trapping_cast<i32>(u32_max / 2);
    EXPECT(expected == (trapping_div<i32, u32, u32>(u32_max, 2)));
    EXPECT(expected == (trapping_div<i32, u32, u16>(u32_max, 2)));
  }

  CallGenericTestDiv<i8, i16, i32, i64>();
}

template <typename T>
void GenericTestMod() {
  using U = typename make_unsigned<T>::type;
  constexpr U u_max = numeric_limits<U>::max();
  constexpr T t_max = numeric_limits<T>::max();

  {
    constexpr T expected{1};
    EXPECT(expected == (trapping_mod<T, U, U>(u_max, 2)));
    EXPECT(expected == (trapping_mod<T, T, T>(t_max, 2)));
    EXPECT(expected == (trapping_mod<T>(t_max, 2)));
    EXPECT(expected == (trapping_mod<i8, T>(t_max, 2)));
    EXPECT(expected == (trapping_mod<i8, U, U>(t_max, 2)));
  }
  {
    constexpr T max = t_max - 1;
    constexpr T expected{0};
    EXPECT(expected == (trapping_mod<T>(max, 2)));
  }
  {
    constexpr U max = u_max - 1;
    constexpr U expected{0};
    EXPECT(expected == (trapping_mod<U>(max, U{2})));
  }
}

template <class... T>
void CallGenericTestMod() {
  (GenericTestMod<T>(), ...);
}

void TestMod() {
  {
    const i64 expected = trapping_cast<i64>(u32_max) % 2;
    EXPECT(expected == (trapping_mod<i64, u32, u32>(u32_max, 2)));
    EXPECT(expected == (trapping_mod<i64, u32, u16>(u32_max, 2)));
  }

  CallGenericTestMod<i8, i16, i32, i64>();
}

void TestConstructorDefault() {
  // TODO: See comments in trapping.h. Someday, we can do this:
  // trapping<int> x;
  // EXPECT(0 == x);
}

void TestConstructorT() {
  {
    trapping<int> x = 42;
    EXPECT(x == 42);
  }
  {
    trapping<int> x{42};
    EXPECT(x == 42);
  }
  { EXPECT(trapping<int>(42) == 42); }

  {
    trapping<i8> x{42};
    EXPECT_DEATH(x = trapping<i8>(512));
    EXPECT_DEATH(((void)trapping<i8>(512)));
    EXPECT_DEATH(((void)trapping<i8>(u8_max)));
    EXPECT_DEATH(((void)trapping<i16>(u16_max)));
    EXPECT_DEATH(((void)trapping<i32>(u32_max)));
    EXPECT_DEATH(((void)trapping<i64>(u64_max)));
  }
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
    i32 y = trapping_cast<i32>(x);
    EXPECT(x == y);
    EXPECT(y == 0x0EADBEEF);
  }

  {
    u32 x = 0xDEADBEEF;
    u32 y = trapping_cast<u32>(x);
    EXPECT(x == y);
    EXPECT(y == 0xDEADBEEF);
  }

  {
    u16 x = 0xBEEF;
    i32 y = trapping_cast<i32>(x);
    EXPECT(x == y);
    EXPECT(y == 0xBEEF);
  }

  {
    i32 x = 42;
    i16 y = trapping_cast<i16>(x);
    EXPECT(x == y);
    EXPECT(y == 42);

    x = 0x0EADBEEF;
    EXPECT_DEATH((y = trapping_cast<i16>(x)));
  }

  // TODO: Use Meredith here.
  {
    u64 x = u64_max;
    i64 y = 0;
    EXPECT_DEATH((y = trapping_cast<i64>(x)));
  }

  // TODO: Use Meredith here.
  {
    i32 x = i32_min;
    i64 y = trapping_cast<i64>(x);
    EXPECT(y == i32_min);
    EXPECT(y == x);
  }
}

template <typename T>
void GenericTestOperatorAdd() {
  {
    trapping<T> x = numeric_limits<T>::max();
    EXPECT_DEATH(x += 1);
  }
  {
    trapping<T> x = numeric_limits<T>::min();
    x += 1;
    // TODO: Consider implementing overrides such that the explicit cast/ctor
    // here is not necessary.
    EXPECT(x == T{numeric_limits<T>::min() + 1});
  }
}

template <class... T>
void CallGenericTestOperatorAdd() {
  (GenericTestOperatorAdd<T>(), ...);
}

void TestOperatorAdd() {
  CallGenericTestOperatorAdd<i8, u8, i16, u16, i32, u32, i64, u64>();
}

template <typename T>
void GenericTestOperatorSub() {
  {
    trapping<T> x = numeric_limits<T>::min();
    EXPECT_DEATH(x -= 1);
  }
  {
    trapping<T> x = numeric_limits<T>::max();
    x -= 1;
    // TODO: Consider implementing overrides such that the explicit cast/ctor
    // here is not necessary (e.g. `operator==(Self, U)`, et c.).
    EXPECT(x == T{numeric_limits<T>::max() - 1});
  }
  {
    trapping<T> x = numeric_limits<T>::min();
    EXPECT_DEATH(x -= T{1});
  }
}

template <class... T>
void CallGenericTestOperatorSub() {
  (GenericTestOperatorSub<T>(), ...);
}

void TestOperatorSub() {
  CallGenericTestOperatorSub<i8, u8, i16, u16, i32, u32, i64, u64>();
}

template <typename T>
void GenericTestOperatorMul() {
  {
    trapping<T> x = numeric_limits<T>::max();
    EXPECT_DEATH(x *= 2);
  }
  {
    trapping<T> x = numeric_limits<T>::min();
    if constexpr (is_signed_v<T>) {
      EXPECT_DEATH(x *= 2);
    } else {
      EXPECT((x *= 2) == static_cast<T>(0));
    }
  }
}

template <class... T>
void CallGenericTestOperatorMul() {
  (GenericTestOperatorMul<T>(), ...);
}

void TestOperatorMul() {
  CallGenericTestOperatorMul<i8, u8, i16, u16, i32, u32, i64, u64>();
}

template <typename T>
void GenericTestOperatorDiv() {
  constexpr T max = numeric_limits<T>::max();
  {
    trapping<T> x = max;
    EXPECT((x /= 1) == static_cast<T>(max / 1));
  }
  {
    trapping<T> x = max;
    EXPECT((x /= 2) == static_cast<T>(max / 2));
  }
  {
    if constexpr (is_signed_v<T>) {
      trapping<T> x = max;
      const T expected = max / -1;
      const trapping<T> result = x /= -1;
      EXPECT(result == expected);
    }
  }
  {
    trapping<T> x = numeric_limits<T>::min();
    EXPECT((x /= 2) == static_cast<T>(numeric_limits<T>::min() / 2));
  }
  {
    if constexpr (is_signed_v<T>) {
      trapping<T> x = numeric_limits<T>::min();
      EXPECT_DEATH((x /= -1));
    }
  }
}

template <class... T>
void CallGenericTestOperatorDiv() {
  (GenericTestOperatorDiv<T>(), ...);
}

void TestOperatorDiv() {
  CallGenericTestOperatorDiv<i8, u8, i16, u16, i32, u32, i64, u64>();
}

template <typename T>
void GenericTestOperatorMod() {
  constexpr T max = numeric_limits<T>::max();
  {
    trapping<T> x = max;
    EXPECT((x %= 1) == static_cast<T>(max % 1));
  }
  {
    trapping<T> x = max;
    EXPECT((x %= 2) == static_cast<T>(max % 2));
  }
  {
    if constexpr (is_signed_v<T>) {
      trapping<T> x = max;
      const T expected = max % -1;
      const trapping<T> result = x %= -1;
      EXPECT(result == expected);
    }
  }
  {
    trapping<T> x = numeric_limits<T>::min();
    EXPECT((x %= 2) == static_cast<T>(numeric_limits<T>::min() % 2));
  }
  {
    if constexpr (is_signed_v<T>) {
      trapping<T> x = numeric_limits<T>::min();
      EXPECT_DEATH((x %= -1));
    }
  }
}

template <class... T>
void CallGenericTestOperatorMod() {
  (GenericTestOperatorMod<T>(), ...);
}

void TestOperatorMod() {
  CallGenericTestOperatorMod<i8, u8, i16, u16, i32, u32, i64, u64>();
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

template <typename T>
void GenericTestOperatorLessThan() {
  {
    trapping<int> x = 42;
    EXPECT((x < 100));
    EXPECT((12 < x));
    EXPECT(!(100 < x));
    EXPECT(!(x < 12));
  }
}

template <class... T>
void CallGenericTestOperatorLessThan() {
  (GenericTestOperatorLessThan<T>(), ...);
}

void TestOperatorLessThan() {
  CallGenericTestOperatorLessThan<i8, u8, i16, u16, i32, u32, i64, u64>();
}

template <typename T>
void GenericTestOperatorGreaterThan() {
  {
    trapping<int> x = 42;
    EXPECT((x > 12));
    EXPECT((100 > x));
    EXPECT(!(12 > x));
    EXPECT(!(x > 100));
  }
}

template <class... T>
void CallGenericTestOperatorGreaterThan() {
  (GenericTestOperatorGreaterThan<T>(), ...);
}

void TestOperatorGreaterThan() {
  CallGenericTestOperatorGreaterThan<i8, u8, i16, u16, i32, u32, i64, u64>();
}

template <typename T>
void GenericTestOperatorLessThanOrEqual() {
  {
    trapping<int> x = 42;
    EXPECT((x <= 100));
    EXPECT((12 <= x));
    EXPECT(!(100 <= x));
    EXPECT(!(x <= 12));
    EXPECT((x <= 42));
    EXPECT((42 <= x));
    EXPECT(!(x <= 12));
    EXPECT(!(100 <= x));
  }
}

template <class... T>
void CallGenericTestOperatorLessThanOrEqual() {
  (GenericTestOperatorLessThanOrEqual<T>(), ...);
}

void TestOperatorLessThanOrEqual() {
  CallGenericTestOperatorLessThanOrEqual<i8, u8, i16, u16, i32, u32, i64,
                                         u64>();
}

template <typename T>
void GenericTestOperatorGreaterThanOrEqual() {
  {
    trapping<int> x = 42;
    EXPECT((x >= 12));
    EXPECT((100 >= x));
    EXPECT(!(12 >= x));
    EXPECT(!(x >= 100));
    EXPECT((x >= 42));
    EXPECT((42 >= x));
    EXPECT(!(x >= 100));
    EXPECT(!(12 >= x));
  }
}

template <class... T>
void CallGenericTestOperatorGreaterThanOrEqual() {
  (GenericTestOperatorGreaterThanOrEqual<T>(), ...);
}

void TestOperatorGreaterThanOrEqual() {
  CallGenericTestOperatorGreaterThanOrEqual<i8, u8, i16, u16, i32, u32, i64,
                                            u64>();
}

void TestOperatorEqual() {
  {
    trapping<i32> x{i32_max};
    EXPECT((x == i32_max));
    EXPECT((i32_max == x));

    trapping<i64> y{i32_max};
    EXPECT((y == i32_max));
    EXPECT((i32_max == y));
    // TODO: Support this:
    // EXPECT((x == y));

    trapping<u64> z{i32_max};
    EXPECT((z == static_cast<u32>(i32_max)));
    EXPECT((static_cast<u32>(i32_max) == z));
    EXPECT((static_cast<u32>(x) == z));
    EXPECT((z == static_cast<u32>(x)));
  }
}

void TestOperatorNotEqual() {
  {
    trapping<i32> x{i32_max - 1};
    EXPECT((x != i32_max));
    EXPECT((i32_max != x));

    trapping<i64> y{i32_max - 1};
    EXPECT((y != i32_max));
    EXPECT((i32_max != y));
    // TODO: Support this:
    // EXPECT((x != y));

    trapping<u64> z{i32_max - 1};
    EXPECT((z != static_cast<u32>(i32_max)));
    EXPECT((static_cast<u32>(i32_max) != z));
  }
}

template <typename T>
void GenericTestOperatorIncrement() {
  constexpr T min = numeric_limits<T>::min();
  constexpr T max = numeric_limits<T>::max();
  {
    trapping<T> x{min};
    for (; x < max; x++) {
      EXPECT(x <= max);
      EXPECT(x >= min);
    }
    EXPECT(x == max);
  }
  {
    trapping<T> x{max};
    EXPECT_DEATH((x++));
  }
  {
    trapping<T> x{max};
    EXPECT_DEATH((++x));
  }
}

template <class... T>
void CallGenericTestOperatorIncrement() {
  (GenericTestOperatorIncrement<T>(), ...);
}

void TestOperatorIncrement() {
  // Unfortunately, doing them all takes too long. 🙃
  CallGenericTestOperatorIncrement<i8, u8, i16, u16 /*, i32, u32, i64, u64*/>();
}

template <typename T>
void GenericTestOperatorDecrement() {
  constexpr T min = numeric_limits<T>::min();
  constexpr T max = numeric_limits<T>::max();
  {
    trapping<T> x{max};
    for (; x > min; x--) {
      EXPECT(x <= max);
      EXPECT(x >= min);
    }
    EXPECT(x == min);
  }
  {
    trapping<T> x{min};
    EXPECT_DEATH((x--));
  }
  {
    trapping<T> x{min};
    EXPECT_DEATH((--x));
  }
}

template <class... T>
void CallGenericTestOperatorDecrement() {
  (GenericTestOperatorDecrement<T>(), ...);
}

void TestOperatorDecrement() {
  // Unfortunately, doing them all takes too long. 🙃
  CallGenericTestOperatorDecrement<i8, u8, i16, u16 /*, i32, u32, i64, u64*/>();
}

template <typename T>
void GenericTestOperatorT() {
  {
    trapping<T> x{0};
    EXPECT(0 == static_cast<T>(x));
  }
  {
    constexpr T expected = numeric_limits<T>::max();
    trapping<T> x{expected};
    EXPECT(expected == static_cast<T>(x));
  }
  {
    constexpr T expected = numeric_limits<T>::min();
    trapping<T> x{expected};
    EXPECT(expected == static_cast<T>(x));
  }
}

template <class... T>
void CallGenericTestOperatorT() {
  (GenericTestOperatorT<T>(), ...);
}

void TestOperatorT() {
  CallGenericTestOperatorT<i8, u8, i16, u16, i32, u32, i64, u64>();
}

void TestOperatorU() {
  {
    trapping<i32> x = 42;
    i16 y = static_cast<i16>(x);
    EXPECT(y == 42);
  }
  {
    trapping<i32> x = i32_max;
    auto y = static_cast<u32>(x);
    EXPECT(y == i32_max);
  }
  {
    trapping<i64> x = i64_max;
    auto y = static_cast<u64>(x);
    EXPECT(y == i64_max);
  }
  {
    trapping<i64> x = i64_max;
    i32 y;
    EXPECT_DEATH(y = x);
  }
}

template <typename T>
void GenericTestMultiOperatorOverflow() {
  using U = typename make_unsigned<T>::type;
  auto x = trapping<U>(numeric_limits<T>::max());
  // I.e. we expect headroom in `U` above the *signed* `T` maximum:
  x *= 2;
  x += 1;
  EXPECT_DEATH(x += 1);
}

template <class... T>
void CallGenericTestMultiOperatorOverflow() {
  (GenericTestMultiOperatorOverflow<T>(), ...);
}

void TestMultiOperatorOverflow() {
  CallGenericTestMultiOperatorOverflow<i8, i16, i32, i64>();
}

void TestOstream() {
  auto x = trapping<i32>(42);
  std::cout << "Testing `operator<<`: " << x << "\n";
}

template <typename T>
void GenericTestAbs() {
  {
    constexpr T expected = numeric_limits<T>::min();
    trapping<T> x{expected};
    if constexpr (is_signed_v<T>) {
      EXPECT_DEATH(abs(x));
    } else {
      EXPECT(abs(x) == expected);
    }
  }
  {
    constexpr T expected = numeric_limits<T>::max();
    trapping<T> x{expected};
    EXPECT(expected == abs(x));
  }
}

template <class... T>
void CallGenericTestAbs() {
  (GenericTestAbs<T>(), ...);
}

void TestAbs() {
  CallGenericTestAbs<i8, u8, i16, u16, i32, u32, i64, u64>();
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

  TestOstream();
  TestAbs();
}
