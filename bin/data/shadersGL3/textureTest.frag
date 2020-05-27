/*
{
"parameters" : [
   {
      "name" : "noiseTex",
      "show" : true,
      "type" : "texture",
      "filePath" : "textures/noise.png",
      "textureIndex" : 1
   }
]
}
*/
#version 150

uniform sampler2DRect _MainTexture;
uniform vec2 _Resolution;

uniform sampler2DRect noiseTex;
uniform vec2 noiseTex_res;


in vec2 texCoordVarying;
out vec4 outputColor;

void main()
{
    vec2 texUv = vec2(texCoordVarying.x * noiseTex_res.x, noiseTex_res.y - texCoordVarying.y * noiseTex_res.y);
    vec4 c = texture(noiseTex,texUv);
    outputColor = c;
}
