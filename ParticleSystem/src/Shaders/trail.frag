#version 430 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D previousFrame;
uniform float decay;

void main()
{
    FragColor = texture(previousFrame, uv) * decay;
}
