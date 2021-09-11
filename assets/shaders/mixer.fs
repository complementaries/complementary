#version 410

uniform sampler2D samp;
uniform vec2 center;
uniform float radius;
uniform mat4 view;
uniform mat4 texToPos;

in vec2 varTex;

out vec4 color;

void main() {
    vec3 c = texture(samp, varTex).xyz;
    vec3 ic = vec3(1.0, 1.0, 1.0) - c;

    vec2 pos = (texToPos * vec4(varTex, 0.0, 1.0)).xy;

    color = vec4(mix(ic, c, float(length(pos - center) < radius)), 1.0);
}