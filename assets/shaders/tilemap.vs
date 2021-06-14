#version 410

layout(location = 0) in vec2 pos;
layout(location = 1) in vec4 color;

uniform float xOffset;

out vec4 varColor;

void main() {
    gl_Position = vec4(pos + vec2(xOffset, 0.0), 0.0, 1.0);
    varColor = color;
}
