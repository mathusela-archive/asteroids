#version 330 core

out vec4 fColor;

in vec2 fTexCoords;
in vec3 fPos;

uniform sampler2D render;

void main()
{
    fColor = texture(render, fTexCoords);

    // vec2 p=vec2(floor(fPos.x), floor(gl_FragCoord.y));

    // if (mod(floor(gl_FragCoord.y), 5.0) == 0.0) {
    //     fColor = fColor/3;
    // }
    // else {
    //     fColor = fColor;
    // }

    // Branchless method:
    fColor = fColor / (3*int(mod(floor(gl_FragCoord.y), 5.0) == 0.0) + int(!(mod(floor(gl_FragCoord.y), 5.0) == 0.0)));
}  