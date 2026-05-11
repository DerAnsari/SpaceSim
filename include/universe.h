#ifndef SPACESIM_UNIVERSE_H
#define SPACESIM_UNIVERSE_H

#include <vector>
#include <glm/glm.hpp>
#include "SimObject.h"
#include "quadtree.h"
#include <algorithm>
using std::vector;

class Universe {
  vector<SimObject *> bodies;

public:
  float G = 0.000005f;
  float softening = 0.05f;
  float theta = 0.6f;

  void addBody(SimObject *b) {
    bodies.push_back(b);
  }

  void clear() {
    for (auto b : bodies) delete b;
    bodies.clear();
  }

  size_t getBodyCount() const {
      return bodies.size();
  }

  void renderAll(unsigned int shader, Circle &renderer) const {
    for (auto b: bodies) {
      b->draw(shader, renderer);
    }
    renderer.flushInstanced(shader);
  }

  /**
   * Barnes-Hut Physics Update
   * 1. Find the spatial boundaries of all bodies.
   * 2. Build a Quadtree.
   * 3. For each body, calculate forces using the tree approximation.
   */
  void updatePhysics(float dt) const {
    if (bodies.empty()) return;

    // 1. Calculate boundaries of the universe for the root QuadNode
    float xMin = bodies[0]->getPos().x, xMax = bodies[0]->getPos().x;
    float yMin = bodies[0]->getPos().y, yMax = bodies[0]->getPos().y;
    for (auto b : bodies) {
        xMin = std::min(xMin, b->getPos().x);
        xMax = std::max(xMax, b->getPos().x);
        yMin = std::min(yMin, b->getPos().y);
        yMax = std::max(yMax, b->getPos().y);
    }

    // Make the boundary square to keep the quadtree balanced
    float size = std::max(xMax - xMin, yMax - yMin) * 1.1f;
    float xMid = (xMin + xMax) / 2.0f;
    float yMid = (yMin + yMax) / 2.0f;

    // 2. Build the Quadtree
    QuadNode<SimObject> root(xMid - size/2, xMid + size/2, yMid - size/2, yMid + size/2);
    for (auto b : bodies) {
        root.insert(b);
    }

    // 3. Calculate forces using the Quadtree (Parallelized with OpenMP)
    #pragma omp parallel for
    for (int i = 0; i < static_cast<int>(bodies.size()); ++i) {
      SimObject* b = bodies[i];
      // The Sun (mass > 100) stays fixed at the center
      if (b->getMass() > 100.0f) continue;

      glm::vec2 acceleration = root.calculateForce(b, G, softening, theta);
      b->setVel(b->getVel() + acceleration * dt);
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
