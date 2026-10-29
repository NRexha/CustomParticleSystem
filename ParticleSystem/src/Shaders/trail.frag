#version 460 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D previousFrame;
uniform float decay;

void main()
{
    //basically screen space trail effect using feedback from the previous frame
    //previous frame colors are faded by decay, so older particle positions gradually disappear over time
    FragColor = texture(previousFrame, uv) * decay;
}
