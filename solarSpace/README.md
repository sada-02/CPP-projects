# 🌌 SolarSpace - 3D Solar System Simulation

A real-time 3D solar system simulation implemented in C++ using OpenGL. This project features realistic planetary motion with gravitational physics, and beautiful visual effects including orbital trails and Saturn's rings.

![C++](https://img.shields.io/badge/C++-17-red)
![Platform](https://img.shields.io/badge/Platform-Linux-green)

## ✨ Features

### 🪐 Astronomical Objects
- **The Sun** - Central star with realistic golden appearance
- **8 Planets** - All planets of our solar system
- **Moons** - Natural satellites orbiting Earth, Jupiter, and Saturn
- **Asteroid Belt** - Thousands of asteroids between Mars and Jupiter
- **Saturn's Rings** - Beautiful ring system around Saturn

### ⚡ Physics Simulation
- **Gravitational Forces** - Realistic n-body gravitational simulation
- **Orbital Mechanics** - Planets follow elliptical orbits based on physics
- **Real-time Updates** - Smooth 60fps simulation with proper time stepping
- **Orbital Trails** - Visual trails showing planetary paths through space

## 🚀 Getting Started

### Prerequisites

Make sure you have the following dependencies installed on your Linux system:

```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential libglew-dev libglfw3-dev libgl1-mesa-dev

# Fedora/CentOS/RHEL
sudo dnf install gcc-c++ glew-devel glfw-devel mesa-libGL-devel

# Arch Linux
sudo pacman -S base-devel glew glfw-x11 mesa
```

### Building the Project

1. **Clone or download the project**
2. **Navigate to the project directory**
   ```bash
   cd solarSpace
   ```
3. **Compile using Make**
   ```bash
   make
   ```
4. **Run the simulation**
   ```bash
   ./solarSystem
   ```

### System Requirements
- Linux distribution
- OpenGL 3.0+ compatible graphics card
- At least 2GB RAM
- C++17 compatible compiler (GCC 7+ or Clang 5+)

## 🤝 Contributing

Feel free to contribute to this project by:
1. Adding new celestial objects (comets, more moons, etc.)
2. Implementing new visual effects (particle systems, lighting)
3. Optimizing performance or adding new features
4. Improving the physics simulation accuracy
5. Adding texture mapping to planets

## 📚 Educational Value

This project demonstrates:
- **3D Computer Graphics** - OpenGL programming and 3D mathematics
- **Physics Simulation** - Gravitational forces and orbital mechanics  
- **Object-Oriented Design** - Clean C++ class architecture
- **Real-time Rendering** - Efficient graphics programming techniques