#include "RaylibWidget.h"
#include <random>

RaylibWidget::RaylibWidget(QWidget* parent) : QWidget(parent) {
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setFocusPolicy(Qt::StrongFocus);
}

RaylibWidget::~RaylibWidget() {
    if (m_isInitialized) CloseWindow();
}

// --- Nouvelles Méthodes ---
void RaylibWidget::togglePause() {
    m_isPaused = !m_isPaused;
}

void RaylibWidget::reset() {
    initParticles();
}
// --------------------------

void RaylibWidget::setGravity(float g) {
    m_gravity = g;
}

void RaylibWidget::initRaylib() {
    InitWindow(width(), height(), "");
    m_isInitialized = true;
    initParticles();
}

void RaylibWidget::initParticles() {
    m_particles.clear();
    for (int i = 0; i < PARTICLE_COUNT; i++) {
        Particle p;
        p.position = { (float)GetRandomValue(0, width()), (float)GetRandomValue(0, height()) };
        p.velocity = { (float)GetRandomValue(-100, 100) / 10.0f, (float)GetRandomValue(-100, 100) / 10.0f };
        p.radius = 3.0f;
        p.color = { (unsigned char)GetRandomValue(50, 255), (unsigned char)GetRandomValue(50, 255), 255, 255 };
        m_particles.push_back(p);
    }
}

void RaylibWidget::updatePhysics() {
    // Si en pause, on ne met pas à jour la physique
    if (m_isPaused) return;

    float dt = 1.0f / 60.0f;

    for (auto& p : m_particles) {
        p.velocity.y += m_gravity * dt * 10.0f;
        p.position.x += p.velocity.x;
        p.position.y += p.velocity.y;

        // Collisions
        if (p.position.y > height() - p.radius) {
            p.position.y = height() - p.radius;
            p.velocity.y *= -0.8f;
        }
        if (p.position.y < p.radius) {
            p.position.y = p.radius;
            p.velocity.y *= -0.8f;
        }
        if (p.position.x > width() - p.radius || p.position.x < p.radius) {
            p.velocity.x *= -0.8f;
            if (p.position.x > width() - p.radius) p.position.x = width() - p.radius;
            if (p.position.x < p.radius) p.position.x = p.radius;
        }
    }
}

void RaylibWidget::draw() {
    BeginDrawing();
    ClearBackground({ 20, 20, 30, 255 });

    for (const auto& p : m_particles) {
        DrawCircleV(p.position, p.radius, p.color);
    }

    DrawFPS(10, 10);
    if (m_isPaused) DrawText("PAUSE", width() / 2 - 50, height() / 2, 40, RAYWHITE);

    EndDrawing();
}

void RaylibWidget::paintEvent(QPaintEvent*) {
    if (!m_isInitialized) initRaylib();
    updatePhysics();
    draw();
    update();
}