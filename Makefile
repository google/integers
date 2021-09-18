CXX = clang++
CXXFLAGS = -Weverything -Werror -Wno-poison-system-directories -Wno-c++98-compat -O0
FORMAT = clang-format
FORMAT_FLAGS = -i -style=Chromium

default: clean test

test: test_20 test_17

test_20: trapping_test_20 wrapping_test_20 clamping_test_20 ranged_test_20
	./trapping_test_20
	./wrapping_test_20
	./clamping_test_20
	./ranged_test_20

trapping_test_20: trapping_test.cc trapping.h test_support.h test_support.o
	$(CXX) $(CXXFLAGS) -std=c++20 trapping_test.cc test_support.o -o trapping_test_20 2> err

wrapping_test_20: wrapping_test.cc wrapping.h test_support.h test_support.o
	$(CXX) $(CXXFLAGS) -std=c++20 wrapping_test.cc test_support.o -o wrapping_test_20 2> err

clamping_test_20: clamping_test.cc clamping.h test_support.h test_support.o
	$(CXX) $(CXXFLAGS) -std=c++20 clamping_test.cc test_support.o -o clamping_test_20 2> err

ranged_test_20: ranged_test.cc ranged.h test_support.h test_support.o
	$(CXX) $(CXXFLAGS) -std=c++20 ranged_test.cc test_support.o -o ranged_test_20 2> err

test_17: trapping_test_17 wrapping_test_17 clamping_test_17 ranged_test_17
	./trapping_test_17
	./wrapping_test_17
	./clamping_test_17
	./ranged_test_17

trapping_test_17: trapping_test.cc trapping.h test_support.h test_support.o
	$(CXX) $(CXXFLAGS) -std=c++17 trapping_test.cc test_support.o -o trapping_test_17 2> err

wrapping_test_17: wrapping_test.cc wrapping.h test_support.h test_support.o
	$(CXX) $(CXXFLAGS) -std=c++17 wrapping_test.cc test_support.o -o wrapping_test_17 2> err

clamping_test_17: clamping_test.cc clamping.h test_support.h test_support.o
	$(CXX) $(CXXFLAGS) -std=c++17 clamping_test.cc test_support.o -o clamping_test_17 2> err

ranged_test_17: ranged_test.cc ranged.h test_support.h test_support.o
	$(CXX) $(CXXFLAGS) -std=c++17 ranged_test.cc test_support.o -o ranged_test_17 2> err

size:
	wc *.{h,cc}

push:
	git push -u origin

format:
	$(FORMAT) $(FORMAT_FLAGS) *.{cc,h}

clean:
	-rm -f trapping_test_20 wrapping_test_20 clamping_test_20 ranged_test_20
	-rm -f trapping_test_17 wrapping_test_17 clamping_test_17 ranged_test_17
	-rm -f *.o
	-rm -f err
	-rm -rf *.dSYM
