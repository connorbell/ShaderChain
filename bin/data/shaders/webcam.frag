/*
{
"parameters" : [
   {
      "name" : "webcamTex",
      "show" : true,
      "type" : "texture",
      "textureIndex" : 1,
      "textype": "Webcam"
   }
]
}
*/
#version 150

uniform sampler2D _MainTexture;
uniform vec2 _Resolution;

uniform sampler2D webcamTex;

in vec2 texCoord;
out vec4 outputColor;

void main()
{
    vec4 c = texture(webcamTex,texCoord);
    outputColor = c;
}
