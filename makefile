CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
TARGET = compiler
SOURCES = qur.cpp utils/lexer.cpp utils/ast.cpp
OBJECTS = $(SOURCES:.cpp=.o)
HEADERS = lexer.h ast.h

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $<

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET)

# Rebuild everything
rebuild: clean all

.PHONY: all test test-all test-fixed clean rebuild
