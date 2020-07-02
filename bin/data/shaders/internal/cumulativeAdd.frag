#version 150

uniform sampler2D _CumulativeTexture;
uniform sampler2D _IncomingTexture;

uniform float factor;

in vec2 texCoord;

out vec4 outputColor;

void main()
{
    vec4 c = texture(_CumulativeTexture, (texCoord));
    c.rgb += texture(_IncomingTexture, (texCoord)).rgb * factor;
    c.a = 1.0;
    outputColor = c;
}
