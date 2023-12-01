#version 330 core

layout (location = 0) in vec3 inPos;
layout (location = 1) in vec3 inCol;
layout (location = 2) in vec4 model_row1;
layout (location = 3) in vec4 model_row2;
layout (location = 4) in vec4 model_row3;
layout (location = 5) in vec4 model_row4;

out vec3 color;


void main() {
	mat4 model = mat4(model_row1, model_row2, model_row3, model_row4);
	gl_Position = model * vec4(inPos, 1.0);
	color = inCol;
}