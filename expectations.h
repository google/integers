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

#ifndef EXPECTATIONS_H_
#define EXPECTATIONS_H_

#include <err.h>
#include <execinfo.h>
#include <unistd.h>

#include <iostream>

#include "trap.h"

#if defined(NDEBUG)
#error Tests must be built with `NDEBUG` undefined.
#endif

namespace integers {

void PrintBacktrace();

#if defined(NDEBUG)
#define NOTREACHED() trap();
#else
#define NOTREACHED()                                                    \
  std::cerr << "NOTREACHED at " << __FILE__ << ":" << __LINE__ << "\n"; \
  ::integers::PrintBacktrace();                                         \
  trap();
#endif

#define EXPECT(condition)                             \
  if (!(condition)) {                                 \
    std::cerr << __FILE__ << ":" << __LINE__ << "\n"; \
    ::integers::PrintBacktrace();                     \
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
        ::integers::PrintBacktrace();                                     \
        _exit(1);                                                         \
      }                                                                   \
      if (r != pid) {                                                     \
        err(errno, "Waitpid got a bad answer");                           \
      }                                                                   \
    }                                                                     \
  }

}  // namespace integers

#endif  // EXPECTATIONS_H_
