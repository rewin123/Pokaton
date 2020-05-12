#version 330 core
layout(location = 0) in vec2 vert_pos;

uniform vec2 pos;
out vec2 uv;
void main() {
	vec2 scaled = pos + vert_pos;
	gl_Position = vec4(pos.x, pos.y, 0.0, 1.0);
	uv = vert_pos;
}