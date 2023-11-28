#version 330 core

in vec2 uv;
in vec3 normal;

uniform sampler2D diffuse_texture1;
uniform sampler2D specular_texture1;
uniform sampler2D normal_texture1;

out vec4 fragCol;

void main() {
	vec4 diffuseCol = texture(diffuse_texture1, uv);
	vec4 specularCol = texture(specular_texture1, uv);
	vec4 normalCol = texture(normal_texture1, uv);
	fragCol = normalCol;
}