#version 330 core
layout(location = 0) in vec2 vert_pos;
layout(location = 1) in vec2 in_uv;

uniform vec2 pos;
uniform float scale_x;
uniform float scale_y;
out vec2 uv;
void main() {
	vec2 scaled = vec2(vert_pos.x * scale_x, vert_pos.y * scale_y);
	scaled = scaled + pos;
	gl_Position = vec4(scaled.x, scaled.y, 0.0, 1.0);
	uv = in_uv;
}