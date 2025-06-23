# Compiler and flags
# -I includes/ adds the includes directory to the header search path.
# -I sfml/include adds SFML headers to the search path.
# -L sfml/lib adds SFML libraries to the library search path.
# -fopenmp enables OpenMP support.
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -I includes/ -I sfml/include -fopenmp

# The name of the final executable file.
TARGET = simulation.exe # <--- Use .exe extension for Windows executable

# A list of all the source code files (.cpp) in the project.
SOURCES = main.cpp World.cpp EntityManager.cpp SimulationSystems.cpp Resource.cpp Tile.cpp Random.cpp GraphicsRenderer.cpp

# This line automatically creates a list of object files (.o) from the source files.
OBJECTS = $(filter %.o, $(SOURCES:.cpp=.o))

# Header files for dependency tracking
HEADERS = $(wildcard includes/*.h)

# The default rule: build the target
all: $(TARGET)

# Rule to link the target executable.
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) -L sfml/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -fopenmp

# Generic rule to compile a .cpp file into a .o (object) file.
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean up build files
# --- MODIFIED CLEAN RULE ---
clean:
	@DEL $(OBJECTS) $(TARGET) >NUL 2>&1 || true
	@echo Clean complete.

.PHONY: all clean