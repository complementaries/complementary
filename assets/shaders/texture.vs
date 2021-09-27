#version 410

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 tex;
layout(location = 2) in vec4 color;

out vec2 varTex;
out vec4 varColor;

void main() {
    gl_Position = vec4(pos, 0.0, 1.0);
    varTex = tex;
    varColor = color;
}