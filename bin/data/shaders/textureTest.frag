/*
{
"parameters" : [
   {
      "name" : "inputTex",
      "show" : true,
      "type" : "texture",
      "filePath" : "textures/test.jpg",
      "textureIndex" : 2
   }
]
}
*/
#version 150

uniform sampler2D _MainTexture;
uniform vec2 _Resolution;

uniform sampler2D inputTex;

in vec2 texCoord;

out vec4 outputColor;

void main()
{
    vec4 c = texture(inputTex, texCoord);
    c.rgb = mix(texture(_MainTexture, texCoord).rgb, c.rgb, 1.);
    outputColor = c;
}
