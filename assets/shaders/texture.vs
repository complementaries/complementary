#version 410

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec4 color;

uniform mat4 view;

out vec2 varTex;
out vec4 varColor;

void main() {
    gl_Position = view * vec4(pos, 0.0, 1.0);
    varTex = tex;
    varColor = color;
}