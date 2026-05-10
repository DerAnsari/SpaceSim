# SpaceSim: N-Body Gravity Simulation

SpaceSim is a high-performance, real-time 2D gravity simulation built to explore the complexities of the N-body problem. Developed in C++ using modern Object-Oriented Design (OOD) principles, the project simulates thousands of celestial bodies interacting under mutual gravitational attraction at interactive speeds.

[![SpaceSim Demo](https://img.youtube.com/vi/qdOf5vv4h2Y/maxresdefault.jpg)](https://www.youtube.com/watch?v=qdOf5vv4h2Y)

## Features
- **Real-Time Performance:** Handles upwards of 10,000 simultaneous bodies at a stable 60+ FPS.
- **Optimized Physics:** Utilizes the Barnes-Hut algorithm to reduce computational complexity from $O(N^2)$ to $O(N \log N)$.
- **Hardware Acceleration:** Leverages OpenGL 3.3 and instanced rendering to draw thousands of entities in a single GPU call.
- **Stable Integration:** Employs a semi-implicit Symplectic Euler integrator and gravitational softening to ensure physical stability.
- **Parallel Computing:** Uses OpenMP to distribute physics calculations across multiple CPU cores.
- **Cross-Platform:** Built with CMake, offering standalone, zero-dependency binaries for both Linux and Windows.

## Project Architecture
The system is designed with a decoupled architecture to separate physics logic from rendering:
- **Universe Manager:** The central orchestrator that manages the application lifecycle, boundary detection, and quadtree reconstruction.
- **SimObject Hierarchy:** A polymorphic base class for celestial entities (Stars, Planets) that handles physical states like position, velocity, and mass.
- **Quadtree:** A recursive spatial partitioning structure used to optimize force calculations.
- **Renderer:** A dedicated OpenGL pipeline using custom GLSL shaders for procedural "misty" galaxy aesthetics.

## Tech Stack
- **Language:** C++20
- **Graphics:** OpenGL 3.3 (Core Profile), GLFW, GLAD
- **Math:** GLM (OpenGL Mathematics)
- **UI:** Dear ImGui for real-time parameter tuning
- **Build System:** CMake
- **Parallelism:** OpenMP

## Implementation Highlights
The simulation solves the N-body problem numerically. Key highlights include:
- **Numerical Stability:** Uses gravitational softening to prevent mathematical singularities when bodies get too close.
- **Procedural Aesthetics:** Visuals are enhanced with fragment shaders that create radial glows and distance-based color gradients.
- **Dynamic Tree Rebuilding:** The Quadtree is rebuilt every frame to accommodate the moving bodies across the spatial domain.
