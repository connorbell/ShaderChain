#version 150
#pragma include "includes/hue.glsl"

uniform sampler2DRect _MainTexture;
uniform vec2 _Resolution;

uniform float shift;

in vec2 texCoordVarying;
out vec4 outputColor;

void main()
{
    vec4 c = texture(_MainTexture, (gl_FragCoord.xy));
    c.rgb = hueShift(c.rgb, shift);
    outputColor = c;
}
