#version 410

layout(location = 0) in vec3 pos;
layout(location = 1) in vec4 color;

uniform mat4 view;

out vec4 varColor;

void main() {
    gl_Position = view * vec4(pos, 1.0);
    varColor = color;
}
