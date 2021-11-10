#version 410

uniform sampler2DMS samp;
uniform float alpha;
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
    color = vec4(textureMultisample(), alpha);
}