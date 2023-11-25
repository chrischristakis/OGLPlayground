#version 330 core

in vec3 WorldPosition;
in vec3 Normal;

uniform samplerCube skybox;
uniform vec3 cameraPos;

out vec4 fragCol;

void main() {
	vec3 Incident = vec3(WorldPosition - cameraPos);
	vec3 Reflected = reflect(Incident, normalize(Normal));
	fragCol = texture(skybox, Reflected);
}