#ifndef _SPRING_DAMPER_
#define _SPRING_DAMPER_

#include "utils.h"
#include "iostream"
#include "Vertex.h"

class SpringDamper {
    private:
        float ks;   // Spring Stiffness Coefficient
        float kd;   // Damping Coefficient
        Vertex* v1;
        Vertex* v2;
        float resistantLength;

    public:
        SpringDamper(Vertex* v_1, Vertex* v_2, float rl) {
            ks = 2000;
            kd = 12;
            v1 = v_1;
            v2 = v_2;
            resistantLength = rl;
        }
        
        void updateAcceleration() {
            float currentLength = glm::length(glm::vec3(v2->position - v1->position));
            float dx = currentLength - resistantLength;
            glm::vec3 direction = glm::vec3(0, 1, 0);
            if (currentLength != 0) {
                direction = glm::normalize(v2->position - v1->position);
            }

            if (currentLength > 1.2 * resistantLength) {
                glm::vec3 center = (v1->position + v2->position) / 2.0f;
                if (!v1->fixed) {
                    v1->position = center - 0.6f * resistantLength * direction;
                }
                if (!v2->fixed) {
                    v2->position = center + 0.6f * resistantLength * direction;
                }
            }
            else if (currentLength < 0.5 * resistantLength) {
                glm::vec3 center = (v2->position + v1->position) / 2.0f;
                if (!v1->fixed) {
                    v1->position = center - resistantLength * direction / 4.0f;
                }
                if (!v2->fixed) {
                    v2->position = center + resistantLength * direction / 4.0f;
                } 
            }

            glm::vec3 vClose = glm::dot(v2->velocity - v1->velocity, direction) * direction;
            glm::vec3 force = ks * dx * direction + kd * vClose;

            v1->addForce(force);
            v2->addForce(-force);
        }
};
#endif