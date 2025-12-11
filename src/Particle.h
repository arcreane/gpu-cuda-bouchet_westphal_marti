#pragma once
#include <raylib.h> // Pour Vector2 et Color

struct Particle {
    Vector2 position;
    Vector2 velocity;
    float radius;
    Color color;
};