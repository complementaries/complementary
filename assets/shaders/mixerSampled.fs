#version 410

uniform sampler2DMS samp;
uniform vec2 center;
uniform float radius;
uniform mat4 view;
uniform mat4 texToPos;
uniform int texels;

in vec2 varTex;

out vec4 color;

vec3 textureMultisample() {
    ivec2 texSize = textureSize(samp);
    ivec2 texCoord = ivec2(varTex * texSize);
    vec3 color = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < texels; i++) {
        color += texelFetch(samp, texCoord, i).xyz;
    }
    color /= texels;
    return color;
}

void main() {
    vec3 c = textureMultisample();
    vec3 ic = vec3(1.0, 1.0, 1.0) - c;

    vec2 pos = (texToPos * vec4(varTex, 0.0, 1.0)).xy;

    color = vec4(mix(ic, c, float(length(pos - center) < radius)), 1.0);
}