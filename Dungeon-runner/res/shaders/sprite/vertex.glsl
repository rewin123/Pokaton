#version 330 core
layout(location = 0) in vec2 vert_pos;
layout(location = 1) in vec2 in_uv;

out vec2 TexCoords;

uniform mat4 model;
uniform mat4 projection;

void main()
{
	TexCoords = in_uv;
	gl_Position = projection * model * vec4(vert_pos, 0.0, 1.0);
}