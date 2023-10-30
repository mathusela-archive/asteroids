#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTexCoords;

out vec2 fTexCoords;
out vec3 fPos;

void main() {

	vec3 pos = vPos;

	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);
	fTexCoords = vTexCoords;
	fPos = vPos;

}