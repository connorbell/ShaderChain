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
			 "y" : 15
		  },
		  "show" : true,
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
	   }
	]
}
*/
#version 150
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
	vec3 originalPos = pos + _CamPos - _CamForward*1.5;

	float scale = .55;
    vec3 spacesize = vec3(3.,4.,2.2);
    float distFromCam = length(pos)*0.4;

    // Divide the space into cells
    pos.xyz = mod(pos.xyz, spacesize) - spacesize*0.5;

    vec3 p = pos;
	vec2 mouse = vec2(0.7, 1.05);

    float cube = 1e20;
    float res = cube;
    vec3 displacement = vec3(-1., -.5, -2.)*0.75;

    for (int i = 0; i < 8; i++) {
        p.xyz = abs(p.xyz);

        float phase = _Time+float(i)*0.25+distFromCam*2.;
        vec2 dispAnim = vec2( sin(phase), cos(phase))*0.025;

        displacement.yz += dispAnim;

        p += displacement * scale;

        phase = _Time+float(i)*0.5 + distFromCam;
        pR(p.xz,-distFromCam+mouse.y+float(i)+sin(phase)*.05);
        pR(p.zy,distFromCam+mouse.x+float(i)+cos(phase)*.075);

		scale *= 0.6;

        float octa = fIcosahedron(p,scale);

        res = min(res,octa);
    }

    // Animate the pos of the icosahedron
    pR(originalPos.xy, .2+sin(_Time)*0.2);
    pR(originalPos.xz,-_Time);

    // Smooth min blend the fractal terrain with itself with a param around zero to give it a scaling effect
    res = smin(res, res, -0.025+ sin(-0.5+_Time-length(vec3(pos.x*0.5,pos.y*0.9,pos.z))*12.)*0.05);
    res = smin(res, fIcosahedron(originalPos,.2), 0.1);

    return res;
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
    vec3 col = nor * 0.5 + 0.5;
    float fres = (1.+dot(rayDir, nor));
    col *= fres;
    //col = pow(col, vec3(0.666));
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
            vec2 uv = (texCoordVarying + o / _Resolution) * 2. - 1. ;
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
