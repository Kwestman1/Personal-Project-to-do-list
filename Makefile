CXX ?= g++
CXXFLAGS ?= -Wall -Werror -pedantic -g --std=c++17 -Wno-sign-compare -Wno-comment

# Compile the main executable
main.exe: main.cpp Master.cpp File.cpp
	$(CXX) $(CXXFLAGS) main.cpp Master.cpp File.cpp -o main.exe

# Compile the test executable (include Master.cpp and File.cpp)
tests.exe: tests.cpp Master.cpp File.cpp
	$(CXX) $(CXXFLAGS) tests.cpp Master.cpp File.cpp -o tests.exe

test: tests.exe
	./tests.exe

# Remove automatically generated files
clean:
	rm -rvf *.exe *~ *.out *.dSYM *.stackdump

# Disable built-in rules
.SUFFIXES:
