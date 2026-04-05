#ifndef SPACESIM_SIMOBJECT_H
#define SPACESIM_SIMOBJECT_H

#include <glm/glm.hpp>
#include "circle.h"

// Base class (Inheritance)
class SimObject {
public:
  glm::vec2 pos;
  glm::vec2 vel;
  float mass;
  float radius;

  SimObject(float x, float y, float m, float r)
    : pos(x, y), vel(0.0f), mass(m), radius(r) {
  }

  virtual ~SimObject() = default;

  // This is Polymorphism: different objects draw differently
  virtual void draw(unsigned int shader, Circle &renderer) = 0;

  void update(float dt) {
    pos += vel * dt;

    // Bounce off walls
    //if (pos.x > 1.0f || pos.x < -1.0f) vel.x *= -1.0f;
    //if (pos.y > 1.0f || pos.y < -1.0f) vel.y *= -1.0f;
  }
};

// A Star (maybe it's bigger/brighter)
class Star : public SimObject {
public:
  using SimObject::SimObject;

  void draw(unsigned int shader, Circle &renderer) override {
    // You could set a different color uniform here
    renderer.draw(shader, pos.x, pos.y, radius);
  }
};

// A Planet
class Planet : public SimObject {
public:
  using SimObject::SimObject;

  void draw(unsigned int shader, Circle &renderer) override {
    renderer.draw(shader, pos.x, pos.y, radius);
  }
};

#endif //SPACESIM_SIMOBJECT_H
