
#include "ParticleSystemUI.h"

#include "imgui.h"

#include <algorithm>

void ParticleSystemUI::Draw(ParticleSystem& particles, float deltaTime)
{
#pragma region Overview
    ImGui::Begin("Particle System");

    ImGui::Text("GPU Particle Simulation");
    ImGui::Separator();
    ImGui::Text("Particle capacity: %d", ParticleSystem::ParticleCount);
    ImGui::Text("Frame time: %.2f ms", deltaTime * 1000.0f);

    if (deltaTime > 0.0f)
        ImGui::Text("FPS: %.1f", 1.0f / deltaTime);

    ImGui::Spacing();
#pragma endregion

#pragma region Spawning
    if (ImGui::CollapsingHeader("Spawning", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderFloat("Spawn rate", &particles.spawnRate, 0.0f, 2000.0f, "%.0f particles/s");

        ImGui::SliderFloat("Minimum lifetime", &particles.lifetimeMin, 0.1f, 20.0f, "%.1f s");
        ImGui::SliderFloat("Maximum lifetime", &particles.lifetimeMax, 0.1f, 20.0f, "%.1f s");

        particles.lifetimeMin = std::min(particles.lifetimeMin, particles.lifetimeMax);
        particles.lifetimeMax = std::max(particles.lifetimeMax, particles.lifetimeMin);
    }
#pragma endregion

#pragma region Emitter
    if (ImGui::CollapsingHeader("Emitter", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderFloat("Base radius", &particles.emitterBaseRadius, 0.0f, 0.5f, "%.3f");
        ImGui::SliderFloat("Radius variation", &particles.emitterRadius, 0.0f, 1.0f, "%.3f");

        ImGui::SliderFloat("Minimum initial speed", &particles.initialSpeedMin, 0.0f, 2.0f, "%.2f");
        ImGui::SliderFloat("Maximum initial speed", &particles.initialSpeedMax, 0.0f, 2.0f, "%.2f");

        particles.initialSpeedMin = std::min(particles.initialSpeedMin, particles.initialSpeedMax);
        particles.initialSpeedMax = std::max(particles.initialSpeedMax, particles.initialSpeedMin);
    }
#pragma endregion

#pragma region Motion
    if (ImGui::CollapsingHeader("Motion", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderFloat("Attraction", &particles.attractionStrength, 0.0f, 2.0f, "%.3f");
        ImGui::SliderFloat("Vortex", &particles.vortexStrength, 0.0f, 2.0f, "%.3f");
        ImGui::SliderFloat("Turbulence", &particles.turbulenceStrength, 0.0f, 2.0f, "%.3f");
        ImGui::SliderFloat("Turbulence scale", &particles.turbulenceScale, 0.1f, 10.0f, "%.2f");
        ImGui::SliderFloat("Damping", &particles.damping, 0.0f, 5.0f, "%.3f");

        ImGui::Separator();

        ImGui::SliderFloat("Orbital bias", &particles.orbitalBias, 0.0f, 2.0f, "%.2f");
        ImGui::SliderFloat("Radial bias", &particles.radialBias, 0.0f, 2.0f, "%.2f");
    }
#pragma endregion

#pragma region ColorsAndRendering
    if (ImGui::CollapsingHeader("Colors & Rendering", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::ColorEdit3("Start color", particles.colorStart);
        ImGui::ColorEdit3("End color", particles.colorEnd);

        ImGui::SliderFloat("Color variation", &particles.colorVariation, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Brightness", &particles.brightness, 0.0f, 5.0f, "%.2f");
    }
#pragma endregion

#pragma region ParticleSize
    if (ImGui::CollapsingHeader("Particle Size", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderFloat("Minimum size", &particles.randomSizeMin, 1.0f, 20.0f, "%.1f px");
        ImGui::SliderFloat("Maximum size", &particles.randomSizeMax, 1.0f, 20.0f, "%.1f px");

        if (particles.randomSizeMin > particles.randomSizeMax)
            particles.randomSizeMax = particles.randomSizeMin;

        ImGui::Separator();

        ImGui::SliderFloat("Start multiplier", &particles.sizeStartMultiplier, 0.0f, 2.0f, "%.2f");
        ImGui::SliderFloat("End multiplier", &particles.sizeEndMultiplier, 0.0f, 2.0f, "%.2f");

        ImGui::SliderFloat("Shrink curve", &particles.sizeCurve, 0.25f, 4.0f, "%.2f");
    }
#pragma endregion

#pragma region Trails
    if (ImGui::CollapsingHeader("Trails", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::SliderFloat("Trail decay", &particles.trailDecay, 0.90f, 0.999f, "%.4f");
    }
#pragma endregion

    ImGui::End();
}
