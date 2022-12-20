#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "utils.h"

class Vertex {
    private:
        float mass;
        bool fixed;
        glm::vec3 normal;
        glm::vec3 position;
        glm::vec3 velocity;
        glm::vec3 acceleration;

        friend class Cloth;
        friend class Triangle;
        friend class SpringDamper;

    public:
        Vertex(float m, glm::vec3 p, glm::vec3 v) {
            mass = m;
            fixed = false;
            normal = glm::vec3(0);
            position = p;
            velocity = v;
            acceleration = glm::vec3(0);
        }

        void fix() {fixed = true;}
        void unfix() {fixed = false;}
        void resetAcceleration() {acceleration = glm::vec3(0);}
        void addAcceleration(glm::vec3 a) {acceleration += a;}
        void addForce(glm::vec3 f) {acceleration += (f / mass);}

        void move(float dx) {
            if (!fixed) {
                velocity += (dx * acceleration);
                position += (dx * velocity);
            }
        }
};
#endif