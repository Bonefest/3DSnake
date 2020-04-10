#version 330 core

layout(location = 0) in vec3 vPos;

out vec3 pos;

uniform mat4 mvp;

void main() {
        gl_Position = mvp * vec4(vPos, 1.0);
        pos = vPos;
}
