# 🌄 C++ Raytracer

This project implements a basic raytracer in modern `C++20`, inspired by the **Ray Tracing in One Weekend** series. It renders 3D scenes with support for multiple materials, physically-based lighting, 
and performance optimizations like bounding volume hierarchies and multithreading. The system is modular, extensible, and designed with clean object-oriented principles.

---

## 🧠 Design Patterns Used

| Pattern   | Purpose                                                       |
|-----------|---------------------------------------------------------------|
| **Factory**  | Used in `MaterialFactory` to instantiate materials cleanly |
| **Strategy** | Implemented through `Material` subclasses for polymorphic scattering behavior |

---

## 📁 File Structure
<pre>
├── main.cpp                 # Program entry: scene setup and rendering
├── CMakeLists.txt           # CMake build configuration
├── Renderer.h               # Multithreaded rendering engine, image config: resolution, samples, output
├── Material.h               # Abstract base Material + subclasses (Diffuse, Glossy, etc.)
├── MaterialFactory.h        # Factory pattern for creating Material instances
├── Scene.h                  # Scene composed multiple instances, abstract Object class, geometric primitives
├── BVHNode.h                # Bounding Volume Hierarchy for acceleration
├── AABB.h                   # Axis-Aligned Bounding Box
├── Camera.h                 # Camera position, direction, FOV
├── Ray.h                    # Ray class used for tracing
├── HitRecord.h              # Stores hit data (point, normal, material, etc.)
├── Vector3.h                # 3D vector operations
├── Color3.h                 # RGB color utilities and tone correction
├── Interval.h               # Clamp and range utilities
└── HelperFunctions.h        # Math helpers, random functions, constants
</pre>

---

## 📐 UML Class Diagram



Key components:


---

## ⚙️ How to Build and Run

### ✅ Prerequisites
- `C++20` compatible compiler (e.g. `g++`, `clang++`)
- `CMake` version 3.23+
- macOS, Linux, or Windows with `CMake` installed

### 🛠️ Build Instructions
Follow these steps to build the project manually:
```bash
git clone https://github.com/hnmai123/ray-tracer.git
cd ray-tracer
mkdir build
cd build
cmake ..
make
```
### 🚀 Run Instructions
Follow these steps to run the **Raytracer** and view the output image
```bash
./raytracer
```
#### macOS
```bash
open image.ppm
```
#### Linux
```bash
sudo apt install imagemagick
display image.ppm
```
#### Windows
```bash
git clone https://github.com/hnmai123/ray-tracer.git
cd ray-tracer
mkdir build
cd build
cmake .. -G "MinGW Makefiles"
mingw32-make
.\raytracer.exe
```





