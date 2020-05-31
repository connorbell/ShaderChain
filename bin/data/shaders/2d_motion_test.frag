/*
{
   "parameters" : [

   ]
}
*/

#version 150
#pragma include "includes/noise.glsl"

uniform sampler2DRect _MainTexture;
uniform vec2 _Resolution;
uniform float _Time;

uniform float shift;

in vec2 texCoordVarying;
out vec4 outputColor;

float smin( float d1, float d2, float k ) {
    float h = clamp( 0.5 + 0.5*(d2-d1)/k, 0.0, 1.0 );
    return mix( d2, d1, h ) - k*h*(1.0-h);
}

float hash( float n )
{
    return fract(sin(n)*43758.5453123);
}

void main()
{
    vec2 uv = (gl_FragCoord.xy/_Resolution) * 2. - 1.;
    float dist = length(uv) - 0.125;
    const int count = 50;
    float time = _Time + sin(_Time*4.)*0.5;
    for (int i = 0; i < count; i++) {
        vec2 p = uv;

        float n = snoise(vec2(hash(float(i)*16.), -1.));
        float a = n * 3.14159 * 4.;

        float lengthDisplacement = sin(hash(float(i)*16.)*111.6+n*14.+ _Time*4.);
        lengthDisplacement = lengthDisplacement*(1.5+n) + 3.5;

        p = (p - 0.5) * lengthDisplacement + lengthDisplacement*0.5;
        p += vec2( cos(a), sin(a) );

        dist = smin(dist,length(p) - 0.05, 0.125);
    }


    float col = smoothstep(-0.015, 0.025, dist);

    outputColor = vec4(vec3(col), 1.);
}
