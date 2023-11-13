#version 330 core

in vec2 uv;
uniform sampler2D tex1;
uniform sampler2D tex2;
out vec4 fragCol;

void main() {
	fragCol = mix(texture(tex1, uv), texture(tex2, uv), 0.2);
}