#version 410

layout(location = 0) in vec2 pos;
layout(location = 1) in vec4 color;

uniform mat4 view;
uniform mat4 model;

out vec4 varColor;

void main() {
    gl_Position = view * model * vec4(pos, -0.15, 1.0);
    varColor = color;
}
