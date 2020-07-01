/*
{
   "parameters" : [
      {
         "name" : "periods",
         "range" : {
            "x" : 1,
            "y" : 15
         },
         "show" : true,
         "type" : "float",
         "value" : 2.82608699798584
      },
      {
         "name" : "hueAmp",
         "range" : {
            "x" : 1,
            "y" : 15
         },
         "show" : true,
         "type" : "float",
         "value" : 5.793478488922119
      }
   ]
}
*/

#version 150
#pragma include "includes/hue.glsl"

uniform sampler2DRect _MainTexture;
uniform vec2 _Resolution;
uniform float _Time;
uniform vec2 _Mouse;

uniform float periods;
uniform float hueAmp;

in vec2 texCoordVarying;
out vec4 outputColor;

// 2D Random by Patricio Gonzalez Vivo https://thebookofshaders.com/12/
vec2 random2(vec2 st){
  st = vec2( dot(st,vec2(127.1,311.7)),
             dot(st,vec2(269.5,183.3)) );
  return fract(sin(st)*43758.5453123);
}

// Cell Noise by Patricio Gonzalez Vivo https://thebookofshaders.com/12/
float cellNoise(in vec2 st) {

    vec2 cell = floor(st);
    vec2 pos = fract(st);
    float res = 1e20;

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            vec2 neighbour = vec2(float(i), float(j));
            vec2 point = random2(cell + neighbour);
            res = min(res, distance(neighbour + point, pos));
        }
    }

    res = min(res, distance(st, _Mouse*round(periods)));
    return res;
}

void main()
{
    float n = cellNoise(texCoordVarying * round(periods));
    vec3 col = hueShift(vec3(1.0, 0.25, 0.25), _Time + n*hueAmp);
    outputColor = vec4(col, 1.0);
}
