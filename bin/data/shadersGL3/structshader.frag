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
uniform float r1;
uniform float r2;
uniform float startScale;
uniform float scaleFactor;
uniform float offsetScalar;

uniform float o1;
uniform float o2;
uniform float o3;


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
vec2 opU(vec2 a, vec2 b) {
	return a.x < b.x ? a : b;
}

vec2 map(in vec3 pos) {
//    float dist = fPlane(pos, vec3(0., 1., 0.), .25-0.25*pow(fbm(vec3(pos.xz, sin(length(pos.xz*16.66)+_Time*2.)*0.01)), 2.));
	vec2 dist = vec2(fPlane(pos, vec3(0., 1., 0.), 1.5), 0.);
	vec3 op = pos;

    //pos += vec3(0.,0.75,0.);

    float scale = startScale;

    vec2 structure = vec2(1e20, -1.);
	float lastQuadrant = 0.;

    for (int i = 0; i < 15; i++) {

		float n1 = pow(noise(vec2(i, o1)), 2.);
		float n2 = pow(noise(vec2(i*1.54123, o2)), 2.);
		float n3 = noise(vec2(i*0.33441, o3));

        //pR(pos.xz, r1 + n1 );
        //pR(pos.zy, r2 + n2);

		structure = opU(structure, vec2(fBox(pos,vec3(n1, n2, n3)), i));
		scale *= 0.9;
	}
	//dist = max(dist, -fBox(op, 5.*vec3(startScale, startScale*3., startScale)));

    dist = opU(dist, structure);

    return dist;
}
vec2 march(in vec3 camPos, in vec3 rayDir) {

    float dist = minDist;
	float id = -1.;

    for (int i = 0; i < 95; i++) {
        vec3 p = camPos + rayDir * dist;
        vec2 res = map(p);
        if (res.x <= minDist) break;
        dist += res.x;
		id = res.y;
        if (dist >= maxDist) break;
    }

    return vec2(dist, id);
}

vec3 calcNormal(in vec3 pos) {
    vec3 eps = vec3(0.0001, 0.0, 0.0);
    return normalize(vec3(map(pos + eps).x - map(pos - eps).x,
                     map(pos + eps.yxz).x - map(pos - eps.yxz).x,
                     map(pos + eps.yzx).x - map(pos - eps.yzx).x));
}

vec3 hsv2rgb(vec3 c)
{
    const vec4 K = vec4(1.0, 0.66667, 0.33333, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 render(in vec3 camPos, in vec3 rayDir) {

    vec2 dist = march(camPos, rayDir);
    vec3 fPos = camPos + rayDir * dist.x;
    vec3 nor = calcNormal(fPos);
	float r = abs(rand(vec2(0.,dist.y)));
    vec3 col = hsv2rgb(vec3(r, 0.8, 0.8));


	float fres = (.666+dot(rayDir, nor));
    //col *= fres;
    //col = pow(col, vec3(0.666));

    col = mix(col, bgColor, clamp(dist.x/maxDist, 0.0, 1.0));
    return col;
}
#define AA 2

void main()
{
    vec4 color = vec4(0.);
    for (int j = 0; j < AA; j++) {
        for (int k = 0; k < AA; k++)
        {
            vec2 o = vec2(float(j), float(k)) / float(AA);
            vec2 uv = (texCoordVarying + o*0.0025) * 2. - 1. ;
            uv.x *= _Resolution.x/_Resolution.y;
            vec3 ray = normalize ((_CamRight) * uv.x +
                                  (_CamUp) * uv.y +
                                  (_CamForward) * focalLength);

            color += vec4(render(_CamPos, ray), 1.0);
        }
    }

    color /= float(AA * AA);
    outputColor = color;
}
