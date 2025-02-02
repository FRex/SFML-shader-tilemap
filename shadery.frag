#version 130

uniform sampler2D mapdata;
uniform sampler2D tilegraphics;

void main()
{
    vec2 a = texture2D(mapdata, gl_TexCoord[0].xy).rg * 255.0;
    vec2 b = mod(gl_TexCoord[0].xy * textureSize(mapdata, 0), 1.0);
    gl_FragColor = vec4(0.0, b.y, 0.0, 1.0);
}
