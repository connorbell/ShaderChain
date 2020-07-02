/*
{
    "parameters" : [
      {
         "name" : "cellOffset",
         "range" : {
            "x" : 0,
            "y" : 1
         },
         "show" : true,
         "type" : "float",
         "midi" : 1,
         "value" : 0.1
      },
      {
         "name" : "phase",
         "range" : {
            "x" : 0,
            "y" : 6.28
         },
         "show" : true,
         "type" : "float",
         "midi" : 2,
         "value" : 1.0
      },
      {
         "name" : "startScale",
         "range" : {
            "x" : 0.1,
            "y" : 1
         },
         "show" : true,
         "type" : "float",
         "midi" : 3,
         "value" : 0.5
      },
      {
         "name" : "scalingFactor",
         "range" : {
            "x" : 0.1,
            "y" : 2
         },
         "show" : true,
         "type" : "float",
         "midi" : 4,
         "value" : 0.5
      },
      {
         "name" : "offset",
         "range" : {
            "x" : 0,
            "y" : 1
         },
         "show" : true,
         "type" : "vec2",
         "value" : {
            "x": 0.5,
            "y": 0.5
         },
         "midi" : {
            "x": 5,
            "y": 6
         }
      },
	  {
        "name" : "audioac",
        "range" : {
           "x" : 0,
           "y" : 6.2831853
        },
		"frequencyRange" : {
			"x" : 0,
			"y" : 200
		},
		"scaleFactor" : 0.2,
		"expFactor" : 2,
		"accumulate" : true,
        "show" : true,
        "type" : "audioFloat",
        "value" : 0
      }
   ]
}
*/

#version 150

in vec2 uv;
out vec4 outputColor;

uniform float _Time;
uniform sampler2D _MainTexture;
uniform vec2 _Resolution;

uniform float cellOffset;
uniform float startScale;
uniform float scalingFactor;
uniform float phase;
uniform vec2 offset;
uniform float audioac;

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
    return smoothstep(0.01, 0.0, abs(dist));
}

float shape (vec2 uv, float size, float t)
{
  return mix(coolBox(uv, vec2(size)),
             smoothstep(0.001, -0.001, max(-(length(uv)-size),length(uv)-size*1.025)), t);
}

vec2 opU(vec2 a, vec2 b)
{
    return a.x > b.x ? a : b;
}

void main() {
    vec2 uv = uv;
    float t = audioac * 2.;

    vec2 centerOffset =  vec2(cos(t), sin(t))*0.015;
    uv += centerOffset;

    float scale = startScale;

   	vec2 dist = vec2(0., -1.);
    const int passes = 6;

    vec2 offs = offset;
    uv = uv * 2.0 - 1.0;
    uv.x *= _Resolution.x / _Resolution.y;

    for (int i = 0; i < passes; i++)
    {
        uv = abs(uv);

        scale /= scalingFactor;

        vec2 cell = floor(uv / scale);

        offs.xy *= (1. + sin(cell.x+cell.y+float(i)*0.5+t)*cellOffset);

        uv -= offs * scale;
        pR(uv, t + phase + float(i)*0.25+ noise(vec2(cell.x+cell.y+float(i),t))*cellOffset);
        dist = opU(dist,vec2(shape(uv, scale, 0.5), i));
    }

    vec4 col = vec4(1.);
    col.rgb = vec3(dist.x);

    outputColor = col;
}
