CXX = clang++
CXXFLAGS = -Weverything -Werror -O3 -std=c++20 -Wno-poison-system-directories -Wno-c++98-compat -g
FORMAT = clang-format
FORMAT_FLAGS = -i -style=Chromium

default: clean test

test: trapping_test wrapping_test clamping_test ranged_test
	./trapping_test
	./wrapping_test
	./clamping_test
	./ranged_test

trapping_test: trapping_test.cc trapping.h expectations.h expectations.o
	$(CXX) $(CXXFLAGS) -Wno-old-style-cast trapping_test.cc expectations.o -o trapping_test 2> err

wrapping_test: wrapping_test.cc wrapping.h expectations.h expectations.o
	$(CXX) $(CXXFLAGS) wrapping_test.cc expectations.o -o wrapping_test 2> err

clamping_test: clamping_test.cc clamping.h expectations.h expectations.o
	$(CXX) $(CXXFLAGS) clamping_test.cc expectations.o -o clamping_test 2> err

ranged_test: ranged_test.cc ranged.h expectations.h expectations.o
	$(CXX) $(CXXFLAGS) ranged_test.cc expectations.o -o ranged_test 2> err

size:
	wc *.{h,cc}

format:
	$(FORMAT) $(FORMAT_FLAGS) *.{cc,h}

clean:
	-rm -f trapping_test wrapping_test clamping_test ranged_test *.o
	-rm -f err
	-rm -rf *.dSYM
