#include "RaylibWidget.h"
#include <QPainter>
#include <QImage>
#include <random>
#include <QResizeEvent>

RaylibWidget::RaylibWidget(QWidget* parent) : QWidget(parent) {
    // Optimisation : On indique à Qt qu'on dessine tout le fond nous-mêmes
    setAttribute(Qt::WA_OpaquePaintEvent);
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true); // Utile pour tes futures interactions souris
}

RaylibWidget::~RaylibWidget() {
    if (m_isInitialized) {
        // Nettoyage propre des ressources Raylib
        UnloadRenderTexture(m_renderTexture);
        CloseWindow();
    }
}

void RaylibWidget::initRaylib() {
    // 1. Configuration de Raylib en mode "Caché"
    // Raylib sert de moteur de calcul graphique, pas de fenêtre
    SetConfigFlags(FLAG_WINDOW_HIDDEN);

    // On doit initialiser une fenêtre pour avoir le contexte OpenGL
    InitWindow(width(), height(), "Raylib Renderer");

    // 2. Création du Framebuffer (Texture) à la taille du Widget
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

        // --- Gestion des Collisions (Murs) ---
        // Bas
        if (p.position.y > height() - p.radius) {
            p.position.y = height() - p.radius;
            p.velocity.y *= -0.8f;
        }
        // Haut
        if (p.position.y < p.radius) {
            p.position.y = p.radius;
            p.velocity.y *= -0.8f;
        }
        // Gauche / Droite
        if (p.position.x > width() - p.radius || p.position.x < p.radius) {
            p.velocity.x *= -0.8f;
            // Correction de position pour éviter que la particule colle au mur
            if (p.position.x > width() - p.radius) p.position.x = width() - p.radius;
            if (p.position.x < p.radius) p.position.x = p.radius;
        }
    }
}

void RaylibWidget::drawToTexture() {
    BeginTextureMode(m_renderTexture);

    ClearBackground({ 20, 20, 30, 255 });

    for (const auto& p : m_particles) {
        DrawCircleV(p.position, p.radius, p.color);
    }
 
    DrawText(TextFormat("%i FPS", m_currentFPS), 10, 10, 20, GREEN);

    if (m_isPaused) {
        DrawText("PAUSE", width() / 2 - 50, height() / 2, 40, RAYWHITE);
    }

    EndTextureMode();
}

void RaylibWidget::paintEvent(QPaintEvent*) {
    if (!m_isInitialized) {
        initRaylib();
        m_lastTime = std::chrono::steady_clock::now(); // Initialiser le chrono
    }

    // --- CALCUL FPS ---
    auto currentTime = std::chrono::steady_clock::now();
    // Calcul de la différence en millisecondes
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_lastTime).count();

    // Si au moins 1ms est passée (pour éviter la division par zéro)
    if (elapsed > 0) {
        m_currentFPS = 1000 / elapsed; // 1000ms / temps écoulé = Images par seconde
        m_lastTime = currentTime;
    }
    // ------------------

    updatePhysics();
    drawToTexture();

    Image image = LoadImageFromTexture(m_renderTexture.texture);
    QImage qimg((uchar*)image.data, image.width, image.height, QImage::Format_RGBA8888);
    QImage displayedImage = qimg.mirrored();

    QPainter painter(this);
    painter.drawImage(0, 0, displayedImage);

    UnloadImage(image);
    update();
}

void RaylibWidget::resizeEvent(QResizeEvent* event) {
    // On appelle l'implémentation de base de Qt
    QWidget::resizeEvent(event);

    // Si Raylib est prêt, on recrée la texture à la nouvelle taille
    if (m_isInitialized) {
        UnloadRenderTexture(m_renderTexture);
        m_renderTexture = LoadRenderTexture(width(), height());
    }
}

void RaylibWidget::setFriction(float f) {
    m_friction = f;
}

void RaylibWidget::setrebond(float r) {
    m_rebond = r;
}

void RaylibWidget::setInitialVelocityScale(float v) {
    m_velocityScale = v;
}