#include "ParticleKernel.h"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <cstdio>
#include <cmath> // Pour sqrtf

#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char* file, int line, bool abort = true) {
    if (code != cudaSuccess) {
        fprintf(stderr, "ERREUR CUDA: %s %s %d\n", cudaGetErrorString(code), file, line);
    }
}

// Kernel CUDA mis à jour avec interaction souris
__global__ void updateParticlesKernel(Particle* particles, int count, float dt, float gravity, float friction, float rebound, int width, int height,
    float mouseX, float mouseY, float cursorStrength, float cursorRadius, bool cursorActive) {

    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= count) return;

    Particle p = particles[i];

    // INTERACTION SOURIS (Portage du code CPU vers GPU
    if (cursorActive) {
        float dx = mouseX - p.position.x;
        float dy = mouseY - p.position.y;

        // distance au carré pour éviter sqrtf inutile
        float distSq = dx * dx + dy * dy;
        float radiusSq = cursorRadius * cursorRadius;

        // Si on est dans le cercle
        if (distSq < radiusSq && distSq > 1.0f) {
            float dist = sqrtf(distSq);

            // Normalisation
            float nx = dx / dist;
            float ny = dy / dist;

            // Facteur linéaire (1 au centre, 0 au bord)
            float forceFactor = (1.0f - (dist / cursorRadius));

            // Application de la force
            p.velocity.x += nx * forceFactor * cursorStrength * 2.0f;
            p.velocity.y += ny * forceFactor * cursorStrength * 2.0f;
        }
    }

    // ---  PHYSIQUE CLASSIQUE ---
	// Gravité
    p.velocity.y += gravity * dt * 10.0f;

	// Friction
    float damping = 1.0f - (friction * dt * 2.0f);
    if (damping < 0) damping = 0;
    p.velocity.x *= damping;
    p.velocity.y *= damping;

	// Mise à jour position
    p.position.x += p.velocity.x;
    p.position.y += p.velocity.y;

    // ---  COLLISIONS MURS ---
    if (p.position.y > height - p.radius) {
        p.position.y = height - p.radius;
        p.velocity.y *= -rebound;
    }
    if (p.position.y < p.radius) {
        p.position.y = p.radius;
        p.velocity.y *= -rebound;
    }
    if (p.position.x > width - p.radius || p.position.x < p.radius) {
        p.velocity.x *= -rebound;
        if (p.position.x > width - p.radius) p.position.x = width - p.radius;
        if (p.position.x < p.radius) p.position.x = p.radius;
    }

    // ---  COLLISIONS ENTRE PARTICULES ---
    for (int j = 0; j < count; j++) {
        if (i == j) continue;
        Particle other = particles[j];
        float dx = p.position.x - other.position.x;
        float dy = p.position.y - other.position.y;
        float distSq = dx * dx + dy * dy;
        float minDist = p.radius + other.radius;

        if (distSq < minDist * minDist && distSq > 0.0001f) {
            float dist = sqrtf(distSq);
            float nx = dx / dist;
            float ny = dy / dist;
            float overlap = minDist - dist;

            p.position.x += nx * overlap * 0.5f;
            p.position.y += ny * overlap * 0.5f;

            float dvx = p.velocity.x - other.velocity.x;
            float dvy = p.velocity.y - other.velocity.y;
            float dot = dvx * nx + dvy * ny;

            if (dot < 0) {
                float impulse = -(1.0f + rebound) * dot * 0.5f;
                p.velocity.x += impulse * nx;
                p.velocity.y += impulse * ny;
            }
        }
    }

    particles[i] = p;
}

// Wrapper mis à jour
void updateParticlesCUDA(Particle* particles, int count, float dt, float gravity, float friction, float rebound, int width, int height,
    float mouseX, float mouseY, float cursorStrength, float cursorRadius, bool cursorActive) {

    Particle* d_particles = nullptr;
    size_t size = count * sizeof(Particle);

    gpuErrchk(cudaMalloc(&d_particles, size));
    gpuErrchk(cudaMemcpy(d_particles, particles, size, cudaMemcpyHostToDevice));

    int threadsPerBlock = 256;
    int blocksPerGrid = (count + threadsPerBlock - 1) / threadsPerBlock;

    // On passe TOUS les nouveaux arguments au Kernel
    updateParticlesKernel << <blocksPerGrid, threadsPerBlock >> > (
        d_particles, count, dt, gravity, friction, rebound, width, height,
        mouseX, mouseY, cursorStrength, cursorRadius, cursorActive
        );

    gpuErrchk(cudaPeekAtLastError());
    gpuErrchk(cudaDeviceSynchronize());
    gpuErrchk(cudaMemcpy(particles, d_particles, size, cudaMemcpyDeviceToHost));
    gpuErrchk(cudaFree(d_particles));
}