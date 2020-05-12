#version 330 core
in vec2 uv;
layout(location = 0) out vec4 outColor;

uniform sampler2D tex;
void main ( )
{
	float clr = texture(tex, uv).r;
	outColor = vec4(clr, clr, clr, clr);
}