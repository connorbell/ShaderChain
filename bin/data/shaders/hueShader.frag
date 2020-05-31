/*
{
    "wantsLastBuffer" : true,

   "parameters" : [
      {
         "name" : "shift",
         "range" : {
            "x" : 0,
            "y" : 6.28318
         },
         "show" : true,
         "type" : "float",
         "value" : 1.0
      }
   ]
}
*/

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
