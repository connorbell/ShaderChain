/*
{
	"parameters" : [
	   {
		  "name" : "startFocusDist",
		  "range" : {
			 "x" : 0,
			 "y" : 10
		  },
		  "show" : true,
		  "type" : 0,
		  "value" : 0.3571428656578064
	   },
	   {
		  "name" : "endFocusDist",
		  "range" : {
			 "x" : 0,
			 "y" : 10
		  },
		  "show" : true,
		  "type" : 0,
		  "value" : 1.275510191917419
	   }
	]
}
*/

#version 150

uniform sampler2DRect _MainTexture;
uniform vec2 _Resolution;

in vec2 texCoordVarying;
out vec4 outputColor;

uniform float startFocusDist;
uniform float endFocusDist;

// This buffer does some bokeh based on the alpha channel of buffer A which is the distance of the surface from the pixel.

float normpdf(in float x, in float sigma) {
	return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}

void main()
{
    vec2 uv = (gl_FragCoord.xy);
    vec3 base = vec3(0.);

    // Gaussian blur by mrharicot https://www.shadertoy.com/view/XdfGDH

    const int mSize = 11;
    const int kSize = (mSize-1)/2;
    float kernel[mSize];
	float depth = texture(_MainTexture, uv).a;

    //create the 1-D kernel
    float mid = startFocusDist + distance(startFocusDist, endFocusDist) * 0.5;

    float sigma = 4.*(1.-(smoothstep(startFocusDist, mid, depth) - smoothstep(mid, endFocusDist, depth)));
    float Z = .0;
    for (int j = 0; j <= kSize; ++j)
    {
        kernel[kSize+j] = kernel[kSize-j] = normpdf(float(j), sigma);
    }

    //get the normalization factor (as the gaussian has been clamped)
    for (int j = 0; j < mSize; ++j)
    {
        Z += kernel[j];
    }

    //read out the texels
    for (int i=-kSize; i <= kSize; ++i)
    {
        for (int j=-kSize; j <= kSize; ++j)
        {
            base += kernel[kSize+j]*kernel[kSize+i]*texture(_MainTexture, (gl_FragCoord.xy+vec2(float(i),float(j))) ).rgb;
        }
    }
   	vec4 b = vec4(base/(Z*Z), 1.0);

    outputColor = b;
}
