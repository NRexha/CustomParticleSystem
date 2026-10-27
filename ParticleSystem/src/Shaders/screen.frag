#version 430 core

in vec2 uv;
out vec4 FragColor;

uniform sampler2D frameTexture;

void main()
{
    vec3 hdr = texture(frameTexture, uv).rgb;

    //tone mapping and gamma corretion
    vec3 mapped = hdr / (vec3(1.0) + hdr);
    mapped = pow(mapped, vec3(1.0 / 2.2));

    FragColor = vec4(mapped, 1.0);
}
