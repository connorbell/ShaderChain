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

uniform sampler2DRect _MainTexture;
uniform sampler2DRect lastTex;
uniform sampler2DRect audioTex;
uniform sampler2DRect _NoiseTexture;

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
    vec2 uv_c_p = uv_c;
    uv_c_p.x *= _Resolution.x / _Resolution.y;
    float angleNoise = texture(_NoiseTexture, vec2((sin(a)*0.5+0.5)*400, (sin(-_Time*0.25 + length(uv_c_p)*0.5)*0.5+0.5)*126)).r;

    float audio = texture(audioTex, vec2(angleNoise*126, 0.)).r * clamp(length(uv_c)-0.125, 0., 1.)*4.;
    float l = length(uv_c)*0.5*(1.-audio*0.01);
    vec2 uv = vec2(cos(a), sin(a)) * l + 0.5;

    uv = (uv - 0.5) * scale + 0.5;

    vec4 lastColor = texture(lastTex, uv*_Resolution);

    color.rgb += lastColor.rgb * intensity;
    outputColor = clamp(vec4(color), vec4(0.0), vec4(1.0));
}
