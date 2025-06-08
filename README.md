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

![UML_RayTracer drawio](https://github.com/user-attachments/assets/8965d7e8-b8c4-4ab7-a4f3-fffb35560d24)<br>
The project follows a clean object-oriented design, modeled through a comprehensive UML class diagram. Key components include:
- `Object` is an abstract base class implemented by `Sphere`, `Plane`, `Scene`, and `BVHNode`.
- `Material` is an abstract class with subclasses like `PureDiffuse`, `Reflective`, `Glossy`, `Dielectric`, `Checker`, and `Emissive`, applying the Strategy Pattern.
- `MaterialFactory` applies the Factory Pattern to centralize material creation.
- `Renderer` manages the ray tracing loop and uses multithreading to render efficiently.
- `Camera` generates rays per pixel; `Ray`, `Interval`, and `HitRecord` are utility classes involved in intersection and shading.
- The UML includes associations, dependencies, generalizations, and clear visibility (`+` public, `-` private) on attributes and methods.
  
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
## 📸 Sample Output
<p align="center">
  <img src="https://github.com/user-attachments/assets/44954946-ae4d-4530-997f-7076ec270ec9" width="49%">
  <img src="https://github.com/user-attachments/assets/d624a58b-eff6-464a-a2e0-562e0c5e2396" width="49%">
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/6e12c3be-9703-4294-8fe3-2495c08bbd5e" width="49%">
  <img src="https://github.com/user-attachments/assets/25b1e574-f381-439f-a167-730b541d2813" width="49%">
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/f6590245-eaf6-4c22-86c4-abc17e677f7a" width="49%">
  <img src="https://github.com/user-attachments/assets/e3be4141-6c9c-4452-a381-247b6c7baa46" width="49%">
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/711e11a7-bd9a-4666-a1f1-8f216a3b664c" width="49%">
  <img src="https://github.com/user-attachments/assets/cf1a4233-674e-46e5-a760-51d17f7e735c" width="49%">
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/ed8895f6-2c7b-4fd5-83bb-9c7407f359b8" width="49%">
  <img src="https://github.com/user-attachments/assets/84fe8921-4d99-4eb0-aef3-e5cf58ec6767" width="49%">
</p>







