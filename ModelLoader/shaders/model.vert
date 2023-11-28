#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inNorm;
layout (location = 2) in vec2 inUv;

uniform mat4 mvp;

out vec2 uv;
out vec3 normal;

void main() {
	gl_Position = mvp * vec4(inPos, 1.0);
	uv = inUv;
	normal = inNorm;
}