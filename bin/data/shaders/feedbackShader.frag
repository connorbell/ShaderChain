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
          "textureIndex" : 1
       }
    ]
}
*/

#version 150

uniform sampler2DRect _MainTexture;
uniform sampler2DRect lastTex;
//uniform sampler2DRect _AudioTexture;
uniform float intensity;
uniform float scale;
uniform float _Time;
uniform vec2 _Resolution;

in vec2 texCoordVarying;
out vec4 outputColor;

void main()
{
    vec4 color = texture(_MainTexture, gl_FragCoord.xy);

    //vec2 uv_c = (gl_FragCoord.xy / _Resolution) * 2.0 - 1.0;

    //float a = atan(uv_c.y, uv_c.x);
    //float audio = texture(_AudioTexture, vec2(sin(a)*4.+4., 0.)).r;
    //float l = length(uv_c)*0.5*(1.-audio*0.015);
    //vec2 uv = vec2(cos(a), sin(a)) * l + 0.5;

    vec2 uv = (gl_FragCoord.xy/_Resolution);
    uv = (uv - 0.5) * scale + 0.5;

    vec4 lastColor = texture(lastTex, uv*_Resolution);
    color.rgb += lastColor.rgb * intensity; // * (1. + audio*.4);
    outputColor = clamp(vec4(color), vec4(0.0), vec4(1.0));
}