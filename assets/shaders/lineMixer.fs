#version 410

uniform sampler2D samp;
uniform mat4 view;

in vec2 varTex;

out vec4 color;

void main() {
    vec3 c = texture(samp, varTex).xyz;
    vec3 ic = vec3(1.0, 1.0, 1.0) - c;

    float t = smoothstep(0.499, 0.501, varTex.x - (varTex.y * 0.1 - 0.02));
    color = vec4(mix(ic, c, t), 1.0);
}
