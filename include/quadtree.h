#ifndef SPACESIM_QUADTREE_H
#define SPACESIM_QUADTREE_H

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "SimObject.h"

/**
 * Barnes-Hut Quadtree Node
 * This class represents a single quadrant in space.
 * It recursively divides until each leaf contains only one body.
 */
class QuadNode {
public:
    // Boundary of this quadrant
    float xMin, xMax, yMin, yMax;

    // Center of mass and total mass of all bodies in this node
    glm::vec2 centerOfMass;
    float totalMass;

    // The single body if this is a leaf node
    SimObject* body;

    // Children (NW, NE, SW, SE)
    std::unique_ptr<QuadNode> children[4];
    bool isLeaf;

    QuadNode(float x1, float x2, float y1, float y2)
        : xMin(x1), xMax(x2), yMin(y1), yMax(y2),
          centerOfMass(0.0f), totalMass(0.0f),
          body(nullptr), isLeaf(true) {
        for (int i = 0; i < 4; i++) children[i] = nullptr;
    }

    /**
     * Inserts a body into the Quadtree, updating center of mass recursively.
     */
    void insert(SimObject* b) {
        if (totalMass == 0) {
            // First body in this empty node
            body = b;
            centerOfMass = b->pos;
            totalMass = b->mass;
            return;
        }

        if (isLeaf) {
            // This was a leaf with one body. Subdivide and move the old body.
            isLeaf = false;
            SimObject* oldBody = body;
            body = nullptr;
            subdivide();
            insertIntoChild(oldBody);
        }

        // Add the new body to the appropriate child and update center of mass
        insertIntoChild(b);
        
        // Update total center of mass: (m1*p1 + m2*p2) / (m1 + m2)
        centerOfMass = (centerOfMass * totalMass + b->pos * b->mass) / (totalMass + b->mass);
        totalMass += b->mass;
    }

    /**
     * Calculates the gravitational acceleration exerted by the masses in this tree
     * on a target body using the Barnes-Hut approximation.
     */
    glm::vec2 calculateForce(SimObject* target, float G, float softening, float theta) {
        if (body == target) return glm::vec2(0.0f);

        glm::vec2 diff = centerOfMass - target->pos;
        float d2 = glm::dot(diff, diff) + (softening * softening);
        float d = std::sqrt(d2);

        // s = width of the quadrant
        float s = xMax - xMin;

        // Barnes-Hut Criterion: if s/d < theta, treat the node as a single point mass
        if (isLeaf || (s / d) < theta) {
            if (totalMass == 0) return glm::vec2(0.0f);
            float accelMag = (G * totalMass) / d2;
            return (accelMag / d) * diff;
        }

        // Otherwise, recurse into children
        glm::vec2 accel(0.0f);
        for (int i = 0; i < 4; i++) {
            if (children[i]) {
                accel += children[i]->calculateForce(target, G, softening, theta);
            }
        }
        return accel;
    }

private:
    void subdivide() {
        float xMid = (xMin + xMax) / 2.0f;
        float yMid = (yMin + yMax) / 2.0f;

        children[0] = std::make_unique<QuadNode>(xMin, xMid, yMid, yMax); // NW
        children[1] = std::make_unique<QuadNode>(xMid, xMax, yMid, yMax); // NE
        children[2] = std::make_unique<QuadNode>(xMin, xMid, yMin, yMid); // SW
        children[3] = std::make_unique<QuadNode>(xMid, xMax, yMin, yMid); // SE
    }

    void insertIntoChild(SimObject* b) {
        float xMid = (xMin + xMax) / 2.0f;
        float yMid = (yMin + yMax) / 2.0f;

        if (b->pos.y >= yMid) {
            if (b->pos.x <= xMid) children[0]->insert(b);
            else children[1]->insert(b);
        } else {
            if (b->pos.x <= xMid) children[2]->insert(b);
            else children[3]->insert(b);
        }
    }
};

#endif
