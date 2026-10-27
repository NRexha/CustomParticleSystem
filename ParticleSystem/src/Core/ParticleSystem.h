#pragma once

#include <glad/glad.h>
#include <vector>
#include <random>

#include "Particle.h"
#include "Shader.h"

class ParticleSystem
{
public:
    static constexpr int ParticleCount = 10000;

    float spawnRate = 100.0f;
    float lifetimeMin = 5.0f;
    float lifetimeMax = 9.0f;

    float emitterRadius = 0.17f;
    float emitterBaseRadius = 0.03f;

    float initialSpeedMin = 0.12f;
    float initialSpeedMax = 0.32f;

    float attractionStrength = 0.16f;
    float vortexStrength = 0.18f;
    float turbulenceStrength = 0.28f;
    float turbulenceScale = 2.2f;
    float damping = 0.30f;

    float orbitalBias = 0.8f;
    float radialBias = 0.35f;

    float trailDecay = 0.975f;

    float colorStart[3] = { 0.08f, 0.9f, 1.0f };
    float colorEnd[3] = { 0.35f, 0.18f, 1.0f };
    float colorVariation = 0.1f;
    float brightness = 1.0f;

    float randomSizeMin = 4.0f;
    float randomSizeMax = 10.0f;
    float sizeStartMultiplier = 1.0f;
    float sizeEndMultiplier = 0.2f;
    float sizeCurve = 1.0f;

    ParticleSystem();
    ~ParticleSystem();

    ParticleSystem(const ParticleSystem&) = delete;
    ParticleSystem& operator=(const ParticleSystem&) = delete;

    void Update(float dt, float time);
    void Render(int width, int height);

private:
    struct SpawnCommand
    {
        unsigned int spawn;
        float lifetime;
    };

    float spawnAccumulator = 0.0f;
    int spawnCursor = 0;

    std::vector<float> slotAvailableTime;
    std::vector<SpawnCommand> spawnCommands;

    std::mt19937 randomEngine{ std::random_device{}() };

    Shader computeShader, renderShader, trailShader, screenShader;

    GLuint ssbo = 0;
    GLuint spawnBuffer = 0;
    GLuint vao = 0;
    GLuint fullscreenVAO = 0;

    GLuint framebuffers[2] = {};
    GLuint textures[2] = {};

    int trailWidth = 0;
    int trailHeight = 0;
    int readIndex = 0;

    float currentTime = 0.0f;

    void ResizeTrailBuffer(int width, int height);
    void DrawFullscreen();
};
