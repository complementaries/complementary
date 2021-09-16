#version 410

uniform sampler2D samp;

in vec2 varTex;
in vec4 varColor;

out vec4 color;

const float smoothing = 1.0 / 32.0;

void main() {
    float distance = 1.0 - texture2D(samp, varTex).r;
    float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);
    color = vec4(varColor.xyz, varColor.w * alpha);
}