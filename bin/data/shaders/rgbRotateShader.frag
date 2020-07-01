/*
{
"parameters" : [
   {
      "name" : "colMult",
      "range" : {
         "x" : 0,
         "y" : 20
      },
      "show" : true,
      "type" : "vec3",
      "value" : {
        "x" : 2.5,
        "y" : 5.0,
        "z" : 10.0
      }
   },
   {
      "name" : "phase",
      "range" : {
         "x" : 0,
         "y" : 20
      },
      "show" : true,
      "type" : "float",
      "value" : 0.5
   }
]
}
*/

#version 150

uniform sampler2DRect _MainTexture;
uniform vec3 colMult;
uniform float phase;

in vec2 texCoordVarying;
out vec4 outputColor;

void main()
{
    vec4 color = texture(_MainTexture, (gl_FragCoord.xy));
    color.rgb = sin(color.rgb * colMult + phase) * 0.5 + 0.5;
    outputColor = color;
}
