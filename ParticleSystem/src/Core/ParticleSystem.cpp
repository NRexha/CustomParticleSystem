#include "ParticleSystem.h"

#include <vector>
#include <iostream>

ParticleSystem::ParticleSystem()
    : computeShader("src/Shaders/particle.comp"),
    renderShader("src/Shaders/particle.vert", "src/Shaders/particle.frag"),
    trailShader("src/Shaders/fullscreen.vert", "src/Shaders/trail.frag"),
    screenShader("src/Shaders/fullscreen.vert", "src/Shaders/screen.frag")
{
#pragma region ParticleInit
    std::vector<Particle> initialParticles(ParticleCount);

    slotAvailableTime.assign(ParticleCount, 0.0f);
    spawnCommands.resize(ParticleCount);

    for (int i = 0; i < ParticleCount; ++i)
    {
        auto& p = initialParticles[i];

        p.position[0] = 2.0f;
        p.position[1] = 2.0f;
        p.position[2] = 2.0f;
        p.position[3] = 1.0f;

        p.velocity[0] = 0.0f;
        p.velocity[1] = 0.0f;
        p.velocity[2] = 0.0f;
        p.velocity[3] = 0.0f;

        p.age = -1.0f;
        p.lifetime = 0.0f;

        p.padding[0] = 0.0f;
        p.padding[1] = 0.0f;

        spawnCommands[i] = { 0u, 0.0f };
    }
#pragma endregion

#pragma region OpenGLInit
    glGenBuffers(1, &ssbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);

    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Particle) * ParticleCount, initialParticles.data(), GL_DYNAMIC_COPY);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    glGenBuffers(1, &spawnBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, spawnBuffer);

    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(SpawnCommand) * ParticleCount, spawnCommands.data(), GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, spawnBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glGenVertexArrays(1, &vao);
    glGenVertexArrays(1, &fullscreenVAO);
#pragma endregion
}

ParticleSystem::~ParticleSystem()
{
    if (ssbo)
        glDeleteBuffers(1, &ssbo);

    if (spawnBuffer)
        glDeleteBuffers(1, &spawnBuffer);

    if (vao)
        glDeleteVertexArrays(1, &vao);

    if (fullscreenVAO)
        glDeleteVertexArrays(1, &fullscreenVAO);

    glDeleteFramebuffers(2, framebuffers);
    glDeleteTextures(2, textures);
}

void ParticleSystem::Update(float dt, float time)
{
#pragma region SpawnParticles
    currentTime = time;

    for (auto& command : spawnCommands)
        command = { 0u, 0.0f };

    spawnAccumulator += spawnRate * dt;

    int requested = static_cast<int>(spawnAccumulator);
    spawnAccumulator -= static_cast<float>(requested);

    std::uniform_real_distribution<float> lifetimeDistribution(lifetimeMin, lifetimeMax);

    int spawned = 0;
    int checked = 0;

    while (spawned < requested && checked < ParticleCount)
    {
        int slot = spawnCursor;

        spawnCursor = (spawnCursor + 1) % ParticleCount;
        ++checked;

        if (slotAvailableTime[slot] > time)
            continue;

        float lifetime = lifetimeDistribution(randomEngine);

        spawnCommands[slot] = { 1u, lifetime };
        slotAvailableTime[slot] = time + lifetime;

        ++spawned;
    }
#pragma endregion

#pragma region UploadSpawnCommands
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, spawnBuffer);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(SpawnCommand) * ParticleCount, spawnCommands.data());
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, spawnBuffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
#pragma endregion

#pragma region ComputeSimulation
    computeShader.Use();

    computeShader.SetFloat("deltaTime", dt);
    computeShader.SetFloat("globalTime", time);
    computeShader.SetVec2("emitterPosition", 0.0f, 0.0f);

    computeShader.SetFloat("attractionStrength", attractionStrength);
    computeShader.SetFloat("vortexStrength", vortexStrength);
    computeShader.SetFloat("turbulenceStrength", turbulenceStrength);
    computeShader.SetFloat("turbulenceScale", turbulenceScale);
    computeShader.SetFloat("damping", damping);

    computeShader.SetFloat("emitterRadius", emitterRadius);
    computeShader.SetFloat("emitterBaseRadius", emitterBaseRadius);
    computeShader.SetFloat("initialSpeedMin", initialSpeedMin);
    computeShader.SetFloat("initialSpeedMax", initialSpeedMax);
    computeShader.SetFloat("randomSizeMin", randomSizeMin);
    computeShader.SetFloat("randomSizeMax", randomSizeMax);

    computeShader.SetFloat("orbitalBias", orbitalBias);
    computeShader.SetFloat("radialBias", radialBias);

    glDispatchCompute((ParticleCount + 255) / 256, 1, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
#pragma endregion
}

void ParticleSystem::ResizeTrailBuffer(int width, int height)
{
    if (width == trailWidth && height == trailHeight)
        return;

    glDeleteFramebuffers(2, framebuffers);
    glDeleteTextures(2, textures);

    framebuffers[0] = framebuffers[1] = 0;
    textures[0] = textures[1] = 0;

    trailWidth = width;
    trailHeight = height;
    readIndex = 0;

    glGenFramebuffers(2, framebuffers);
    glGenTextures(2, textures);

    for (int i = 0; i < 2; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, textures[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[i]);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textures[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "Trail framebuffer " << i << " is incomplete.\n";
        }

        glViewport(0, 0, width, height);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

}

void ParticleSystem::DrawFullscreen()
{
    glBindVertexArray(fullscreenVAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void ParticleSystem::Render(int width, int height)
{
    if (width <= 0 || height <= 0)
        return;

    ResizeTrailBuffer(width, height);

    const int writeIndex = 1 - readIndex;

#pragma region TrailAccumulation
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[writeIndex]);
    glViewport(0, 0, trailWidth, trailHeight);

    trailShader.Use();
    trailShader.SetFloat("decay", trailDecay);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[readIndex]);

    GLint currentProgram = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

    glUniform1i(glGetUniformLocation(currentProgram, "previousFrame"), 0);

    DrawFullscreen();
#pragma endregion

#pragma region ParticleRendering
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);

    renderShader.Use();
    renderShader.SetFloat("globalTime", currentTime);

    renderShader.SetVec3("startColor", colorStart[0], colorStart[1], colorStart[2]);
    renderShader.SetVec3("endColor", colorEnd[0], colorEnd[1], colorEnd[2]);
    renderShader.SetFloat("colorVariation", colorVariation);
    renderShader.SetFloat("brightness", brightness);
    renderShader.SetFloat("sizeStartMultiplier", sizeStartMultiplier);
    renderShader.SetFloat("sizeEndMultiplier", sizeEndMultiplier);
    renderShader.SetFloat("sizeCurve", sizeCurve);

    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, ParticleCount);
    glBindVertexArray(0);
#pragma endregion

#pragma region FinalComposition
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);

    glClearColor(0.015f, 0.02f, 0.04f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    screenShader.Use();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[writeIndex]);

    glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);

    glUniform1i(glGetUniformLocation(currentProgram, "frameTexture"), 0);

    DrawFullscreen();

    readIndex = writeIndex;

    glBindTexture(GL_TEXTURE_2D, 0);
#pragma endregion
}
