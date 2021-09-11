#version 410

uniform sampler2D samp;
uniform float alpha;

in vec2 varTex;

out vec4 color;

void main() {
    color = vec4(texture(samp, varTex).xyz, alpha);
}