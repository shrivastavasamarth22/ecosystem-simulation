# Compiler and flags
# g++ is the GNU C++ compiler.
# -std=c++17 sets the C++ standard.
# -Wall and -Wextra enable most compiler warnings, which is good practice.
# -g includes debugging information in the executable.
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# The name of the final executable file.
TARGET = simulation

# A list of all the source code files (.cpp) in the project.
# The make utility will automatically find the corresponding .h files.
SOURCES = main.cpp World.cpp Animal.cpp Herbivore.cpp Carnivore.cpp Omnivore.cpp Resource.cpp Tile.cpp

# This line automatically creates a list of object files (.o) from the source files.
# e.g., "main.cpp" becomes "main.o".
OBJECTS = $(SOURCES:.cpp=.o)

# The default rule, which is run when you just type "make".
# It says that to build "all", you first need to build the TARGET.
all: $(TARGET)

# Rule to link the target executable.
# It tells make that to build the $(TARGET), it needs all the $(OBJECTS).
# It then runs the compiler to link all the object files together into the final executable.
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

# Generic rule to compile a .cpp file into a .o (object) file.
# $< is an automatic variable that means "the first prerequisite" (the .cpp file).
# $@ is an automatic variable that means "the target" (the .o file).
# -c means "compile only, do not link".
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# A rule to clean up the directory.
# `rm -f` removes files without asking for confirmation.
# This deletes all compiled object files and the final executable.
clean:
	rm -f $(OBJECTS) $(TARGET)

# This tells make that "all" and "clean" are not actual files to be built.
.PHONY: all clean