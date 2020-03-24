#version 150

uniform sampler2DRect _MainTexture;
uniform sampler2DRect _LastTexture;
//uniform sampler2DRect _AudioTexture;
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
    vec2 uv = gl_FragCoord.xy;

    vec4 lastColor = texture(_LastTexture, uv);
    color.rgb += lastColor.rgb * scale; // * (1. + audio*.4);
    outputColor = vec4(color);
}
