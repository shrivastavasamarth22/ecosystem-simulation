# Compiler and flags
# -fopenmp enables OpenMP support
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -I includes/ -fopenmp

# The name of the final executable file.
TARGET = simulation

# Source files
SOURCES = main.cpp World.cpp EntityManager.cpp SimulationSystems.cpp Resource.cpp Tile.cpp Random.cpp

# This line automatically creates a list of object files (.o) from the source files.
OBJECTS = $(filter %.o, $(SOURCES:.cpp=.o))

# Header files for dependency tracking
HEADERS = $(wildcard includes/*.h)

# The default rule: build the target
all: $(TARGET)

# Rule to link the target executable. Add -fopenmp for the linker.
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) -fopenmp

# Generic rule to compile a .cpp file into a .o (object) file.
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean up build files
clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean