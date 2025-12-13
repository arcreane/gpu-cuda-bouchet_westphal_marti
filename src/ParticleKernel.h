#pragma once
#include "Particle.h"

void updateParticlesCUDA(Particle* particles, int count, float dt, float gravity, float friction, float rebound, int width, int height);