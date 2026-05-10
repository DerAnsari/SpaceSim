#ifndef SPACESIM_SIMOBJECT_H
#define SPACESIM_SIMOBJECT_H

#include <glm/glm.hpp>
#include "circle.h"

// Base class (Inheritance)
class SimObject {
protected:
  glm::vec2 pos;
  glm::vec2 vel;
  float mass;
  float radius;

public:
  SimObject(float x, float y, float m, float r)
    : pos(x, y), vel(0.0f), mass(m), radius(r) {
  }

  virtual ~SimObject() = default;

  // Polymorphism: different objects draw by queuing themselves into the renderer
  virtual void draw(unsigned int shader, Circle &renderer) = 0;

  void applyForce(glm::vec2 force, float dt) {
      glm::vec2 acceleration = force / mass;
      vel += acceleration * dt;
  }

  // Symplectic Euler: update position using the already updated velocity
  void update(float dt) {
    pos += vel * dt;
  }

  // Getters for Physics & Rendering
  glm::vec2 getPos() const { return pos; }
  glm::vec2 getVel() const { return vel; }
  void setVel(glm::vec2 v) { vel = v; }
  float getMass() const { return mass; }
  float getRadius() const { return radius; }
};

// A Star
class Star : public SimObject {
public:
  using SimObject::SimObject;

  void draw(unsigned int shader, Circle &renderer) override {
    renderer.queueInstance(pos.x, pos.y, radius);
  }
};

// A Planet
class Planet : public SimObject {
public:
  using SimObject::SimObject;

  void draw(unsigned int shader, Circle &renderer) override {
    renderer.queueInstance(pos.x, pos.y, radius);
  }
};

#endif //SPACESIM_SIMOBJECT_H
