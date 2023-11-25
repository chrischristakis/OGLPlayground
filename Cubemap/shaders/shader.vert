#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNorm;

uniform mat4 model;
uniform mat4 mvp;

out vec3 WorldPosition;
out vec3 Normal;

void main() {
	Normal = mat3(transpose(inverse(model))) * inNorm;  // normal multiplied by Normal matrix
	WorldPosition = vec3(model * vec4(inPos, 1.0));
	gl_Position = mvp * vec4(inPos, 1.0);
}