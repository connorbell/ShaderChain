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

uniform sampler2DRect _MainTexture;
uniform vec2 _Resolution;

uniform sampler2DRect blendTex;
uniform vec2 blendTex_res;

in vec2 texCoordVarying;
out vec4 outputColor;

float blendSoftLight(float base, float blend) {
	return (blend<0.5)?(2.0*base*blend+base*base*(1.0-2.0*blend)):(sqrt(base)*(2.0*blend-1.0)+2.0*base*(1.0-blend));
}

vec3 blendSoftLight(vec3 base, vec3 blend) {
	return vec3(blendSoftLight(base.r,blend.r),blendSoftLight(base.g,blend.g),blendSoftLight(base.b,blend.b));
}

void main()
{
    vec2 blendUv = vec2(texCoordVarying.x * blendTex_res.x, blendTex_res.y - texCoordVarying.y * blendTex_res.y);

    vec4 mainCol = texture(_MainTexture, (gl_FragCoord.xy));
    vec4 blendCol = texture(blendTex, blendUv);

    outputColor = vec4(blendSoftLight(mainCol.rgb, blendCol.rgb), 1.0);
}
