#version 330 core

layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTexCoords;

out vec2 fTexCoords;
out vec3 fPos;

uniform mat4 world;

void main() {

	gl_Position = world * vec4(vPos, 1.0);
	fTexCoords = vTexCoords;

}