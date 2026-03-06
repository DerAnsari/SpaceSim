#ifndef SPACESIM_CIRCLE_H
#define SPACESIM_CIRCLE_H

#include <glad/gl.h>
#include <glm/glm.hpp>
#include <vector>
#include <cmath>

class Circle {
public:
  // Constructor: radius and segments (more segments = smoother circle)
  Circle(float radius = 0.5f, int segments = 100)
    : radius(radius), segments(segments), VAO(0), VBO(0), EBO(0), indexCount(0) {
    generateCircle();
  }

  // Destructor: clean up GPU memory
  ~Circle() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
  }

  // Draw the circle
  void draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
  }

private:
  float radius;
  int segments;
  unsigned int VAO, VBO, EBO;
  unsigned int indexCount;

  void generateCircle() {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    // Center point
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);

    // Generate vertices around the circle
    for (int i = 0; i <= segments; i++) {
      float angle = (2.0f * 3.14159265359f * i) / segments;
      float x = radius * cos(angle);
      float y = radius * sin(angle);

      vertices.push_back(x);
      vertices.push_back(y);
      vertices.push_back(0.0f);
    }

    // Generate indices (connect center to each edge point)
    for (int i = 1; i <= segments; i++) {
      indices.push_back(0); // center
      indices.push_back(i); // current edge point
      indices.push_back(i + 1); // next edge point
    }

    indexCount = indices.size();

    // Set up OpenGL buffers
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }
};

#endif
