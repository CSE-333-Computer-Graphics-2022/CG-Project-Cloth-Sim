#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

// Uniform variables
uniform mat4 viewProj;
uniform mat4 model;

// The default output, gl_Position, should be assigned something. 
out vec3 fragNormal;


void main()
{
    gl_Position = viewProj * model * vec4(position, 1.0);

    // for shading
	fragNormal = vec3(model * vec4(normal, 0));
}