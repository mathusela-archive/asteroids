#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTexCoords;

out vec2 fTexCoords;

void main() {

	vec3 pos = vPos;

	// pos.x = pos.x * 0.9;
	// pos.y = pos.y * 0.9;

	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
	fTexCoords = vTexCoords;

}