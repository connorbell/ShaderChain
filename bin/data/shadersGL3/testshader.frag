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
    float dist = fPlane(pos, vec3(0., 1., 0.), .25-0.25*pow(fbm(vec3(pos.xz, sin(length(pos.xz*16.66)+_Time*2.)*0.01)), 2.));
    vec2 idx = floor(abs(pos.xz)*16.);

    pos += vec3(0.,0.15,-.75);

    pos.y += sin((length(idx)) + _Time*2.)*0.015;
    dist = smin(dist, fOctahedron(pos, 0.15), 0.15);

    return dist;
}

float march(in vec3 camPos, in vec3 rayDir) {

    float dist = minDist;

    for (int i = 0; i < 45; i++) {
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
    vec3 col = 1. - nor * 0.5 - 0.5;
    float fres = (.666+dot(rayDir, nor));
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
            vec2 uv = (texCoordVarying + o*0.0025) * 2. - 1. ;
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
