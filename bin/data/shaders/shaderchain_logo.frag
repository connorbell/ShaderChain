/*
{
   "parameters" : [
      {
         "name" : "size",
         "range" : {
            "x" : 0,
            "y" : 0.5
         },
         "show" : true,
         "type" : "float",
         "value" : 0.1086956486105919
      }
   ]
}
*/
#version 150

uniform float size;
uniform float _Time;

in vec2 uv;
out vec4 outputColor;

float box(vec2 uv, vec2 s) {
    vec2 q = abs(uv) - s;
    return length(max(q,0.0)) + min(max(q.x,q.y),0.0);
}

void pR(inout vec2 p, float a) {
	p = cos(a)*p + sin(a)*vec2(p.y, -p.x);
}

void main()
{
    vec4 color = vec4(0., 0., 0., 1.);
    vec2 uv = uv * 2.0 - 1.0;
    float angle = atan(uv.y, uv.x);
	pR(uv, -0.1+sin(_Time*2. + length(uv)*4. + angle*4.) * 0.05);

    float circleSize = size;

    vec2 topPos = uv-vec2(0.,circleSize);
    vec2 bottomPos = uv+vec2(0.,circleSize);
    float circleBottom = length(bottomPos) - circleSize;

    float dist = 0.0;
    float a = atan(bottomPos.y, bottomPos.x);
    float tempA = mod(a + 3.14159 + 0.5, 6.28318);
    float progress = smoothstep(-0.3, 2.05470795, tempA);

    float edge = (a < 1.570795 && a > -3.2) ? 1.0 : 0.0;

    dist += (1.-smoothstep(0.0, 0.01, abs(circleBottom))) * edge;

    a = atan(topPos.y, topPos.x);

    float tempB = mod( a+1.570795*0.25, 6.28318);
    progress *= smoothstep(0., 2., tempB);

    edge = (a < -1.570795 || a > .3) ? 1.0 : 0.0;

    float circleTop = length(topPos) - circleSize;

    dist += (1.-smoothstep(0.0, 0.01, abs(circleTop))) * edge;

    color = vec4(vec3(dist), progress);
    outputColor = color;
}
