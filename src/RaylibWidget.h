#pragma once
#include <QWidget>
#include <vector>
#include <raylib.h>
#include <chrono>
#include <QMouseEvent> // Nécessaire pour les événements souris
#include "Particle.h"

class RaylibWidget : public QWidget {
    Q_OBJECT
public:
    explicit RaylibWidget(QWidget* parent = nullptr);
    ~RaylibWidget();

    // Commande simulation
    void togglePause();
    void reset();

    // Physique Globale
    void setGravity(float g);
    void setFriction(float f);
    void setrebond(float r);
    void setInitialVelocityScale(float v);
    void setParticleSize(float s);
    void setParticleCount(int count);

    // --- NOUVEAU : Interaction Curseur ---
    void setCursorActive(bool active);
    void setCursorRadius(float radius);
    void setCursorStrength(float strength);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    // --- NOUVEAU : Capture de la souris ---
    void mouseMoveEvent(QMouseEvent* event) override;

private:
    void initRaylib();
    void initParticles();
    void updatePhysics();
    void drawToTexture();

    bool m_isInitialized = false;
    bool m_isPaused = false;

    RenderTexture2D m_renderTexture;
    std::vector<Particle> m_particles;

    // Valeurs de base pour la physique
    float m_gravity = 9.81f;
    float m_friction = 0.05f;
    float m_rebond = 0.7f;
    float m_velocityScale = 1.0f;
    float m_particleRadius = 3.0f;
    int m_targetCount = 1000;

    // Variables Interaction Curseur
    Vector2 m_mousePos = { -1000.0f, -1000.0f }; // Hors écran par défaut
    bool m_cursorActive = false;
    float m_cursorEffectRadius = 150.0f;
    float m_cursorEffectStrength = 0.0f; // 0 = Neutre

    // Variables calcul FPS
    std::chrono::steady_clock::time_point m_lastTime;
    int m_currentFPS = 0;
};