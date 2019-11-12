CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic -Werror

all: json.hpp main.cpp
	@echo "  CXX   main.cpp"
	@$(CXX) $(CXXFLAGS) main.cpp -o main

tests: json.hpp test/tests.cpp
	@echo "  CXX   tests.cpp"
	@$(CXX) $(CXXFLAGS) test/tests.cpp -o tests

