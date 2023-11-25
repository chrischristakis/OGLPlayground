#version 330 core

in vec3 texCoords;

uniform samplerCube cube;

out vec4 fragCol;

void main() {
	fragCol = texture(cube, texCoords);
}