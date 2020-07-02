/*
{
    "parameters" : [
       {
          "name" : "intensity",
          "range" : {
             "x" : 0,
             "y" : 1
          },
          "show" : true,
          "type" : "float",
          "value" : 0.75
       },
       {
          "name" : "scale",
          "range" : {
             "x" : 0,
             "y" : 2
          },
          "show" : true,
          "type" : "float",
          "value" : 1.0
       },
       {
          "name" : "lastTex",
          "show" : true,
          "type" : "texture",
          "textype" : "Last",
          "textureIndex" : 2
       }
    ]
}
*/

#version 150

uniform sampler2D _MainTexture;
uniform sampler2D lastTex;
uniform float intensity;
uniform float scale;
uniform float _Time;
uniform vec2 _Resolution;

in vec2 texCoord;
out vec4 outputColor;

void pR(inout vec2 p, float a) {
	p = cos(a)*p + sin(a)*vec2(p.y, -p.x);
}

void main()
{
    vec4 color = texture(_MainTexture, texCoord);

    vec2 uv = texCoord;

    uv = (uv - 0.5) * scale + 0.5;

    vec4 lastColor = texture(lastTex, uv);
    color.rgb += lastColor.rgb * intensity; // * (1. + audio*.4);
    outputColor = clamp(vec4(color), vec4(0.0), vec4(1.0));
}
