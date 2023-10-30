#version 330 core

out vec4 fColor;

in vec2 fTexCoords;

uniform sampler2D render;

void main()
{
    fColor = texture(render, fTexCoords);
}  