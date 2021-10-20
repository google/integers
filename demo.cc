#include <stdlib.h>

#include <iostream>
#include <limits>

#include "trapping.h"

using TrappingSizeT = integers::trapping<size_t>;

namespace {

char Help[] =
    "Usage: demo solution count\n"
    "\n"
    "`solution` is 1 of: 1, 2, 3, 4. There are 4 possible approaches to\n"
    "fixing the problem in this demo.\n"
    "\n"
    "This program simulates a vulnerable integer overflow condition by\n"
    "allowing dynamic, untrustworthy input influence the size of a\n"
    "heap allocation. It also shows a simple way to repair the problem\n"
    "with a minimal source code change that uses `trapping` to detect\n"
    "the integer overflow at run-time.\n"
    "\n"
    "The program will try to allocate `count` * object-size bytes. Try\n"
    "several values. The program will advise you about a number that\n"
    "will trigger overflow.\n"
    "\n"
    "Trapping will surface as an abort in debug builds, or as an\n"
    "illegal instruction if you build with `NDEBUG` defined.\n";

[[noreturn]] void help() {
  std::cout << Help;
  exit(1);
}

struct Friend {
  int age;
  char name[1024];
  bool wears_a_watch;
  char bio[4096];
};

// Vulnerable code like this is not uncommon:
Friend* Vulnerable(size_t count) {
  std::cerr << "Vulnerable calculation:\n";
  size_t total = count * sizeof(Friend);
  std::cerr << "count " << count << " * sizeof(Friend) " << sizeof(Friend)
            << " = " << total << "\n";
  if (total < count || total < sizeof(Friend)) {
    std::cerr << "\nLook out! The computation overflowed. Allocation may "
                 "'succeed' but the region will be too small.\n";
  }

  Friend* friends = static_cast<Friend*>(malloc(total));
  return friends;
}

// A version that changes the interface, which in this case happens to fix the
// implementation:
Friend* Checked1(TrappingSizeT count) {
  std::cerr << "Checked calculation, version 1 (`trapping` interface):\n";
  // This multiplication now traps on overflow, since `count` is TrappingSizeT.
  size_t total = count * sizeof(Friend);
  std::cerr << "count " << count << " * sizeof(Friend) " << sizeof(Friend)
            << " = " << total << "\n";

  Friend* friends = static_cast<Friend*>(malloc(total));
  return friends;
}

// A version that changes the implementation but not the interface:
Friend* Checked2(size_t count) {
  std::cerr << "Checked calculation, version 2 (`trapping` implementation):\n";

  // NOTE: Do *not* do this:
  // TrappingSizeT total = count * sizeof(Friend);
  // The overflow will happen before `trapping` can notice it. Instead, do this:
  TrappingSizeT total = count;
  total *= sizeof(Friend);

  std::cerr << "count " << count << " * sizeof(Friend) " << sizeof(Friend)
            << " = " << total << "\n";

  Friend* friends = static_cast<Friend*>(malloc(total));
  return friends;
}

// Another version that changes the implementation but not the interface. This
// version uses the `trapping_mul` function instead of the `trapping<T>`
// template class.
Friend* Checked3(size_t count) {
  std::cerr << "Checked calculation, version 3 (`trapping_mul`):\n";
  size_t total = integers::trapping_mul<size_t>(count, sizeof(Friend));

  std::cerr << "count " << count << " * sizeof(Friend) " << sizeof(Friend)
            << " = " << total << "\n";

  Friend* friends = static_cast<Friend*>(malloc(total));
  return friends;
}

// This version uses the `mul_overflow` function to multiply and then explicitly
// check for overflow. You can then handle the condition however you like.
Friend* Checked4(size_t count) {
  std::cerr << "Checked calculation, version 4 (`mul_overflow` check):\n";
  size_t total;
  if (integers::mul_overflow(count, sizeof(Friend), &total)) {
    std::cerr << "Look out! The computation overflowed. Allocation may "
                 "'succeed' but the region will be too small. Exiting.\n";
    // Your error-handling strategy here:
    exit(1);
  }

  std::cerr << "count " << count << " * sizeof(Friend) " << sizeof(Friend)
            << " = " << total << "\n";

  Friend* friends = static_cast<Friend*>(malloc(total));
  return friends;
}
}  // namespace

int main(int count, char* arguments[]) {
  if (count != 3) {
    help();
  }

  std::cerr << "sizeof(Friend): " << sizeof(Friend) << "\n";
  std::cerr << "sizeof(Friend) / numeric_limits<size_t>::max(): "
            << (std::numeric_limits<size_t>::max() / sizeof(Friend)) << "\n";
  std::cerr << "If you pass a larger number, the calculation is likely to "
               "overflow.\n\n";

  int checked_version = atoi(arguments[1]);

  // `strtoll` returns `long long`, but for `malloc` we need `size_t`. Use the
  // helper function to make sure the cast is safe. Don't just use
  // `static_cast`!
  size_t friend_count =
      integers::trapping_cast<size_t>(strtoll(arguments[2], NULL, 0));

  {
    Friend* friends = Vulnerable(friend_count);
    std::cerr << friends << "\n";
  }

  std::cerr << "\n";

  std::cerr << "If your count is too high, this should crash instead of "
               "continue.\n";
  {
    Friend* friends;
    switch (checked_version) {
      case 1:
        // If `friend_count` were `long long`, we could explicitly cast to
        // `TrappingSizeT` here and enjoy the safety check in the constructor.
        friends = Checked1(friend_count);
        break;
      case 2:
        friends = Checked2(friend_count);
        break;
      case 3:
        friends = Checked3(friend_count);
        break;
      case 4:
        friends = Checked4(friend_count);
        break;
    }
    std::cerr << friends << "\n";
  }
}
