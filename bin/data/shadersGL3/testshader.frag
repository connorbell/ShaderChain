/*
{
"wantsCamera" : true,
"parameters" : [
	   {
		  "midi" : 1,
		  "name" : "focalLength",
		  "range" : {
			 "x" : 0,
			 "y" : 10
		  },
		  "show" : true,
		  "type" : 0,
		  "value" : 2.5
	   },
	   {
		  "name" : "minDist",
		  "range" : {
			 "x" : 0,
			 "y" : 0.1000000014901161
		  },
		  "show" : false,
		  "type" : 0,
		  "value" : 0.001000000047497451
	   },
	   {
		  "name" : "maxDist",
		  "range" : {
			 "x" : 0,
			 "y" : 5
		  },
		  "show" : false,
		  "type" : 0,
		  "value" : 2.5
	   },
	   {
		  "name" : "bgColor",
		  "range" : {
			 "x" : 0,
			 "y" : 1
		  },
		  "show" : true,
		  "type" : 1,
		  "value" : {
			 "x" : 0,
			 "y" : 0,
			 "z" : 0
		  }
	   },
	   {
		  "name" : "noiseX",
		  "range" : {
			 "x" : -10,
			 "y" : 10
		  },
		  "show" : true,
		  "type" : 0,
		  "value" : 0
	   },
	   {
		  "name" : "noiseY",
		  "range" : {
			 "x" : -10,
			 "y" : 10
		  },
		  "show" : true,
		  "type" : 0,
		  "value" : 0
	   },
	   {
		  "name" : "noiseTex",
		  "show" : true,
		  "type" : 2,
		  "filePath" : "textures/noise_loop.png",
		  "textureIndex" : 1
	   }
	]
}
*/

#version 150
#pragma include "includes/noise.glsl"
#pragma include "includes/hg_sdfs.glsl"

uniform sampler2DRect tex0;
uniform float _Time;
uniform vec2 _Resolution;
uniform vec3 _CamPos;
uniform vec3 _CamForward;
uniform vec3 _CamRight;
uniform vec3 _CamUp;

uniform float focalLength;

uniform float minDist;
uniform float maxDist;
uniform float noiseX;
uniform float noiseY;

uniform sampler2DRect noiseTex;
uniform vec2 noiseTex_res;

uniform vec3 bgColor;

in vec2 texCoordVarying;
out vec4 outputColor;

void pR(inout vec2 p, float a) {
	p = cos(a)*p + sin(a)*vec2(p.y, -p.x);
}
float smin( float d1, float d2, float k ) {
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h);
}

float map(in vec3 pos) {
	vec2 noiseUv = mod(abs(pos.xz*0.5+vec2(noiseX, noiseY)+_Time/(3.14159*4.)) * noiseTex_res, noiseTex_res);
	float n = texture(noiseTex, noiseUv).r*4.;
	n = pow(noise(vec2(n + cos(_Time + pos.z*3.14), sin(_Time*0.25 + pos.z*3.14 + n))),2.)*0.15;
    float dist = fPlane(pos, vec3(0., 1., 0.), .25-n);
    vec2 idx = (abs(pos.xz));

    pos += vec3(-0.5,-0.,-.6);
	pR(pos.xz,0.25);

    pos.y += cos((length(idx)*5.5) + _Time)*0.01;

	dist = smin(dist, fBox(pos, vec3(0.14 )), 0.1);
	return dist;
}

float march(in vec3 camPos, in vec3 rayDir) {

    float dist = minDist;

    for (int i = 0; i < 85; i++) {
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
    vec3 col = nor.grb * 0.5 + 0.5;
    float fres = (1.+dot(rayDir, nor));
    col *= fres;
    col = pow(col, vec3(0.666));
    col = mix(col, bgColor, clamp(dist/maxDist, 0.0, 1.0));
    return vec4(col, dist);
}
#define AA 1

void main()
{
    vec4 color = vec4(0.);
    for (int j = 0; j < AA; j++) {
        for (int k = 0; k < AA; k++)
        {
            vec2 o = vec2(float(j), float(k)) / float(AA);
            vec2 uv = (texCoordVarying + o/_Resolution) * 2. - 1. ;
            uv.x *= _Resolution.x/_Resolution.y;
            vec3 ray = normalize ((_CamRight) * uv.x +
                                  (_CamUp) * uv.y +
                                  (_CamForward) * focalLength);

            color += vec4(render(_CamPos, ray));
        }
    }

    color /= float(AA * AA);
    outputColor = color;
}
