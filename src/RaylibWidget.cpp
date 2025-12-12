#include "RaylibWidget.h"
#include <QPainter>
#include <QImage>
#include <random>
#include <QResizeEvent>
#include <cmath>

RaylibWidget::RaylibWidget(QWidget* parent) : QWidget(parent) {
    // Optimisation : On indique à Qt qu'on dessine tout le fond nous-mêmes
    setAttribute(Qt::WA_OpaquePaintEvent);
    setFocusPolicy(Qt::StrongFocus);

    // IMPORTANT : Permet de recevoir les mouvements de souris même sans cliquer
    setMouseTracking(true);
}

RaylibWidget::~RaylibWidget() {
    if (m_isInitialized) {
        UnloadRenderTexture(m_renderTexture);
        CloseWindow();
    }
}

// --- GESTION SOURIS ---
void RaylibWidget::mouseMoveEvent(QMouseEvent* event) {
    // Conversion des coordonnées Qt vers Raylib
    m_mousePos.x = (float)event->pos().x();
    m_mousePos.y = (float)event->pos().y();
}

void RaylibWidget::setCursorActive(bool active) { m_cursorActive = active; }
void RaylibWidget::setCursorRadius(float radius) { m_cursorEffectRadius = radius; }
void RaylibWidget::setCursorStrength(float strength) { m_cursorEffectStrength = strength; }
// ----------------------

void RaylibWidget::initRaylib() {
    SetConfigFlags(FLAG_WINDOW_HIDDEN);
    InitWindow(width(), height(), "Raylib Renderer");
    m_renderTexture = LoadRenderTexture(width(), height());
    m_isInitialized = true;
    initParticles();
}

void RaylibWidget::reset() {
    initParticles();
}

void RaylibWidget::togglePause() {
    m_isPaused = !m_isPaused;
}

void RaylibWidget::setGravity(float g) {
    m_gravity = g;
}

void RaylibWidget::initParticles() {
    m_particles.clear();
    for (int i = 0; i < m_targetCount; i++) {
        Particle p;
        p.position = { (float)GetRandomValue(0, width()), (float)GetRandomValue(0, height()) };

        float vx = (float)GetRandomValue(-100, 100) / 10.0f;
        float vy = (float)GetRandomValue(-100, 100) / 10.0f;
        p.velocity = { vx * m_velocityScale, vy * m_velocityScale };

        p.radius = m_particleRadius;
        p.color = { (unsigned char)GetRandomValue(50, 255), (unsigned char)GetRandomValue(50, 255), 255, 255 };
        m_particles.push_back(p);
    }
}

void RaylibWidget::updatePhysics() {
    if (m_isPaused) return;

    float dt = 1.0f / 60.0f;

    for (auto& p : m_particles) {
        // 1. Gravité
        p.velocity.y += m_gravity * dt * 10.0f;

        // --- INTERACTION CURSEUR ---
        if (m_cursorActive) {
            float dx = m_mousePos.x - p.position.x;
            float dy = m_mousePos.y - p.position.y;
            float distSq = dx * dx + dy * dy; // Distance au carré (plus rapide)
            float dist = std::sqrt(distSq);

            // Si la particule est dans le rayon d'action
            if (dist < m_cursorEffectRadius && dist > 1.0f) {
                float nx = dx / dist; // Vecteur normalisé
                float ny = dy / dist;

                // Force progressive : plus forte au centre (1.0) et nulle au bord (0.0)
                float forceFactor = (1.0f - (dist / m_cursorEffectRadius));

                // Application de la force (Force * Facteur * Puissance arbitraire)
                // Strength positif = Attraction, Négatif = Répulsion
                p.velocity.x += nx * forceFactor * m_cursorEffectStrength * 2.0f;
                p.velocity.y += ny * forceFactor * m_cursorEffectStrength * 2.0f;
            }
        }
        // ---------------------------

        // 2. Friction
        float damping = 1.0f - (m_friction * dt * 2.0f);
        if (damping < 0) damping = 0;

        p.velocity.x *= damping;
        p.velocity.y *= damping;

        // 3. Mouvement
        p.position.x += p.velocity.x;
        p.position.y += p.velocity.y;

        // 4. Murs
        if (p.position.y > height() - p.radius) {
            p.position.y = height() - p.radius;
            p.velocity.y *= -m_rebond;
        }
        if (p.position.y < p.radius) {
            p.position.y = p.radius;
            p.velocity.y *= -m_rebond;
        }
        if (p.position.x > width() - p.radius || p.position.x < p.radius) {
            p.velocity.x *= -m_rebond;
            if (p.position.x > width() - p.radius) p.position.x = width() - p.radius;
            if (p.position.x < p.radius) p.position.x = p.radius;
        }
    }

    // --- Collisions Inter-Particules (Limitée pour perfs) ---
    if (m_particles.size() > 2000) return;

    for (size_t i = 0; i < m_particles.size(); i++) {
        for (size_t j = i + 1; j < m_particles.size(); j++) {
            Particle& p1 = m_particles[i];
            Particle& p2 = m_particles[j];

            float dx = p2.position.x - p1.position.x;
            float dy = p2.position.y - p1.position.y;
            float distSq = dx * dx + dy * dy;
            float minDist = p1.radius + p2.radius;

            if (distSq < minDist * minDist && distSq > 0.0001f) {
                float distance = std::sqrt(distSq);
                float nx = dx / distance;
                float ny = dy / distance;

                float overlap = minDist - distance;
                float moveX = nx * overlap * 0.5f;
                float moveY = ny * overlap * 0.5f;

                p1.position.x -= moveX;
                p1.position.y -= moveY;
                p2.position.x += moveX;
                p2.position.y += moveY;

                float dvx = p2.velocity.x - p1.velocity.x;
                float dvy = p2.velocity.y - p1.velocity.y;
                float dotProduct = dvx * nx + dvy * ny;

                if (dotProduct > 0) continue;

                float impulseScale = -(1.0f + m_rebond) * dotProduct;
                impulseScale /= 2.0f;

                float impulseX = nx * impulseScale;
                float impulseY = ny * impulseScale;

                p1.velocity.x -= impulseX;
                p1.velocity.y -= impulseY;
                p2.velocity.x += impulseX;
                p2.velocity.y += impulseY;
            }
        }
    }
}

void RaylibWidget::drawToTexture() {
    BeginTextureMode(m_renderTexture);
    ClearBackground({ 20, 20, 30, 255 });

    // --- VISUALISATION CURSEUR ---
    if (m_cursorActive) {
        Color areaColor;
        // Vert si on attire, Rouge si on repousse
        if (m_cursorEffectStrength > 0) areaColor = { 0, 255, 0, 30 };
        else areaColor = { 255, 0, 0, 30 };

        // Cercle plein transparent
        DrawCircleV(m_mousePos, m_cursorEffectRadius, areaColor);
        // Contour blanc
        DrawCircleLines((int)m_mousePos.x, (int)m_mousePos.y, m_cursorEffectRadius, RAYWHITE);
    }
    // -----------------------------

    for (const auto& p : m_particles) {
        DrawCircleV(p.position, p.radius, p.color);
    }

    DrawText(TextFormat("%i FPS", m_currentFPS), 10, 10, 20, GREEN);
    DrawText(TextFormat("Count: %i", (int)m_particles.size()), 10, 30, 20, LIGHTGRAY);

    if (m_isPaused) {
        DrawText("PAUSE", width() / 2 - 50, height() / 2, 40, RAYWHITE);
    }

    EndTextureMode();
}

void RaylibWidget::paintEvent(QPaintEvent*) {
    if (!m_isInitialized) {
        initRaylib();
        m_lastTime = std::chrono::steady_clock::now();
    }

    auto currentTime = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_lastTime).count();

    if (elapsed > 0) {
        m_currentFPS = 1000 / elapsed;
        m_lastTime = currentTime;
    }

    updatePhysics();
    drawToTexture();

    Image image = LoadImageFromTexture(m_renderTexture.texture);
    // Raylib est inversé en Y par rapport à Qt, on utilise .mirrored()
    QImage qimg((uchar*)image.data, image.width, image.height, QImage::Format_RGBA8888);
    QImage displayedImage = qimg.mirrored();

    QPainter painter(this);
    painter.drawImage(0, 0, displayedImage);

    UnloadImage(image);
    update();
}

void RaylibWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (m_isInitialized) {
        UnloadRenderTexture(m_renderTexture);
        m_renderTexture = LoadRenderTexture(width(), height());
    }
}

void RaylibWidget::setParticleSize(float s) {
    m_particleRadius = s;
    for (auto& p : m_particles) p.radius = m_particleRadius;
}

void RaylibWidget::setParticleCount(int count) {
    m_targetCount = count;
    int currentSize = (int)m_particles.size();

    if (count < currentSize) {
        m_particles.resize(count);
    }
    else if (count > currentSize) {
        for (int i = 0; i < (count - currentSize); i++) {
            Particle p;
            p.position = { (float)GetRandomValue(0, width()), (float)GetRandomValue(0, height()) };
            float vx = (float)GetRandomValue(-100, 100) / 10.0f;
            float vy = (float)GetRandomValue(-100, 100) / 10.0f;
            p.velocity = { vx * m_velocityScale, vy * m_velocityScale };
            p.radius = m_particleRadius;
            p.color = { (unsigned char)GetRandomValue(50, 255), (unsigned char)GetRandomValue(50, 255), 255, 255 };
            m_particles.push_back(p);
        }
    }
}

void RaylibWidget::setFriction(float f) { m_friction = f; }
void RaylibWidget::setrebond(float r) { m_rebond = r; }

void RaylibWidget::setInitialVelocityScale(float v) {
    if (m_velocityScale <= 0.0001f) {
        m_velocityScale = v;
        return;
    }
    float ratio = v / m_velocityScale;
    m_velocityScale = v;
    for (auto& p : m_particles) {
        p.velocity.x *= ratio;
        p.velocity.y *= ratio;
    }
}