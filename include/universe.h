#ifndef SPACESIM_UNIVERSE_H
#define SPACESIM_UNIVERSE_H

#include <vector>
#include <glm/glm.hpp>
#include "SimObject.h"
#include "quadtree.h"
#include <algorithm>

class Universe {
private:
  std::vector<SimObject *> bodies;

public:
  void addBody(SimObject *b) {
    bodies.push_back(b);
  }

  void renderAll(unsigned int shader, Circle &renderer) {
    for (auto b: bodies) {
      b->draw(shader, renderer);
    }
  }

  /**
   * Barnes-Hut Physics Update
   * 1. Find the spatial boundaries of all bodies.
   * 2. Build a Quadtree.
   * 3. For each body, calculate forces using the tree approximation.
   */
  void updatePhysics(float dt) {
    if (bodies.empty()) return;

    const float G = 0.000005f; // Lowered to account for 5x more mass in the system
    const float softening = 0.05f; // Slightly more softening for high density
    const float theta = 0.6f; // Lower theta for higher precision (prevents drift)

    // 1. Calculate boundaries of the universe for the root QuadNode
    float xMin = bodies[0]->pos.x, xMax = bodies[0]->pos.x;
    float yMin = bodies[0]->pos.y, yMax = bodies[0]->pos.y;
    for (auto b : bodies) {
        xMin = std::min(xMin, b->pos.x);
        xMax = std::max(xMax, b->pos.x);
        yMin = std::min(yMin, b->pos.y);
        yMax = std::max(yMax, b->pos.y);
    }

    // Make the boundary square to keep the quadtree balanced
    float size = std::max(xMax - xMin, yMax - yMin) * 1.1f; // 10% padding
    float xMid = (xMin + xMax) / 2.0f;
    float yMid = (yMin + yMax) / 2.0f;

    // 2. Build the Quadtree
    QuadNode root(xMid - size/2, xMid + size/2, yMid - size/2, yMid + size/2);
    for (auto b : bodies) {
        root.insert(b);
    }

    // 3. Calculate forces using the Quadtree
    for (size_t i = 0; i < bodies.size(); ++i) {
      // The Sun (mass > 100) stays fixed at the center
      if (bodies[i]->mass > 100.0f) continue;

      glm::vec2 acceleration = root.calculateForce(bodies[i], G, softening, theta);
      bodies[i]->vel += acceleration * dt;
    }

    // 4. Update positions
    for (auto b: bodies) {
      b->update(dt);
    }
  }


  ~Universe() {
    for (auto b: bodies) delete b;
  }
};

#endif
