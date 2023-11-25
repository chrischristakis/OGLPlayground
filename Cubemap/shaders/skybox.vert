#version 330 core

layout (location = 0) in vec3 inPos;

uniform mat4 mvp;

out vec3 texCoords;

void main() {
	vec4 pos = mvp * vec4(inPos, 1.0);
	gl_Position = pos.xyww;
	texCoords = inPos;
}