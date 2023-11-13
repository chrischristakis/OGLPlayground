#version 330 core

layout (location = 0) in vec2 inPos;

void main() {
	gl_Position = mvp * vec4(inPos, 1.0, 1.0);
}