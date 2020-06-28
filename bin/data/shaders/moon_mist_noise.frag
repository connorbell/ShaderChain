/*
{
"parameters" : [
{
   "name" : "gradientTex",
   "show" : true,
   "type" : "texture",
   "filePath" : "textures/gradient.png",
   "textureIndex" : 2
},
{
   "name" : "freq",
   "range" : {
      "x" : 0,
      "y" : 50
   },
   "show" : true,
   "type" : "float",
   "value" : 5.0
}
]
}
*/
#version 150
#pragma include "includes/noise.glsl"

uniform sampler2DRect _MainTexture;
uniform vec2 _Resolution;
uniform float _Time;

uniform sampler2DRect gradientTex;
uniform vec2 gradientTex_res;

uniform float freq;

in vec2 texCoordVarying;
out vec4 outputColor;

vec3 curl(vec3 pos) {
    vec3 eps = vec3(1., 0., 0.);
    vec3 res = vec3(0.);

    float n1 = snoise(pos + eps.yxy);
    float n2 = snoise(pos - eps.yxy);
    float a = (n1 - n2) / (2 * eps.x);

    n1 = snoise(pos + eps.yyx);
    n2 = snoise(pos - eps.yyx);
    float b = (n1 - n2) / (2 * eps.x);

    res.x = a - b;

    n1 = snoise(pos + eps.yyx);
    n2 = snoise(pos - eps.yyx);
    a = (n1 - n2) / (2 * eps.x);

    n1 = snoise(pos + eps.xyy);
    n2 = snoise(pos - eps.xyy);
    b = (n1 - n2) / (2 * eps.x);

    res.x = a - b;

    n1 = snoise(pos + eps.xyy);
    n2 = snoise(pos - eps.xyy);
    a = (n1 - n2) / (2 * eps.x);

    n1 = snoise(pos + eps.yxy);
    n2 = snoise(pos - eps.yxy);
    b = (n1 - n2) / (2 * eps.x);

    res.z = a - b;

    return res;
}


void main()
{
    vec2 uv = texCoordVarying;
    uv.x *= _Resolution.x / _Resolution.y;
    vec3 p = vec3(uv, _Time*0.05 );
    vec3 c = curl(p*freq);
    c = texture(gradientTex, vec2(mod(length(c)*500. + _Time*500*.5, 500.), 0.)).rgb;
    outputColor = vec4(c, 1.0);
}
