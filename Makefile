CXX = clang++-10
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic -Werror -ggdb -ferror-limit=2

all: json.hpp main.cpp
	@echo "  CXX   main.cpp"
	@$(CXX) $(CXXFLAGS) main.cpp -o main

tests: json.hpp test/tests.cpp
	@echo "  CXX   tests.cpp"
	@$(CXX) $(CXXFLAGS) test/tests.cpp -o tests

