#ifndef SPACESIM_SIMOBJECT_H
#define SPACESIM_SIMOBJECT_H

#include <glm/glm.hpp>
#include "circle.h"
#include <ostream>
#include <string>
using glm::vec2, std::string;

// Base class
class SimObject {
protected:
  vec2 pos;
  vec2 vel;
  float mass;
  float radius;

public:
  SimObject(float x, float y, float m, float r)
    : pos(x, y), vel(0.0f), mass(m), radius(r) {
  }

  virtual ~SimObject() = default;

  virtual void draw(unsigned int shader, Circle &renderer) = 0;

  virtual string getTypeName() const = 0;

  friend std::ostream &operator<<(std::ostream &os, const SimObject &obj) {
    os << obj.getTypeName() << " [Mass: " << obj.mass << ", Rad: " << obj.radius << "]";
    return os;
  }

  //update the velocity based on the force and time (physics calculations per frame
  void applyForce(vec2 force, float dt) {
    vec2 acceleration = force / mass;
    vel += acceleration * dt;
  }

  //update position using the already updated velocity
  void update(float dt) {
    pos += vel * dt;
  }

  // Getters for Physics & Rendering
  vec2 getPos() const { return pos; }
  vec2 getVel() const { return vel; }
  void setVel(vec2 v) { vel = v; }
  float getMass() const { return mass; }
  float getRadius() const { return radius; }
};

// A Star (Derrived Class)
class Star : public SimObject {
public:
  using SimObject::SimObject;

  string getTypeName() const override { return "Star"; }

  void draw(unsigned int shader, Circle &renderer) override {
    renderer.queueInstance(pos.x, pos.y, radius);
  }
};

// A Planet (Derrived Class)
class Planet : public SimObject {
public:
  using SimObject::SimObject;

  string getTypeName() const override { return "Planet"; }

  void draw(unsigned int shader, Circle &renderer) override {
    renderer.queueInstance(pos.x, pos.y, radius);
  }
};

#endif
