CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Libraries for OpenGL, GLEW, and GLFW
LIBS = -lGLEW -lglfw -lGL -ldl -lpthread -lm

# Output Executable
TARGET = solarSystem

# Source file
SRC = solarSys.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LIBS)

clean:
	rm -f $(TARGET)
