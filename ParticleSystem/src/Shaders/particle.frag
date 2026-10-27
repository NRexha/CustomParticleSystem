#version 430 core

in vec3 vColor;
in float vAlpha;

uniform float brightness;

out vec4 FragColor;

void main()
{
    float d = length(gl_PointCoord - vec2(0.5)) * 2.0;

    float halo = exp(-d * d * 5.0);
    float core = exp(-d * d * 28.0);

    float alpha = (halo * 0.18 + core * 0.85) * vAlpha;
    vec3 color = vColor * (0.65 + core * 2.2) * brightness;

    FragColor = vec4(color, alpha);
}
