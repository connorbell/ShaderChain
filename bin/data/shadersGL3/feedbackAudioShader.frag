/*
{
    "lastBufferTextureIndex": 1,
    "audioTextureIndex": 2,
    "parameters" : [
       {
          "name" : "intensity",
          "range" : {
             "x" : 0,
             "y" : 1
          },
          "show" : true,
          "type" : 0,
          "value" : 0.75
       },
       {
          "name" : "scale",
          "range" : {
             "x" : 0,
             "y" : 2
          },
          "show" : true,
          "type" : 0,
          "value" : 1.0
       },
       {
          "name" : "hue",
          "range" : {
             "x" : 0,
             "y" : 5
          },
          "show" : true,
          "type" : 0,
          "value" : 0.1
       }
    ]
}
*/

#version 150
#pragma include "includes/hue.glsl"

uniform sampler2DRect _MainTexture;
uniform sampler2DRect _LastTexture;
uniform sampler2DRect _AudioTexture;
uniform float intensity;
uniform float scale;
uniform float hue;

uniform float _Time;
uniform vec2 _Resolution;

in vec2 texCoordVarying;
out vec4 outputColor;

void main()
{
    vec4 color = texture(_MainTexture, gl_FragCoord.xy);

    vec2 uv_c = (gl_FragCoord.xy / _Resolution) * 2.0 - 1.0;

    float a = atan(uv_c.y, uv_c.x);
    float audio = texture(_AudioTexture, vec2((sin(a)*.5+.5)*256, 0.)).r * min(1., length(uv_c));
    float l = length(uv_c)*0.5*(1.-audio*0.075);
    vec2 uv = vec2(cos(a), sin(a)) * l + 0.5;

    uv = (uv - 0.5) * scale + 0.5;

    vec4 lastColor = texture(_LastTexture, uv*_Resolution);
    color.r = 0.;
    color.rgb = hueShift(color.rgb, hue + l*hue + audio);

    color.rgb += lastColor.rgb * intensity + lastColor.rgb * (audio);
    outputColor = clamp(vec4(color), vec4(0.0), vec4(1.0));
}
