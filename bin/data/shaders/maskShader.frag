/*
{
"parameters" : [
    {
       "name" : "blendTex",
       "show" : true,
       "type" : "texture",
       "textureIndex" : 1
    },
    {
       "name" : "maskTex",
       "show" : true,
       "type" : "texture",
       "textureIndex" : 2
    },
    {
       "name" : "targetMaskCol",
       "show" : true,
       "type" : "color",
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
      "type" : "float",
      "value" : 1.0
   },
  {
     "name" : "invert",
     "show" : true,
     "type" : "bool",
     "value" : false
  }
]
}
*/
#version 150

uniform sampler2D _MainTexture;
uniform vec2 _Resolution;

uniform sampler2D blendTex;

uniform sampler2D maskTex;

uniform vec4 targetMaskCol;

uniform float tolerance;

uniform bool invert;
in vec2 uv;
in vec2 texCoord;

out vec4 outputColor;

void main()
{
    vec4 mainCol = texture(_MainTexture, texCoord);
    vec4 blendCol = texture(blendTex, texCoord);
    vec4 maskCol = texture(maskTex, texCoord);

    vec3 fade = abs(maskCol.rgb - targetMaskCol.rgb) * tolerance;
    if (invert) fade = 1. - fade;
    outputColor = mix(mainCol, blendCol, clamp(length(fade), 0., 1.));
}
