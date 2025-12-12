#pragma once
#include <QWidget>
#include <vector>
#include <raylib.h>
#include <chrono>
#include "Particle.h"

class RaylibWidget : public QWidget {
    Q_OBJECT
public:
	//selecteur CPU / GPU
    enum ComputeMode {      
        CPU,
        GPU};
	// Constructeur / Destructeur
    explicit RaylibWidget(QWidget* parent = nullptr);
    ~RaylibWidget();


    // Commande simulation
    void togglePause();
    void reset();
    void setComputeMode(ComputeMode mode);

    // Physique
    void setGravity(float g);
    void setFriction(float f);
    void setrebond(float r);
    void setInitialVelocityScale(float v);
    void setParticleSize(float s);
    void setParticleCount(int count);

	

protected:
    void paintEvent(QPaintEvent* event) override;
    // --- Redimensionnement ---
    void resizeEvent(QResizeEvent* event) override;

private:
    void initRaylib();
    void initParticles();
    void updatePhysics();
    void drawToTexture();

    bool m_isInitialized = false;
    bool m_isPaused = false;

	//variable mode CPU / GPU
    ComputeMode m_computeMode = CPU;

    RenderTexture2D m_renderTexture;
    std::vector<Particle> m_particles;

	// Valeur de base pour la physique
    float m_gravity = 9.81f;
    float m_friction = 0.05f;
    float m_rebond = 0.7f;   
    float m_velocityScale = 1.0f;
    float m_particleRadius = 3.0f;
    int m_targetCount = 1000;

    // Variables calcul FPS
    std::chrono::steady_clock::time_point m_lastTime;
    int m_currentFPS = 0;
};