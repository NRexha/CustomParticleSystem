#version 460 core

struct Particle
{
    vec4 position;
    vec4 velocity;
    float age;
    float lifetime;
    vec2 padding;
};

layout(std430, binding = 0) buffer ParticleBuffer
{
    Particle particles[];
};

uniform float globalTime;
uniform vec3 startColor;
uniform vec3 endColor;
uniform float colorVariation;
uniform float sizeStartMultiplier;
uniform float sizeEndMultiplier;
uniform float sizeCurve;

out vec3 vColor;
out float vAlpha;

float random(float x)
{
    return fract(sin(x * 127.1 + 311.7) * 43758.5453);
}

void main()
{
    //each vertex represents one particle, indexed by its vertex ID ofc
    Particle p = particles[gl_VertexID];


    //when a particle is inactive, we move it outside the visible clip space, so it's "skipped"
    //i guess there are better methods but hey, it works
    if (p.age < 0.0)
    {
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
        gl_PointSize = 0.0;
        vColor = vec3(0.0);
        vAlpha = 0.0;
        return;
    }

    float lifeRatio = clamp(p.age / max(p.lifetime, 0.001), 0.0, 1.0);

    float angle = globalTime * 0.12;


    //orbit camera
    vec3 cameraPosition = vec3(sin(angle) * 3.5, 1.0 + sin(globalTime * 0.1) * 0.25, cos(angle) * 3.5);


    //camera unit vectors
    vec3 forward = normalize(-cameraPosition);
    vec3 right = normalize(cross(forward, vec3(0.0, 1.0, 0.0)));
    vec3 up = cross(right, forward);


    //camera relative pos
    vec3 relative = p.position.xyz - cameraPosition;
    float depth = dot(relative, forward);

    if (depth <= 0.1) //this also serves to skip particles that are either behind the camera or too close 
    {
        gl_Position = vec4(2.0, 2.0, 2.0, 1.0);
        gl_PointSize = 0.0;
        vColor = vec3(0.0);
        vAlpha = 0.0;
        return;
    }

    float focalLength = 2.8;

    float viewX = dot(relative, right);
    float viewY = dot(relative, up);


    //project to clip space
    gl_Position = vec4(viewX * focalLength, viewY * focalLength, 0.0, depth); //depth as w allows to scale distant particles down

    float sizeLifeRatio = pow(lifeRatio, max(sizeCurve, 0.001));
    float lifetimeSize = mix(sizeStartMultiplier, sizeEndMultiplier, sizeLifeRatio);

    gl_PointSize = clamp(p.padding.x * lifetimeSize * (3.5 / depth), 1.0, 32.0);

    float variation = (random(float(gl_VertexID) * 17.13) - 0.5) * 2.0 * colorVariation;
    vec3 color = mix(startColor, endColor, lifeRatio);

    vColor = clamp(color + variation, 0.0, 1.0);
    vAlpha = 1.0 - smoothstep(0.65, 1.0, lifeRatio);
}


//note: I could have also calculate the camera projection with matrices, but since i don't need to control the cameraPosition
//through the cpu I did it manually. I guees that this is not a valid option for a scalable project, but for this samll demo
//i think it's fine, and I also had to re-remember all the maths behind it, so it was worth it

//void main()
//{
    //Particle p = particles[gl_VertexID];

    //vec4 worldPosition = vec4(p.position.xyz, 1.0);

    //gl_Position = projectionMatrix * viewMatrix * worldPosition;
//}
