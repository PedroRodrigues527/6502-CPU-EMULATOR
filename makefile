# Compiler
CXX := g++

# Compiler flags
CXXFLAGS := -std=c++17 -Wall -Wextra -O0 -g3

# Target binary
TARGET := 6502_processor

# Source files
SRC := $(wildcard src/*.cpp src/*/*.cpp src/*/*/*.cpp)

# Default target
all: build

# Build only
build:
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Build and run (equivalente ao compile.sh)
run: build
	./$(TARGET)

# Build and run tests (equivalente ao test_cpu.sh)
test: build
	./$(TARGET) --check_errors

# Clean build artifacts
clean:
	rm -f $(TARGET)

# Phony targets
.PHONY: all build run test clean
