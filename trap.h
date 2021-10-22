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

#ifndef TRAP_H_
#define TRAP_H_

/// ## Utility Functions
///
/// ### `trap`
///
/// Crashes the program immediately.
///
/// Depending on your compiler, this may be done by a trap instruction or by
/// invoking `std::abort`. This function will also use `abort` unless
/// `NDEBUG` is defined, since that yields better debug info than a plain
/// trap instruction.
///
/// Note that the rest of the functions and classes in this library handle
/// errors by calling this function. If you don’t want immediate fiery
/// death, then you should use the `*_overflow` functions.
///
/// However, may we humbly suggest that immediate fiery death is actually
/// quite desirable? If you find that surprising, which is not unreasonable
/// since it is surprising, you might enjoy [“Crash-Only Software” by Candea
/// and
/// Fox](https://www.usenix.org/legacy/events/hotos03/tech/full_papers/candea/candea.pdf).
#if __has_builtin(__builtin_trap) && defined(NDEBUG)
#define trap() __builtin_trap();
#else
#define trap() abort();
#endif

#endif  // TRAP_H_
