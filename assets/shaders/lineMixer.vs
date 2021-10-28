#version 410

layout(location = 0) in vec2 pos;

out vec2 varTex;

void main() {
    gl_Position = vec4(pos, 0.0, 1.0);
    varTex = (pos + vec2(1.0, 1.0)) * 0.5;
}
