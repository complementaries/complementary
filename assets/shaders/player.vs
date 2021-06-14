#version 410

layout(location = 0) in vec2 pos;

uniform mat4 view;

void main() {
    gl_Position = view * vec4(pos, 0.0, 1.0);
}
