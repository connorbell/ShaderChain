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

uniform sampler2D _MainTexture;
uniform vec2 _Resolution;

uniform float shift;

in vec2 texCoord;
out vec4 outputColor;

void main()
{
    vec4 c = texture(_MainTexture, texCoord);
    c.rgb = hueShift(c.rgb, shift);
    outputColor = c;
}
