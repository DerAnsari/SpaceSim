# SpaceSim

A high-performance gravitational physics simulator built with C++ and OpenGL.

## Features
* **Barnes-Hut Algorithm:** Optimized $O(N \log N)$ force calculation using a Quadtree.
* **Physics Integrators:** Supports Euler, Verlet, and RK4 integration.
* **Real-time UI:** Uses Dear ImGui for live parameter tuning.
* **Efficient Rendering:** OpenGL instanced rendering for thousands of particles.

## Build Requirements
* C++17 Compiler
* CMake
* OpenGL & GLFW
* Dear ImGui (included as submodule)


