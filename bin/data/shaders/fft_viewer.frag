/*
{
    "lastBufferTextureIndex": 1,
    "audioTextureIndex": 2
}
*/

#version 150

in vec2 texCoordVarying;
out vec4 outputColor;

uniform float _Time;
uniform sampler2DRect _MainTexture;
uniform sampler2DRect _LastTexture;
uniform sampler2DRect _AudioTexture;
uniform vec2 _Resolution;

void main() {
    vec2 uv = texCoordVarying;

    float samp = texture(_AudioTexture, vec2(texCoordVarying.x*256., 0.)).r;

    vec4 col = vec4(vec3(samp), 1.0);

    outputColor = col;
}
