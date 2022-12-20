#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_

#include "utils.h"
#include "iostream"
#include "Vertex.h"

class Triangle {
    private:
        Vertex* v1;
        Vertex* v2;
        Vertex* v3;
        float dragCoefficient;
        float fluidDensity;

    public:
        Triangle(Vertex* v_1, Vertex* v_2, Vertex* v_3) {
            v1 = v_1;
            v2 = v_2;
            v3 = v_3;
            dragCoefficient = 1.28f;
            fluidDensity = 1.225f;
            updateNormal();
        }
        
        void updateNormal() {
            glm::vec3 normal = glm::normalize(glm::cross(glm::vec3(v2->position - v1->position), glm::vec3(v3->position - v1->position)));
            v1->normal += normal;
            v2->normal += normal;
            v3->normal += normal;
        }

        void addWind(glm::vec3 velocityWind) {
            glm::vec3 edge1 = glm::vec3(v2->position - v1->position);
            glm::vec3 edge2 = glm::vec3(v3->position - v1->position);
            float area = glm::length(glm::cross(edge1, edge2)) / 2.0f;

            glm::vec3 normal = glm::vec3(0, 1, 0);
            if (area != 0) {
                normal = glm::normalize(glm::cross(edge1, edge2));
            }

            glm::vec3 velocityTriangle = (v1->velocity + v2->velocity + v3->velocity) / 3.0f;
            
            glm::vec3 vClose = velocityTriangle - velocityWind;
            float crossArea = 0;
            if (glm::length(vClose) != 0) {
                crossArea = area * glm::dot(vClose, normal) / glm::length(vClose);
            }

            glm::vec3 force = -fluidDensity * dragCoefficient * glm::dot(vClose, vClose) * crossArea * normal / (2.0f * 3.0f);  // Force on each vertex since also divided by 3

            v1->addForce(force);
            v2->addForce(force);
            v3->addForce(force);
        }
};
#endif