#version 330 core

out vec4 fColor;

in vec2 fTexCoords;
in vec3 fPos;

uniform sampler2D fontMap;

void main()
{
    fColor = texture(fontMap, fTexCoords);
    
}  