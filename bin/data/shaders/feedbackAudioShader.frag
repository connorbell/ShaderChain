/*
{
    "parameters" : [
       {
          "name" : "intensity",
          "range" : {
             "x" : 0,
             "y" : 1
          },
          "show" : true,
          "type" : "float",
          "value" : 0.75
       },
       {
          "name" : "scale",
          "range" : {
             "x" : 0,
             "y" : 2
          },
          "show" : true,
          "type" : "float",
          "value" : 1.0
       },
          {
          "name" : "lastColMult",
		  "value" : {
             "x" : 0,
             "y" : 1,
		     "z" : 1
          },
          "show" : true,
          "type" : "color"
       },
       {
          "name" : "hue",
          "range" : {
             "x" : 0,
             "y" : 5
          },
          "show" : true,
          "type" : "float",
          "value" : 0.1
       },
       {
		  "name" : "_NoiseTexture",
		  "show" : true,
		  "type" : "texture",
		  "filePath" : "textures/noise_loop.png",
		  "textureIndex" : 3
	   },
       {
          "name" : "audioTex",
          "show" : true,
          "type" : "texture",
          "textype" : "Audio",
          "textureIndex" : 3
       },
       {
          "name" : "lastTex",
          "show" : true,
          "type" : "texture",
          "textype" : "Last",
          "textureIndex" : 2
       }
    ]
}
*/

#version 150
#pragma include "includes/hue.glsl"

uniform sampler2D _MainTexture;
uniform sampler2D lastTex;
uniform sampler2D audioTex;

uniform sampler2D _NoiseTexture;

uniform float intensity;
uniform float scale;
uniform float hue;

uniform float _Time;
uniform vec2 _Resolution;

uniform vec4 lastColMult;

in vec2 texCoord;

out vec4 outputColor;

void main()
{
    vec4 color = texture(_MainTexture, texCoord);

    vec2 texCoord_c = (gl_FragCoord.xy / _Resolution) * 2.0 - 1.0;
    float a = atan(texCoord_c.y, texCoord_c.x);
    vec2 texCoord_c_p = texCoord_c;
    texCoord_c_p.x *= _Resolution.x / _Resolution.y;
    float angleNoise = texture(_NoiseTexture, vec2((sin(1.57+a*2)*0.5+0.5)*400, (sin(-_Time*10.5 + length(texCoord_c_p)*0.25)*0.5+0.5)*126)).r;

    float audio = texture(audioTex, vec2(angleNoise, 0.)).r * clamp(length(texCoord_c)-0.125, 0., 1.)*4.;
    float l = length(texCoord_c)*0.5*(1.-audio*0.015);
    vec2 texCoord = vec2(cos(a), sin(a)) * l + 0.5;

    texCoord = (texCoord - 0.5) * scale + 0.5;

    vec4 lastColor = texture(lastTex, texCoord);
	float difference = min(1.,distance(lastColor.rgb, color.rgb)+.5);
	lastColor.rgb *= lastColMult.rgb;
	lastColor.rgb = hueShift(lastColor.rgb, hue);
    color.rgb += lastColor.rgb * min(10., (1. + audio) * intensity);
    outputColor = clamp(vec4(color), vec4(0.0), vec4(1.0));
}
