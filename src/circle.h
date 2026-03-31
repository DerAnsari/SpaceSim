#ifndef SPACESIM_CIRCLE_H
#define SPACESIM_CIRCLE_H

#include <glad/gl.h>
#include <vector>
#include <cmath>

class Circle {
    unsigned int VAO, VBO, EBO;
    int indexCount;

public:
    Circle(float radius = 1.0f, int segments = 32) {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);

        for (int i = 0; i <= segments; i++) {
            float angle = (2.0f * 3.14159f * i) / segments;
            vertices.push_back(radius * cos(angle));
            vertices.push_back(radius * sin(angle));
            vertices.push_back(0.0f);
        }

        for (int i = 1; i <= segments; i++) {
            indices.push_back(0);
            indices.push_back(i);
            indices.push_back(i % segments + 1);
        }

        indexCount = indices.size();

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    }

    // This is the key: tell the shader where to put this specific instance
    void draw(unsigned int shaderProgram, float x, float y, float scale) {
        int locScale = glGetUniformLocation(shaderProgram, "scale");
        int locOffset = glGetUniformLocation(shaderProgram, "offset");

        glUniform1f(locScale, scale);
        glUniform2f(locOffset, x, y);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    }
};
#endif
