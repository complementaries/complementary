#version 410

uniform sampler2DMS samp;
uniform mat4 view;
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

    float t = smoothstep(0.499, 0.501, varTex.x - (varTex.y * 0.1 - 0.02));
    color = vec4(mix(ic, c, t), 1.0);
}
