CXX = clang++
CXXFLAGS = -Weverything -Werror -Wno-poison-system-directories -Wno-c++98-compat -O0
FORMAT = clang-format
FORMAT_FLAGS = -i -style=Chromium
INSTALL_DIR = $(HOME)/include/integers

default: clean test

test: test_20 test_17

test_20: trapping_test_20 wrapping_test_20 clamping_test_20 ranged_test_20
	./trapping_test_20
	./wrapping_test_20
	./clamping_test_20
	./ranged_test_20

trapping_test_20: trapping_test.cc trapping.h trap.h is_integral.h test_support.h test_support.o
	$(CXX) $(CXXFLAGS) -std=c++20 trapping_test.cc test_support.o -o trapping_test_20

wrapping_test_20: wrapping_test.cc wrapping.h is_integral.h test_support.h test_support.o
	$(CXX) $(CXXFLAGS) -std=c++20 wrapping_test.cc test_support.o -o wrapping_test_20

clamping_test_20: clamping_test.cc clamping.h is_integral.h test_support.h test_support.o
	$(CXX) $(CXXFLAGS) -std=c++20 clamping_test.cc test_support.o -o clamping_test_20

ranged_test_20: ranged_test.cc ranged.h trap.h is_integral.h test_support.h test_support.o
	$(CXX) $(CXXFLAGS) -std=c++20 ranged_test.cc test_support.o -o ranged_test_20

test_17: trapping_test_17 wrapping_test_17 clamping_test_17 ranged_test_17
	./trapping_test_17
	./wrapping_test_17
	./clamping_test_17
	./ranged_test_17

trapping_test_17: trapping_test.cc trapping.h trap.h is_integral.h test_support.h test_support.o
	$(CXX) $(CXXFLAGS) -std=c++17 trapping_test.cc test_support.o -o trapping_test_17

wrapping_test_17: wrapping_test.cc wrapping.h is_integral.h test_support.h test_support.o
	$(CXX) $(CXXFLAGS) -std=c++17 wrapping_test.cc test_support.o -o wrapping_test_17

clamping_test_17: clamping_test.cc clamping.h is_integral.h test_support.h test_support.o
	$(CXX) $(CXXFLAGS) -std=c++17 clamping_test.cc test_support.o -o clamping_test_17

ranged_test_17: ranged_test.cc ranged.h trap.h is_integral.h test_support.h test_support.o
	$(CXX) $(CXXFLAGS) -std=c++17 ranged_test.cc test_support.o -o ranged_test_17

size:
	wc *.{h,cc}

push:
	git push -u origin

format:
	$(FORMAT) $(FORMAT_FLAGS) *.{cc,h}

demo: demo.cc trapping.h
	# Try setting -DNDEBUG also.
	$(CXX) -std=c++20 demo.cc -o demo

install: clamping.h in_range.h is_integral.h ranged.h test_support.h trap.h trapping.h wrapping.h
	mkdir -p $(INSTALL_DIR)
	cp $^ $(INSTALL_DIR)

clean:
	-rm -f trapping_test_20 wrapping_test_20 clamping_test_20 ranged_test_20
	-rm -f trapping_test_17 wrapping_test_17 clamping_test_17 ranged_test_17
	-rm -f demo
	-rm -f *.o
	-rm -rf *.dSYM
