#version 330 core

out vec4 fColor;

uniform vec3 pColor;

void main() {

	fColor = vec4(pColor, 1.0);

}