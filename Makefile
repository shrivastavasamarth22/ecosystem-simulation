# Compiler and flags
# -I include/ adds the include directory to the header search path.
# -I external/SFML/include adds SFML headers to the search path.
# -L external/SFML/lib adds SFML libraries to the library search path.
# -fopenmp enables OpenMP support.
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g -I include/ -I external/SFML/include -fopenmp

# The name of the final executable file.
TARGET = simulation.exe # <--- Use .exe extension for Windows executable

# Source directories
SRC_DIR = src
BUILD_DIR = build/obj

# A list of all the source code files (.cpp) in the project.
SOURCES = $(SRC_DIR)/main.cpp \
          $(SRC_DIR)/core/World.cpp \
          $(SRC_DIR)/core/EntityManager.cpp \
          $(SRC_DIR)/core/Random.cpp \
          $(SRC_DIR)/systems/SimulationSystems.cpp \
          $(SRC_DIR)/resources/Resource.cpp \
          $(SRC_DIR)/resources/Tile.cpp \
          $(SRC_DIR)/resources/Biome.cpp \
          $(SRC_DIR)/graphics/GraphicsRenderer.cpp \
          $(SRC_DIR)/graphics/Camera.cpp \
          $(SRC_DIR)/graphics/UIManager.cpp

# Object files in build directory
OBJECTS = $(BUILD_DIR)/main.o \
          $(BUILD_DIR)/World.o \
          $(BUILD_DIR)/EntityManager.o \
          $(BUILD_DIR)/Random.o \
          $(BUILD_DIR)/SimulationSystems.o \
          $(BUILD_DIR)/Resource.o \
          $(BUILD_DIR)/Tile.o \
          $(BUILD_DIR)/Biome.o \
          $(BUILD_DIR)/GraphicsRenderer.o \
          $(BUILD_DIR)/Camera.o \
          $(BUILD_DIR)/UIManager.o

# Header files for dependency tracking
HEADERS = $(wildcard include/*/*.h)

# The default rule: build the target
all: $(TARGET)

# Rule to link the target executable.
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) -L external/SFML/lib -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -fopenmp

# Generic rules to compile source files into object files in build directory
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/core/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/systems/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/resources/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/graphics/%.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Rule to clean up build files
clean:
	-@DEL "$(subst /,\,$(BUILD_DIR))\*.o" "$(TARGET)" >NUL 2>&1
	@echo Clean complete.

.PHONY: all clean