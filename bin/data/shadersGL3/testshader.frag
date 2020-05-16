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
			 "y" : 40
		  },
		  "show" : true,
		  "type" : 0,
		  "value" : 10.
	   },
	   {
		  "name" : "bgColor",
		  "show" : true,
		  "type" : 3,
		  "value" : {
			 "r" : 0,
			 "g" : 1,
			 "b" : 0,
 			 "a" : 0
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
#pragma include "includes/hue.glsl"

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

uniform vec4 bgColor;

in vec2 texCoordVarying;
out vec4 outputColor;

void pR(inout vec2 p, float a) {
	p = cos(a)*p + sin(a)*vec2(p.y, -p.x);
}
float smin( float d1, float d2, float k ) {
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h);
}

float map(in vec2 pos) {
	vec2 noiseUv = mod(abs((pos*0.5+vec2(noiseX, noiseY))) * noiseTex_res, noiseTex_res);
	float n = texture(noiseTex, noiseUv).r;
	n += cos(n*25.+ _Time)*0.05;
	n*=0.4;
	n = min(0.35, n);
	n = pow(n, 1.5);
    float dist = -0.0 - n;
	return dist;
}

float map2(in vec3 pos) {
	pos.y += 0.1;
	pos.x += 0.15;
	//pos.z += 0.5;
	//pos = mod(pos - .5, 1.) - .5;
    float s = 0.065;
	pR(pos.xz, _Time*0.125);
//	pos.y += sin(_Time + pos.y*10.)*0.00025;

//	pos.y = mod(pos.y - s*0.5, s) - s*0.5;
	float b = fOctahedron(pos,0.125);
	b = max(b, -(length(pos)-0.145));
	return b;
}
float march(in vec3 camPos, in vec3 rayDir) {

    float dist = minDist;

    for (int i = 0; i < 45; i++) {
        vec3 p = camPos + rayDir * dist;
        float res = map2(p);
        if (res <= minDist) break;
        dist += res;
        if (dist >= maxDist) break;
    }

    return dist;
}

float terrainMarch(in vec3 camPos, in vec3 rayDir) {
	float dt = 0.0025;
    float dist = minDist;
	float lastY = 0.;
	float lastHeight = 0.;

    for (int i = 0; i < 550; i++) {
        vec3 p = camPos + rayDir * dist;
        float res = map(p.xz);
        if (p.y < res) {
			return dist - dt + dt*(lastHeight-lastY)/(p.y-lastY-res+lastHeight);
		}
        dist += dt;
        if (res >= maxDist) return -1;
		lastY = p.y;
		lastHeight = res;
    }

    return 100000;
}

vec3 calcNormal2(in vec3 pos) {
    vec3 eps = vec3(0.001, 0.0, 0.0);
    return normalize(vec3(map2(pos + eps) - map2(pos - eps),
                     map2(pos + eps.yxz) - map2(pos - eps.yxz),
                     map2(pos + eps.yzx) - map2(pos - eps.yzx)));
}

vec3 calcNormal(in vec3 p) {
    float eps = 0.001;

	return normalize( vec3( map(vec2(p.x-eps,p.z)) - map(vec2(p.x+eps,p.z)),
                            2.0f*eps,
                            map(vec2(p.x,p.z-eps)) - map(vec2(p.x,p.z+eps)) ) );
}

vec4 render(in vec3 camPos, in vec3 rayDir) {

    float dist = terrainMarch(camPos, rayDir);
	float dist2 = march(camPos, rayDir);

	float h = clamp( 0.5 + 0.5*(dist2-dist)/0.1, 0.0, 1.0 );

	float mixDist = 0.;
	if (dist > 100) { mixDist = dist2; h = 0.; }

	else { mixDist = mix( dist2, dist, h ) - 0.1*h*(1.0-h); }

	vec3 fPos = camPos + rayDir * mixDist;
	vec3 nor = mix(calcNormal2(fPos), calcNormal(fPos), h);

    vec3 col = nor.grb * 0.5 + 0.5;
    float fres = (1.+dot(rayDir, nor));
    col *= fres;
    col = pow(col, vec3(1.));
	//col = hueShift(col, (_Time / 25.) * 3.14159 * 2.);

	col = mix(col, bgColor.rgb, smoothstep(-0.05,-0.2, fPos.y));
    col = mix(col, bgColor.rgb, smoothstep(maxDist - .25, maxDist, mixDist));
	return vec4(clamp(col.rgb, vec3(0.), vec3(1.)), mixDist);
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
