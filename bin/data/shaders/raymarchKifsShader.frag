/*
{
"parameters" : [
	   {
		  "midi" : 1,
		  "name" : "focalLength",
		  "range" : {
			 "x" : 0,
			 "y" : 10
		  },
		  "show" : true,
		  "type" : "float",
		  "value" : 2.5
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
		  "value" : 2.5
	   },
	   {
		  "name" : "bgColor",
		  "show" : true,
		  "type" : "color",
		  "value" : {
			 "r" : 0,
			 "g" : 1,
			 "b" : 0,
 			 "a" : 0
		  }
	   },
   	   {
   		  "name" : "startScale",
   		  "range" : {
   			 "x" : 0,
   			 "y" : 2
   		  },
   		  "show" : true,
   		  "type" : "float",
   		  "value" : 0.75
   	   },
	   {
		  "name" : "scaleFactor",
		  "range" : {
			 "x" : 0,
			 "y" : 1
		  },
		  "type" : "float",
		  "show" : true,
		  "value" : 0.575
	   },
	   {
		  "name" : "offset",
		  "range" : {
			 "x" : 0,
			 "y" : 4
		  },
		  "show" : true,
		  "type" : "vec3",
		  "value" : {
			 "x" : 0.75,
			 "y" : 1,
			 "z" : 2
		  }
	   },
	   {
		  "name" : "spaceSize",
		  "range" : {
			 "x" : 0,
			 "y" : 5
		  },
		  "show" : true,
		  "type" : "vec3",
		  "value" : {
			 "x" : 3,
			 "y" : 3,
			 "z" : 3
		  }
	   },
	   {
		  "name" : "distScalar",
		  "range" : {
			 "x" : 0,
			 "y" : 2
		  },
		  "show" : true,
		  "type" : "float",
		  "value" : 0.5
	   },
	   {
		  "name" : "rotationPhaseX",
		  "range" : {
			 "x" : 0,
			 "y" : 7
		  },
		  "show" : true,
		  "type" : "float",
		  "value" : 2.5
	   },
	   {
		  "name" : "rotationPhaseY",
		  "range" : {
			 "x" : 0,
			 "y" : 7
		  },
		  "show" : true,
		  "type" : "float",
		  "value" : 2.5
	   },
	   {
		  "name" : "animationAmpX",
		  "range" : {
			 "x" : 0,
			 "y" : 0.5
		  },
		  "show" : true,
		  "type" : "float",
		  "value" : 0.05
	   },
	   {
		  "name" : "animationAmpY",
		  "range" : {
			 "x" : 0,
			 "y" : 0.5
		  },
		  "show" : true,
		  "type" : "float",
		  "value" : 0.015
	   },
	   {
		  "type" : "camera",
		  "value" : {
			 "pos" : {
			 	"x" : 1.0,
				"y" : 2.0,
				"z" : 1.0
			 },
			 "rot" : {
			 	"x" : 0.0,
				"y" : 0.0,
				"z" : 0.0
			 }
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

uniform float startScale;
uniform float scaleFactor;
uniform float distScalar;
uniform float rotationPhaseX;
uniform float rotationPhaseY;
uniform float animationAmpX;
uniform float animationAmpY;

uniform vec3 offset;
uniform vec3 spaceSize;


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

float map(in vec3 pos) {
	vec3 originalPos = pos + _CamPos - _CamForward*1.5;

	float scale = startScale;
    vec3 spacesize = spaceSize;
    float distFromCam = length(pos)*distScalar;

    // Divide the space into cells
    pos.xyz = mod(pos.xyz, spacesize) - spacesize*0.5;

    vec3 p = pos;

    float res = 1e20;

    for (int i = 0; i < 8; i++) {
        p.xyz = abs(p.xyz);

        float phase = _Time+float(i)*0.25+distFromCam*2.;
		vec2 anim = vec2(cos(phase), sin(phase));
		vec3 offs = offset + vec3(0., anim*0.025);

        p -= offs * scale;

        pR(p.xz,-distFromCam+rotationPhaseY+anim.x*animationAmpY);
        pR(p.zy,distFromCam+rotationPhaseX+anim.y*animationAmpX);

        float octa = fOctahedron(p, scale);

        res = min(res,octa);
		scale *= scaleFactor;
    }

    // Animate the pos of the icosahedron
    pR(originalPos.xy, .2+sin(_Time)*0.2);
    pR(originalPos.xz,-_Time);

    // Smooth min blend the fractal terrain with itself with a param around zero to give it a scaling effect
    res = smin(res, res, -0.025+ sin(-0.5+_Time-length(vec3(pos.x*0.5,pos.y*0.9,pos.z))*12.)*0.05);
    res = smin(res, fIcosahedron(originalPos,.25), 0.2);

    return res;
}

float march(in vec3 camPos, in vec3 rayDir) {

    float dist = minDist;

    for (int i = 0; i < 65; i++) {
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
    col = pow(col, vec3(0.666));
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
            vec2 o = vec2(float(j), float(k)) / float(AA);
            vec2 uv = (texCoordVarying + o / _Resolution) * 2. - 1. ;
            uv.x *= _Resolution.x/_Resolution.y;
            vec3 ray = normalize (_CamRight * uv.x +
                                  _CamUp * uv.y +
                                  _CamForward * focalLength);

            color += vec4(render(_CamPos, ray));
        }
    }

    color /= float(AA * AA);
    outputColor = vec4(color.rgb, 1.);
}
