#include "ParticleKernel.h"
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <cstdio> // Pour printf

// Macro de vérification d'erreur
#define gpuErrchk(ans) { gpuAssert((ans), __FILE__, __LINE__); }
inline void gpuAssert(cudaError_t code, const char* file, int line, bool abort = true) {
    if (code != cudaSuccess) {
        fprintf(stderr, "ERREUR CUDA: %s %s %d\n", cudaGetErrorString(code), file, line);;
    }
}

__global__ void updateParticlesKernel(Particle* particles, int count, float dt, float gravity, float friction, float rebound, int width, int height) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= count) return;

    Particle p = particles[i];

    // --- Physique ---
    p.velocity.y += gravity * dt * 10.0f;

    float damping = 1.0f - (friction * dt * 2.0f);
    if (damping < 0) damping = 0;
    p.velocity.x *= damping;
    p.velocity.y *= damping;

    p.position.x += p.velocity.x;
    p.position.y += p.velocity.y;

    // Collisions Murs
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

    particles[i] = p;
}

// Wrapper corrigé
void updateParticlesCUDA(Particle* particles, int count, float dt, float gravity, float friction, float rebound, int width, int height) {
    Particle* d_particles = nullptr;
    size_t size = count * sizeof(Particle);

    // 1. Allocation
    gpuErrchk(cudaMalloc(&d_particles, size));

    // 2. Copie CPU -> GPU
    gpuErrchk(cudaMemcpy(d_particles, particles, size, cudaMemcpyHostToDevice));

    // 3. Config
    int threadsPerBlock = 256;
    int blocksPerGrid = (count + threadsPerBlock - 1) / threadsPerBlock;

    // 4. Lancement (ATTENTION : <<< et >>> doivent être collés, sans espace !)
    updateParticlesKernel <<<blocksPerGrid, threadsPerBlock>>> (d_particles, count, dt, gravity, friction, rebound, width, height);

    // Vérification immédiate du lancement
    gpuErrchk(cudaPeekAtLastError());
    gpuErrchk(cudaDeviceSynchronize());

    // 5. Retour GPU -> CPU
    gpuErrchk(cudaMemcpy(particles, d_particles, size, cudaMemcpyDeviceToHost));

    // 6. Ménage
    gpuErrchk(cudaFree(d_particles));
}