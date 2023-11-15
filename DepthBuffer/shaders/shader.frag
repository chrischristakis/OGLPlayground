#version 330 core

in vec2 uv;

uniform sampler2D tex;

uniform float near;
uniform float far;

out vec4 fragCol;

void main() {
	float zNdc = gl_FragCoord.z * 2.0 - 1.0;
	float linearDepth = (2.0 * far * near) / (zNdc * (far - near) - (far + near));
	float normalized = (linearDepth + near) / (near - far);
	fragCol = vec4(vec3(normalized), 1.0);
}