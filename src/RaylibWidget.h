#pragma once
#include <QWidget>
#include <vector>
#include <raylib.h>
#include <chrono> // <--- AJOUT POUR LE TEMPS
#include "Particle.h"

class RaylibWidget : public QWidget {
    Q_OBJECT
public:
    explicit RaylibWidget(QWidget* parent = nullptr);
    ~RaylibWidget();

    void setGravity(float g);
    void togglePause();
    void reset();

protected:
    void paintEvent(QPaintEvent* event) override;

    // --- NOUVEAU : Gère le redimensionnement ---
    void resizeEvent(QResizeEvent* event) override;

private:
    void initRaylib();
    void initParticles();
    void updatePhysics();
    void drawToTexture();

    bool m_isInitialized = false;
    bool m_isPaused = false;

    RenderTexture2D m_renderTexture;
    std::vector<Particle> m_particles;
    float m_gravity = 9.81f;
    const int PARTICLE_COUNT = 1000;

    // --- NOUVEAU : Variables pour calculer les FPS ---
    std::chrono::steady_clock::time_point m_lastTime;
    int m_currentFPS = 0;
};