#version 410

layout(location = 0) in vec2 pos;
layout(location = 1) in vec4 color;

uniform mat4 view;
uniform float zLayer;

out vec4 varColor;

void main() {
    gl_Position = view * vec4(pos, zLayer, 1.0);
    varColor = color;
}
