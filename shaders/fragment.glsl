#version 330 core

in vec3 pos;
out vec4 outColor;

uniform vec4 color;

void main() {
    outColor = color * (1.0f - pos.y);
}
