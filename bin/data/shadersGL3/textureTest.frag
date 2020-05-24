/*
{
"parameters" : [
   {
      "name" : "noiseTex",
      "show" : true,
      "type" : 2,
      "filePath" : "textures/noise.png",
      "textureIndex" : 1
   },
   {
      "name" : "noiseTex2",
      "show" : true,
      "type" : 2,
      "filePath" : "textures/noise.png",
      "textureIndex" : 2
   }
]
}
*/
#version 150

uniform sampler2DRect _MainTexture;
uniform vec2 _Resolution;

uniform sampler2DRect noiseTex;
uniform vec2 noiseTex_res;

uniform sampler2DRect noiseTex2;
uniform vec2 noiseTex2_res;

in vec2 texCoordVarying;
out vec4 outputColor;

void main()
{
    vec2 texUv = vec2(texCoordVarying.x * noiseTex_res.x, noiseTex_res.y - texCoordVarying.y * noiseTex_res.y);
    vec4 c = texture(_MainTexture, gl_FragCoord.xy);
    outputColor = c;
}
