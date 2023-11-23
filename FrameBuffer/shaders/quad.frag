#version 330 core

in vec2 uv;

uniform sampler2D tex;
out vec4 fragCol;

const float offset = 1.0 / 300.0;

void main() {
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
    );

    float kernel[9] = float[](
        2,  2,  2,
        2, -15, 2,
        2,  2,  2
    );

    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++) {
        vec3 pixelRGB = vec3(texture(tex, uv.st + offsets[i]));
        col += pixelRGB * kernel[i];
    }

	fragCol = vec4(col, 1.0);
}