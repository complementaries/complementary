#version 410

uniform sampler2D samp;

in vec2 varTex;
in vec4 varColor;

out vec4 color;

const float smoothing = 1.0 / 12.0;

void main() {
    float dist = texture(samp, varTex).r;
    float alpha = smoothstep(0.5 - smoothing, 0.5 + smoothing, dist);
    color = vec4(varColor.xyz, varColor.w * alpha);
}
