/*
{
"parameters" : [
   {
      "name" : "blurSize",
      "range" : {
         "x" : 1,
         "y" : 32
      },
      "show" : true,
      "type" : "float",
      "value" : 1.0
   },
   {
      "name" : "brightness",
      "range" : {
         "x" : 0,
         "y" : 32
      },
      "show" : true,
      "type" : "float",
      "value" : 1.0
   }
]
}
*/
#version 150

uniform sampler2D _MainTexture;
uniform vec2 _Resolution;

uniform float blurSize;
uniform float brightness;
uniform int samples;

uniform bool invert;
in vec2 uv;
in vec2 texCoord;

out vec4 outputColor;

float normpdf(in float x, in float sigma) {
	return 0.39894*exp(-0.5*x*x/(sigma*sigma))/sigma;
}

void main()
{
    vec3 base = vec3(0.);

    // Gaussian blur by mrharicot https://www.shadertoy.com/view/XdfGDH
    const int mSize = 13;
    const int kSize = (mSize-1)/2;
    float kernel[mSize];

    //create the 1-D kernel
    float sigma = blurSize;
    float Z = .0;

    for (int j = 0; j <= kSize; ++j) {
        kernel[kSize+j] = kernel[kSize-j] = normpdf(float(j), sigma);
    }

    //get the normalization factor (as the gaussian has been clamped)
    for (int j = 0; j < mSize; ++j) {
        Z += kernel[j];
    }

    //read out the texels
    vec2 texelSize = vec2(1.) / _Resolution;

    for (int i=-kSize; i <= kSize; ++i)
    {
        for (int j=-kSize; j <= kSize; ++j)
        {
            base += kernel[kSize+j]*kernel[kSize+i]*texture(_MainTexture, (texCoord+vec2(float(i),float(j))*texelSize) ).rgb;
        }
    }
   	vec4 b = vec4(base/(Z*Z), 1.0);
    b.rgb = pow(b.rgb, vec3(brightness));
    outputColor = vec4(b);
}
