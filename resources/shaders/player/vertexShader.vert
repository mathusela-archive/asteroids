#version 330 core

layout (location = 0) in vec3 vPos;

uniform mat4 world;

void main() {

	gl_Position = world * vec4(vPos, 1.0);

}