/*
{
"parameters" : [
    {
       "name" : "blendTex",
       "show" : true,
       "type" : 2,
       "textureIndex" : 1
    },
    {
       "name" : "maskTex",
       "show" : true,
       "type" : 2,
       "textureIndex" : 2
    },
    {
       "name" : "targetMaskCol",
       "show" : true,
       "type" : 3,
       "value" : {
          "r" : 0,
          "g" : 0,
          "b" : 0,
          "a" : 1
       }
    },
   {
      "name" : "tolerance",
      "range" : {
         "x" : 0,
         "y" : 1
      },
      "show" : true,
      "type" : 0,
      "value" : 1.0
   }
]
}
*/
#version 150

uniform sampler2DRect _MainTexture;
uniform vec2 _Resolution;

uniform sampler2DRect blendTex;
uniform vec2 blendTex_res;

uniform sampler2DRect maskTex;
uniform vec2 maskTex_res;

uniform vec4 targetMaskCol;

uniform float tolerance;

in vec2 texCoordVarying;
out vec4 outputColor;

void main()
{
    vec2 blendUv = vec2(texCoordVarying.x * blendTex_res.x, blendTex_res.y - texCoordVarying.y * blendTex_res.y);
    vec2 maskUv = vec2(texCoordVarying.x * maskTex_res.x, maskTex_res.y - texCoordVarying.y * maskTex_res.y);

    vec4 mainCol = texture(_MainTexture, (gl_FragCoord.xy));
    vec4 blendCol = texture(blendTex, blendUv);
    vec4 maskCol = texture(maskTex, maskUv);

    vec3 fade = abs(maskCol.rgb - targetMaskCol.rgb) * tolerance;
    outputColor = mix(mainCol, blendCol, clamp(length(fade), 0., 1.));
}
