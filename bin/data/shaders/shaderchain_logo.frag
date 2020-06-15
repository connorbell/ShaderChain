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
         "name" : "letterColor",
         "range" : {
            "x" : 0,
            "y" : 1
         },
         "show" : true,
         "type" : 1,
         "value" : {
            "x" : 1,
            "y" : 1,
            "z" : 1
         }
      }
	]
}
*/
#version 150

uniform vec4 bgColor;
uniform vec4 letterColor;

in vec2 texCoordVarying;
out vec4 outputColor;

float box(vec2 uv, vec2 s) {
    vec2 q = abs(uv) - s;
    return length(max(q,0.0)) + min(max(q.x,q.y),0.0);
}

void main()
{
    vec4 color = vec4(bgColor.rgb, 1.);
    vec2 uv = texCoordVarying * 2. - 1.;
    float circleSize = 0.35;

    vec2 topPos = uv-vec2(0.,circleSize);
    vec2 bottomPos = uv+vec2(0.,circleSize);
    float circleBottom = length(bottomPos) - circleSize;

    float dist = 0.0;
    float a = atan(bottomPos.y, bottomPos.x);
    float tempA = mod(a + 3.14159 + 0.5, 6.28318);
    float progress = smoothstep(0.25, 2.25470795, tempA);

    float edge = (a < 1.570795 && a > -3.2) ? 1.0 : 0.0;

    dist += (1.-smoothstep(0.0, 0.01, abs(circleBottom))) * edge;

    a = atan(topPos.y, topPos.x);

    float tempB = mod( a+1.570795*0.25, 6.28318);
    progress *= smoothstep(0.25, 2., tempB);

    edge = (a < -1.570795 || a > .3) ? 1.0 : 0.0;

    float circleTop = length(topPos) - circleSize;

    dist += (1.-smoothstep(0.0, 0.01, abs(circleTop))) * edge;

    color = vec4(vec3(dist), progress);
    outputColor = color;
}
