#ifndef SPACESIM_CIRCLE_H
#define SPACESIM_CIRCLE_H

#include <glad/gl.h>
#include <vector>
#include <cmath>
#include <numbers>

class Circle {
    unsigned int VAO, VBO, EBO;
    unsigned int instanceVBO;
    int indexCount;
    std::vector<float> instanceData;

public:
    Circle(float radius = 1.0f, int segments = 32) {
        std::vector<float> vertices;
        std::vector<unsigned int> indices;

        vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);

        for (int i = 0; i <= segments; i++) {
            float angle = (2.0f * std::numbers::pi_v<float> * i) / segments;
            vertices.push_back(radius * std::cos(angle));
            vertices.push_back(radius * std::sin(angle));
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
        glGenBuffers(1, &instanceVBO);

        glBindVertexArray(VAO);
        
        // Setup geometry
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // Setup instance buffer (location 1: offset, location 2: scale)
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        // We will stream this data every frame
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glVertexAttribDivisor(1, 1);

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(2 * sizeof(float)));
        glVertexAttribDivisor(2, 1);

        glBindVertexArray(0);
    }

    ~Circle() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
        glDeleteBuffers(1, &instanceVBO);
    }

    // Delete copy constructor and assignment operator to avoid double-free
    Circle(const Circle&) = delete;
    Circle& operator=(const Circle&) = delete;

    void queueInstance(float x, float y, float scale) {
        instanceData.push_back(x);
        instanceData.push_back(y);
        instanceData.push_back(scale);
    }

    void flushInstanced(unsigned int shaderProgram) {
        if (instanceData.empty()) return;

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceData.size() * sizeof(float), instanceData.data(), GL_STREAM_DRAW);

        glDrawElementsInstanced(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr, instanceData.size() / 3);
        
        glBindVertexArray(0);
        instanceData.clear();
    }

    // Deprecated: old single-draw method for backward compatibility if needed, 
    // but we'll move to instanced for everything.
    void draw(unsigned int shaderProgram, float x, float y, float scale) {
        int locScale = glGetUniformLocation(shaderProgram, "scale");
        int locOffset = glGetUniformLocation(shaderProgram, "offset");

        glUniform1f(locScale, scale);
        glUniform2f(locOffset, x, y);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
    }
};
#endif
