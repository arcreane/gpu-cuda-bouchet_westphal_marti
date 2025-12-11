#pragma once
#include <QWidget>
#include <vector>
#include <raylib.h>
#include "Particle.h"

class RaylibWidget : public QWidget {
    Q_OBJECT
public:
    explicit RaylibWidget(QWidget* parent = nullptr);
    ~RaylibWidget();

    // Méthode pour mettre à jour les paramètres depuis Qt
    void setGravity(float g);
	void togglePause(); // Met en pause ou reprend la simulation
	void reset();       // Réinitialise les particules

protected:
    // Surcharge pour intégrer le rendu custom
    void paintEvent(QPaintEvent* event) override;
    QPaintEngine* paintEngine() const override { return nullptr; }

private:
    void initRaylib();
    void initParticles();
    void updatePhysics(); // Calcul CPU
    void draw();          // Rendu Raylib
    bool m_isInitialized = false;
    bool m_isPaused = false;

    // Données de simulation
    std::vector<Particle> m_particles;
    float m_gravity = 9.81f;
    const int PARTICLE_COUNT = 1000;
};