#ifndef SPACESIM_CAMERA_H
#define SPACESIM_CAMERA_H

#include <glm/glm.hpp>
using glm::vec2;

class Camera2D {
public:
  vec2 position;
  float zoom;
  float moveSpeed;

  Camera2D(vec2 pos = vec2(0.0f, 0.0f), float z = 1.0f, float speed = 2.0f)
    : position(pos), zoom(z), moveSpeed(speed) {
  }

  void moveUp(float dt) {
    position.y += moveSpeed * dt / zoom;
  }

  void moveDown(float dt) {
    position.y -= moveSpeed * dt / zoom;
  }

  void moveLeft(float dt) {
    position.x -= moveSpeed * dt / zoom;
  }

  void moveRight(float dt) {
    position.x += moveSpeed * dt / zoom;
  }

  void adjustZoom(float offset) {
    zoom += offset * 0.1f * zoom;
    if (zoom < 0.01f) zoom = 0.01f;
  }
};

#endif
