#version 330 core
in vec2 uv;
layout(location = 0) out vec4 outColor;

uniform sampler2D tex;
void main ( )
{
	vec4 clr = texture(tex, uv);
	outColor = vec4(clr.r, clr.g, clr.b, clr.a);
}