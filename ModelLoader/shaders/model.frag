#version 330 core

in vec2 uv;
in vec3 normal;
in vec3 fragPos;

uniform sampler2D diffuse_texture1;
uniform sampler2D specular_texture1;
uniform sampler2D normal_texture1;

uniform vec3 lightDir;
uniform vec3 viewPos;
uniform vec3 lightColor;

out vec4 fragCol;

void main() {
	// Texture colours at our fragment
	vec3 diffuseTex = texture(diffuse_texture1, uv).rgb;
	vec3 specularTex = texture(specular_texture1, uv).rgb;

	float ambient = 0.2f;
	vec3 ambientCol = ambient * diffuseTex;

	vec3 norm = normalize(normal);  // In world space
	vec3 lightdir = normalize(-lightDir); // From fragment to light source

	float diffuse = max(dot(norm, lightdir), 0);
	vec3 diffuseCol = diffuse * diffuseTex;

	vec3 viewdir = normalize(viewPos - fragPos);
	vec3 reflected = reflect(-lightdir, norm);  // From light source to fragment
	float specular = max(pow(dot(reflected, viewdir), 32.0), 0);
	vec3 specularCol = specular * specularTex;

	fragCol = vec4(ambientCol, 1.0) + vec4(diffuseCol, 1.0) + vec4(specularCol, 1.0);
	fragCol *= vec4(lightColor, 1.0);
}