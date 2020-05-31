/*
{
   "parameters" : [
      {
         "name" : "edges",
         "range" : {
            "x" : 0,
            "y" : 50
         },
         "show" : true,
         "type" : 0,
         "value" : 5.0
      },
      {
         "name" : "periods",
         "range" : {
            "x" : 0,
            "y" : 150
         },
         "show" : true,
         "type" : 0,
         "value" : 5.0
      },
      {
         "name" : "octaves",
         "range" : {
            "x" : 0,
            "y" : 15
         },
         "show" : true,
         "type" : 0,
         "value" : 10
      }
   ]
}
*/

#version 150
#pragma include "includes/hue.glsl"

uniform sampler2DRect _MainTexture;
uniform vec2 _Resolution;
uniform float edges;
uniform float periods;
uniform float octaves;

in vec2 texCoordVarying;
out vec4 outputColor;

// 2D Random
vec2 random2(vec2 st){
  st = vec2( dot(st,vec2(127.1,311.7)),
             dot(st,vec2(269.5,183.3)) );
  return fract(sin(st)*43758.5453123);
}

vec2 opU(vec2 a, vec2 b) {
    return a.x < b.x ? a : b;
}

vec2 cellNoise(in vec2 st, vec2 last, int iteration) {

    if (iteration == floor(octaves)) {
        return last;
    }

    vec2 cell = floor(st);
    vec2 pos = fract(st);
    vec2 res = vec2(1e20, -1);

    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            vec2 neighbour = vec2(float(i), float(j));
            vec2 point = random2(cell + neighbour);
            res = opU(res, vec2(distance(neighbour + point, pos), point));
        }
    }

    if (iteration > 0) {
        res.x = mix(res.x, last.x, smoothstep(0.4, 0.6, last.y));
    }
    iteration++;

    return cellNoise(st * 2., res, iteration);
}

void main()
{
    float n = cellNoise(texCoordVarying * round(periods), vec2(1, -1), 0).x;
    outputColor = vec4(vec3(n), 1.0);
}
