# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# Target executable name
TARGET = simulation

# Source files
SOURCES = main.cpp World.cpp Animal.cpp Herbivore.cpp Carnivore.cpp

# Object files are derived from source files
OBJECTS = $(SOURCES:.cpp=.o)

# Default rule: build the target
all: $(TARGET)

# Rule to link the target executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Rule to compile a .cpp file into a .o file
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean up build files
clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean