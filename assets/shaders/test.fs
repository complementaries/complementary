#version 410

in float varColor;

out vec4 color;

void main() {
    color = vec4(1.0, 0.0, 0.0, 1.0) * varColor;
}
