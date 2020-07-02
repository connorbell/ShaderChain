/*
{
   "parameters" : [
      {
         "name" : "focalLength",
         "range" : {
            "x" : 0,
            "y" : 10
         },
         "show" : true,
         "type" : "float",
         "value" : 1.14130437374115
      },
      {
         "name" : "minDist",
         "range" : {
            "x" : 0,
            "y" : 0.1000000014901161
         },
         "show" : false,
         "type" : "float",
         "value" : 0.001000000047497451
      },
      {
         "name" : "maxDist",
         "range" : {
            "x" : 0,
            "y" : 15
         },
         "show" : true,
         "type" : "float",
         "value" : 12.30978298187256
      }
   ]
}
*/
#version 150
#pragma include "includes/hg_sdfs.glsl"

uniform sampler2D tex0;
uniform float _Time;
uniform vec2 _Resolution;
uniform vec3 _CamPos;
uniform vec3 _CamForward;
uniform vec3 _CamRight;
uniform vec3 _CamUp;

uniform float focalLength;

uniform float minDist;
uniform float maxDist;

uniform vec4 bgColor;

in vec2 uv;
out vec4 outputColor;

void pR(inout vec2 p, float a) {
	p = cos(a)*p + sin(a)*vec2(p.y, -p.x);
}

float map(in vec3 pos) {
	pos.z +=  cos(_Time) + smoothstep(1., 5., mod(_Time,6.28318)) * 20.;
	pos = mod(pos, vec3(1.))-vec3(.5);
    float dist = fBox(pos, vec3(0.05));
    return dist;
}

float march(in vec3 camPos, in vec3 rayDir) {

    float dist = minDist;

    for (int i = 0; i < 35; i++) {
        vec3 p = camPos + rayDir * dist;
        float res = map(p);
        if (res <= minDist) break;
        dist += res;
        if (res >= maxDist) break;
    }

    return dist;
}

vec3 calcNormal(in vec3 pos) {
    vec3 eps = vec3(0.001, 0.0, 0.0);
    return normalize(vec3(map(pos + eps) - map(pos - eps),
                     map(pos + eps.yxz) - map(pos - eps.yxz),
                     map(pos + eps.yzx) - map(pos - eps.yzx)));
}

vec4 render(in vec3 camPos, in vec3 rayDir) {

    float dist = march(camPos, rayDir);
    vec3 fPos = camPos + rayDir * dist;
    vec3 nor = calcNormal(fPos);
    vec3 col = vec3(0.);
    float fres = (1.+dot(rayDir, nor));
    col *= fres;
    col = mix(col, bgColor.rgb, clamp(dist/maxDist, 0.0, 1.0));
    return vec4(col, dist);
}
#define AA 1

void main()
{
    vec4 color = vec4(0.);
    for (int j = 0; j < AA; j++) {
        for (int k = 0; k < AA; k++)
        {
			float f = focalLength + sin(_Time);
            vec2 o = vec2(float(j), float(k)) / float(AA);
            vec2 uv_c = uv * 2. - 1. ;
            uv_c.x *= _Resolution.x/_Resolution.y;
            vec3 ray = normalize (vec3(1., 0., 0.) * uv_c.x +
                                  vec3(0., 1., 0.) * uv_c.y +
                                  vec3(0., 0., 1.) * f);

            color += vec4(render(vec3(0.), ray));
        }
    }

    color /= float(AA * AA);
    outputColor = color;
}
