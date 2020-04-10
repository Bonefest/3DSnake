#version 330 core

in vec3 pos;
out vec4 outColor;

void main() {
    outColor = vec4(1.0f, 0.7f, -pos.y * 2.0f, 1.0f);
}
