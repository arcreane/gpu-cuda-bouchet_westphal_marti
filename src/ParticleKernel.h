#pragma once
#include "Particle.h"

// Fonction CUDA pour mettre à jour les particules
void updateParticlesCUDA(Particle* particles, int count, float dt, float gravity, float friction, float rebound, int width, int height,
    float mouseX, float mouseY, float cursorStrength, float cursorRadius, bool cursorActive);