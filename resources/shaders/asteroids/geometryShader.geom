#version 330 core

uniform float radius;
uniform int segments;

layout(points) in;
layout(line_strip, max_vertices = 128) out;

const float PI = 3.141592;

void main()
{
    for (int i; i <= segments; i++) {

        float ang = PI * 2.0 / segments * i;

        vec4 offset = vec4(cos(ang) * radius, -sin(ang) * radius, 0.0, 0.0);

        gl_Position = gl_in[0].gl_Position + offset;
        EmitVertex();
    }

    EndPrimitive();
}