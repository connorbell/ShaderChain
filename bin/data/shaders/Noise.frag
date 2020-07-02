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
         "type" : "float",
         "value" : 5.0
      },
      {
         "name" : "periods",
         "range" : {
            "x" : 0,
            "y" : 150
         },
         "show" : true,
         "type" : "float",
         "value" : 5.0
      },
      {
         "name" : "octaves",
         "range" : {
            "x" : 0,
            "y" : 15
         },
         "show" : true,
         "type" : "float",
         "value" : 10
      }
   ]
}
*/

#version 150
#pragma include "includes/hue.glsl"

uniform sampler2D _MainTexture;
uniform vec2 _Resolution;
uniform float edges;
uniform float periods;
uniform float octaves;

in vec2 uv;
out vec4 outputColor;

// 2D Random
float random (in vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise (in vec2 st, in float edges) {
    st *= edges;
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile, restricting the domain of the space
    float a = random(mod(i, edges));
    float b = random(mod(i + vec2(1.0, 0.0), edges));
    float c = random(mod(i + vec2(0.0, 1.0), edges));
    float d = random(mod(i + vec2(1.0, 1.0), edges));

    // Smooth Interpolation
    vec2 u = smoothstep(0.,1.,f);

    // Mix 4 coorners porcentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

float fbm(in vec2 st, in float edges) {
    float n = 0.;
    float scale = .55;

    for (int i = 0; i < floor(octaves); i += 1) {
        n += noise(st, edges) * scale;
        scale *= 0.5;
        st *= 2.;
    }
    return n;
}


void main()
{
    float n = fbm(uv*round(periods), round(edges));
    outputColor = vec4(vec3(n), 1.0);
}
