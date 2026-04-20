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
    // Reduced G for slower, more majestic movement
    const float G = 0.0001f; 
    // Lower softening makes particles look like sharp points again
    const float softening = 0.1f;

    for (size_t i = 0; i < bodies.size(); ++i) {
      if (bodies[i]->mass > 100.0f) continue;

      glm::vec2 acceleration(0.0f);
      for (size_t j = 0; j < bodies.size(); ++j) {
        if (i == j) continue;

        glm::vec2 diff = bodies[j]->pos - bodies[i]->pos;
        float distSq = glm::dot(diff, diff) + (softening * softening);
        float dist = std::sqrt(distSq);
        
        float accelMag = (G * bodies[j]->mass) / distSq;
        acceleration += (accelMag / dist) * diff;
      }
      
      bodies[i]->vel += acceleration * dt;
    }

    // Update positions based on the new velocities
    for (auto b: bodies) {
      b->update(dt);
    }
  }

  ~Universe() {
    for (auto b: bodies) delete b;
  }
};

#endif
