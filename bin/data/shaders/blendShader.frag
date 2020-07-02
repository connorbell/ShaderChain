/*
{
"parameters" : [
    {
       "name" : "blendTex",
       "show" : true,
       "type" : "texture",
       "textureIndex" : 1
    }
]
}
*/
#version 150

uniform sampler2D _MainTexture;
uniform vec2 _Resolution;

uniform sampler2D blendTex;
uniform vec2 blendTex_res;

in vec2 uv;
in vec2 texCoord;
out vec4 outputColor;

float blendSoftLight(float base, float blend) {
	return (blend<0.5)?(2.0*base*blend+base*base*(1.0-2.0*blend)):(sqrt(base)*(2.0*blend-1.0)+2.0*base*(1.0-blend));
}

vec3 blendSoftLight(vec3 base, vec3 blend) {
	return vec3(blendSoftLight(base.r,blend.r),blendSoftLight(base.g,blend.g),blendSoftLight(base.b,blend.b));
}

void main()
{
    vec4 mainCol = texture(_MainTexture, texCoord);
    vec4 blendCol = texture(blendTex, texCoord);

    outputColor = vec4(blendSoftLight(mainCol.rgb, blendCol.rgb), 1.0);
}
