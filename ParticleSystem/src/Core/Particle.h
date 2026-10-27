#pragma once

#include <cstddef>

struct alignas(16) Particle {
    float position[4];
    float velocity[4];
    float age;
    float lifetime;
    float padding[2];
};

static_assert(sizeof(Particle) == 48, "Particle must be 48 bytes.");
static_assert(offsetof(Particle, age) == 32, "Unexpected particle layout.");