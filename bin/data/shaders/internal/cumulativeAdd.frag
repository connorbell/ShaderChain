#version 150

uniform sampler2DRect _CumulativeTexture;
uniform sampler2DRect _IncomingTexture;

uniform float factor;

in vec2 texCoordVarying;
out vec4 outputColor;

void main()
{
    vec4 c = texture(_CumulativeTexture, (gl_FragCoord.xy));
    c.rgb += texture(_IncomingTexture, (gl_FragCoord.xy)).rgb * factor;
    c.a = 1.0;
    outputColor = c;
}
