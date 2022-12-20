#ifndef _CAMERA_H_
#pragma once

#include "utils.h"
#include "glm/gtx/euler_angles.hpp"

class Camera {
    private:
        // Perspective controls
        float fov;			// Field of View Angle (degrees)
        float aspectRatio;	// Aspect Ratio
        float nearPlane;	// Near clipping plane distance
        float farPlane;		// Far clipping plane distance

        float distance;		// Distance of the camera eye position to the origin (meters)
        float rotation;		// Rotation of the camera eye position around the Y axis (degrees)
        float angle;		// Angle of the camera eye position over the XZ plane (degrees)

        // Computed data
        glm::mat4 viewProjectMatrix;

    public:
        Camera() {setDefault();}
        
        void update() {
            // Compute camera world matrix
            glm::mat4 world(1);
            world[3][2] = distance;
            world = glm::eulerAngleY(glm::radians(-rotation)) * glm::eulerAngleX(glm::radians(-angle)) * world;

            // Compute view matrix (inverse of world matrix)
            glm::mat4 view = glm::inverse(world);

            // Compute perspective projection matrix
            glm::mat4 project = glm::perspective(glm::radians(fov),aspectRatio, nearPlane, farPlane);

            // Compute final view-projection matrix
            viewProjectMatrix = project * view;
        }

        void setDefault() {
            fov = 60.0f;
            aspectRatio = 2.0f;
            nearPlane = 0.1f;
            farPlane = 100.0f;
            distance = 10.0f;
            rotation = 0.0f;
            angle = 25.0f;
        }

        void setAspectRatio(float ar) {aspectRatio = ar;}
        void setDistance(float d) {distance = d;}
        void setRotation(float r) {rotation = r;}
        void setAngle(float a) {angle = a;}

        float getAspectRatio() {return aspectRatio;}
        float getDistance() {return distance;}
        float getRotation() {return rotation;}
        float getAngle() {return angle;}
        const glm::mat4 &GetViewProjectMatrix()	{return viewProjectMatrix;}
};
#endif