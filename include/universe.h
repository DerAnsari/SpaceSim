#ifndef SPACESIM_UNIVERSE_H
#define SPACESIM_UNIVERSE_H

#include <vector>
#include <glm/glm.hpp>
#include "SimObject.h"

class Universe {
private:
  std::vector<SimObject *> bodies;

public:
  void addBody(SimObject *b) {
    bodies.size();
    bodies.push_back(b);
  }

  void renderAll(unsigned int shader, Circle &renderer) {
    for (auto b: bodies) {
      b->draw(shader, renderer);
    }
  }

  void updatePhysics(float dt) {
    const float G = 0.001f;
    const float softening = 0.01f;

    for (size_t i = 0; i < bodies.size(); ++i) {
      // --- THE STATIC SUN FIX ---
      // We skip the outer loop for the Sun so its velocity never changes.
      // It remains an 'anchor' at (0,0).
      if (bodies[i]->mass > 100.0f) continue;

      glm::vec2 acceleration(0.0f);
      for (size_t j = 0; j < bodies.size(); ++j) {
        if (i == j) continue;

        glm::vec2 diff = bodies[j]->pos - bodies[i]->pos;
        float distSq = glm::dot(diff, diff) + (softening * softening);
        float accelMag = (G * bodies[j]->mass) / distSq;

        acceleration += accelMag * glm::normalize(diff);
      }
      bodies[i]->vel += acceleration * dt;
    }

    for (auto b: bodies) {
      b->update(dt);
    }
  }

  ~Universe() {
    for (auto b: bodies) delete b;
  }
};

#endif
