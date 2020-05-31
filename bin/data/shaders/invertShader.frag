#version 150

uniform sampler2DRect _MainTexture;

in vec2 texCoordVarying;
out vec4 outputColor;

void main()
{
    vec4 color = texture(_MainTexture, (gl_FragCoord.xy));
    color.rgb = 1.-color.rgb;
    outputColor = color;
}
