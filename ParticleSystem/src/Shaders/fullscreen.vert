#version 460 core

out vec2 uv;

void main()
{
    //generate the triangle's vertex positions from the vertex ID
    //no vertex buffer or vertex attributes are needed
    vec2 p = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2); //bitwise operations to generate three 2D coordinates
    uv = p;

    //convert the coordinates to clip space covering the whole screen
    gl_Position = vec4(p * 2.0 - 1.0, 0.0, 1.0);
}