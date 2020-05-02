/*
{
    "lastBufferTextureIndex": 1,
    "lastaudioTextureIndex": 2,
    "parameters" : [
      {
         "name" : "edges",
         "range" : {
            "x" : 0,
            "y" : 50
         },
         "show" : true,
         "type" : 0,
         "value" : 5.0
      },
      {
         "name" : "midi1",
         "range" : {
            "x" : 0,
            "y" : 10
         },
         "show" : true,
         "type" : 0,
         "midi" : 1,
         "value" : 5.0
      },
      {
         "name" : "midi2",
         "range" : {
            "x" : 0,
            "y" : 1
         },
         "show" : true,
         "type" : 0,
         "midi" : 2,
         "value" : 0.1
      },
      {
         "name" : "midi3",
         "range" : {
            "x" : 0,
            "y" : 10
         },
         "show" : true,
         "type" : 0,
         "midi" : 3,
         "value" : 1.0
      },
      {
         "name" : "midi4",
         "range" : {
            "x" : 0,
            "y" : 1
         },
         "show" : true,
         "type" : 0,
         "midi" : 4,
         "value" : 1.0
      },
      {
         "name" : "midi5",
         "range" : {
            "x" : 0,
            "y" : 1
         },
         "show" : true,
         "type" : 0,
         "midi" : 5,
         "value" : 0.5
      },
      {
         "name" : "midi6",
         "range" : {
            "x" : 0,
            "y" : 1
         },
         "show" : true,
         "type" : 0,
         "midi" : 6,
         "value" : 0.5
      },
      {
         "name" : "midi7",
         "range" : {
            "x" : 0,
            "y" : 1
         },
         "show" : true,
         "type" : 0,
         "midi" : 7,
         "value" : 0.5
      },
      {
         "name" : "midi8",
         "range" : {
            "x" : 0,
            "y" : 1
         },
         "show" : true,
         "type" : 0,
         "midi" : 8,
         "value" : 0.5
      },
   ]
}
*/

#version 150

in vec2 texCoordVarying;
out vec4 outputColor;

uniform float _Time;
uniform sampler2DRect _MainTexture;
uniform sampler2DRect _LastTexture;
uniform sampler2DRect _AudioTexture;
uniform vec2 _Resolution;

uniform float midi1;
uniform float midi2;
uniform float midi3;
uniform float midi4;
uniform float midi5;
uniform float midi6;
uniform float midi7;
uniform float midi8;

vec3 hue(vec3 color, float shift) {

    const vec3  kRGBToYPrime = vec3 (0.299, 0.587, 0.114);
    const vec3  kRGBToI     = vec3 (0.596, -0.275, -0.321);
    const vec3  kRGBToQ     = vec3 (0.212, -0.523, 0.311);

    const vec3  kYIQToR   = vec3 (1.0, 0.956, 0.621);
    const vec3  kYIQToG   = vec3 (1.0, -0.272, -0.647);
    const vec3  kYIQToB   = vec3 (1.0, -1.107, 1.704);

    // Convert to YIQ
    float   YPrime  = dot (color, kRGBToYPrime);
    float   I      = dot (color, kRGBToI);
    float   Q      = dot (color, kRGBToQ);

    // Calculate the hue and chroma
    float   hue     = atan (Q, I);
    float   chroma  = sqrt (I * I + Q * Q);

    // Make the user's adjustments
    hue += shift;

    // Convert back to YIQ
    Q = chroma * sin (hue);
    I = chroma * cos (hue);

    // Convert back to RGB
    vec3    yIQ   = vec3 (YPrime, I, Q);
    color.r = dot (yIQ, kYIQToR);
    color.g = dot (yIQ, kYIQToG);
    color.b = dot (yIQ, kYIQToB);

    return color;
}

vec3 rgb2hsv(vec3 c)
{
    const vec4 K = vec4(0.0, -0.33333, 0.66667, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    const vec4 K = vec4(1.0, 0.66667, 0.33333, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}


void pR(inout vec2 p, float a) {
	p = cos(a)*p + sin(a)*vec2(p.y, -p.x);
}

vec2 hash( vec2 p ) // replace this by something better
{
	p = vec2( dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)) );
	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise( in vec2 p )
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

	vec2  i = floor( p + (p.x+p.y)*K1 );
    vec2  a = p - i + (i.x+i.y)*K2;
    float m = step(a.y,a.x);
    vec2  o = vec2(m,1.0-m);
    vec2  b = a - o + K2;
	vec2  c = a - 1.0 + 2.0*K2;
    vec3  h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );
	vec3  n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));
    return dot( n, vec3(70.0) );
}


float fbm(in vec2 x)
{
 	const int passes = 2;
    float res = 0.;
    float scale = 1.0;

    for (int i = 0; i < passes; i++)
    {
        res += noise(x) * scale;
        x *= 2.0;
    	scale *= 0.666;
    }
    return res;
}

float sdBox( vec2 p, vec2 b )
{
  vec2 d = abs(p) - b;
  return length(max(d,0.0))
         + min(max(d.x,d.y),0.0); // remove this line for an only partially signed sdf
}

float coolBox(vec2 uv, vec2 b)
{
    float dist = sdBox(uv, b);
    return smoothstep(0.0025, 0.0, abs(dist));
}

float shape (vec2 uv, float size, float t)
{
  return mix(coolBox(uv, vec2(size)),
             smoothstep(0.005, -0.005, max(-(length(uv)-size),length(uv)-size*1.025)), t);
}

vec2 opU(vec2 a, vec2 b)
{
    return a.x > b.x ? a : b;
}

void main() {
    vec2 uv = texCoordVarying;
    float angleOffset = 0.5;

    vec2 oUv = uv;
    float uvOffsetStrength = midi5;
    float audioStrength = 1.65;
    float t = _Time * 0.1;
    float feedbackStrength = midi7 * 4.;

    vec2 centerOffset =  vec2(cos(_Time*0.2), sin(_Time*0.2))*0.015;
    uv += centerOffset;

    vec2 uv_c = texCoordVarying * 2.0 - 1.0;

    float a = atan(uv_c.y, uv_c.x);
    float samp = texture(_AudioTexture, vec2(texCoordVarying.x*256., 0.)).r;
    float l = length(uv_c)*(0.5-samp*0.5);
    uv_c = vec2(cos(a), sin(a)) * l + 0.5;
    uv_c += vec2(fbm(uv*15.+ vec2(l,sin(_Time+a+l))), fbm(-uv*15.+ vec2(l,-cos(t+a+l))) )*midi1;

    vec4 last = texture(_LastTexture, uv_c*_Resolution);

    last.g -= 0.15;
    last.r -= 0.025;
    last.b -=samp*0.075;
    last.rgb = hue(last.rgb,.5);

    vec4 col = vec4(0.);
    float scale = midi5;

   	vec2 dist = vec2(0., -1.);
    const int passes = 8;

    vec2 offset = vec2(midi8,midi3);
    uv = uv * 2.0 - 1.0;
    uv.x *= _Resolution.x / _Resolution.y;

    for (int i = 0; i < passes; i++)
    {
        uv = abs(uv);

        scale /= (1.1);

        vec2 cell = floor(uv / scale);

        offset.xy *= (1. + sin(cell.x+cell.y+float(i)*0.5+_Time)*midi2);

        uv -= offset * scale;
        pR(uv, _Time*0.05+ midi6*4.+ float(i)*0.25+ noise(vec2(cell.x+cell.y+float(i),_Time*0.1))*midi2);
        dist = opU(dist,vec2(shape(uv, scale, midi4*2.), i));
    }

    col.rgb = vec3(dist.x);
    col = vec4(vec3(samp), 1.0);

    outputColor = col;
}
