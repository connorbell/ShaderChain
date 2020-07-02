#version 150

uniform sampler2D _MainTexture;

in vec2 texCoord;
out vec4 outputColor;

void main()
{
    vec4 color = texture(_MainTexture, texCoord);
    color.rgb = 1.-color.rgb;
    outputColor = color;
}
