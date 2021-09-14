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

#ifndef EXPECTATIONS_H
#define EXPECTATIONS_H

#include <unistd.h>
#include <err.h>
#include <execinfo.h>

#include <iostream>

namespace integers {

void PrintBacktrace();

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
/// death, then you should use the `*_overflow` functions above.
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

#if defined(NDEBUG)
#define NOTREACHED() trap();
#else
#define NOTREACHED()                                                    \
  std::cerr << "NOTREACHED at " << __FILE__ << ":" << __LINE__ << "\n"; \
  PrintBacktrace();                                                     \
  trap();
#endif

#define EXPECT(condition)                             \
  if (!(condition)) {                                 \
    std::cerr << __FILE__ << ":" << __LINE__ << "\n"; \
    PrintBacktrace();                                 \
    assert(false);                                    \
  }

// This is less janky than trying to install the Google Test framework using
// CMake, which doesn't build (CMakeLists.txt syntax errors). It was literally
// easier and faster to write this. Sorry.
#define EXPECT_DEATH(statement)                                           \
  {                                                                       \
    pid_t pid = fork();                                                   \
    if (pid == -1) {                                                      \
      err(errno, "Could not fork");                                       \
    }                                                                     \
    if (pid == 0) {                                                       \
      statement;                                                          \
      _exit(0);                                                           \
    } else {                                                              \
      int status = 0;                                                     \
      pid_t r = waitpid(pid, &status, 0);                                 \
      if (WIFSIGNALED(status)) {                                          \
        /*cout << "SUCCESS: Signaled, as expected.\n";*/                  \
      } else {                                                            \
        std::cerr << "FAILURE: Child exited normally. Boo!! " << __FILE__ \
                  << ":" << __LINE__ << "\n";                             \
        PrintBacktrace();                                                 \
        failure = true;                                                   \
      }                                                                   \
      if (r != pid) {                                                     \
        err(errno, "Waitpid got a bad answer");                           \
      }                                                                   \
    }                                                                     \
  }

}  // namespace integers

#endif  // EXPECTATIONS_H
