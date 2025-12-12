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

		//Position aléatoire dans la fenêtre
        p.position = { (float)GetRandomValue(0, width()), (float)GetRandomValue(0, height()) };

        // On génère une vitesse de base aléatoire
        float vx = (float)GetRandomValue(-100, 100) / 10.0f;
        float vy = (float)GetRandomValue(-100, 100) / 10.0f;
		p.velocity = { vx * m_velocityScale, vy * m_velocityScale };


        p.radius = 5.0f;
        p.color = { (unsigned char)GetRandomValue(50, 255), (unsigned char)GetRandomValue(50, 255), 255, 255 };
        m_particles.push_back(p);
    }
}

void RaylibWidget::updatePhysics() {
    // Si en pause, on ne met pas à jour la physique
    if (m_isPaused) return;

	// Delta Time fixe pour la simulation
    float dt = 1.0f / 60.0f;

    for (auto& p : m_particles) {
        p.velocity.y += m_gravity * dt * 10.0f;
        p.position.x += p.velocity.x;
        p.position.y += p.velocity.y;

        // 2. Friction (Viscosité)
         // Formule : v = v * (1 - coefficient)
         // On applique une réduction proportionnelle au temps écoulé
        float damping = 1.0f - (m_friction * dt * 2.0f);
        // *2.0f est un facteur arbitraire pour rendre l'effet du slider plus visible
        if (damping < 0) damping = 0;

        p.velocity.x *= damping;
        p.velocity.y *= damping;

        // 3. Application du mouvement
        p.position.x += p.velocity.x;
        p.position.y += p.velocity.y;

        // 4. Collisions Murs (Avec m_rebond)
        // Bas
        if (p.position.y > height() - p.radius) {
            p.position.y = height() - p.radius;
            p.velocity.y *= -m_rebond; // Rebond dynamique
        }
        // Haut
        if (p.position.y < p.radius) {
            p.position.y = p.radius;
            p.velocity.y *= -m_rebond;
        }
        // Gauche / Droite
        if (p.position.x > width() - p.radius || p.position.x < p.radius) {
            p.velocity.x *= -m_rebond;

            // Correction de position
            if (p.position.x > width() - p.radius) p.position.x = width() - p.radius;
            if (p.position.x < p.radius) p.position.x = p.radius;
        }
    }

    // --- B. BOUCLE DE COLLISION INTER-PARTICULES (Naïve O(N^2)) ---
    // Note : C'est très lourd pour le CPU. C'est là que CUDA brillera plus tard.
    for (size_t i = 0; i < m_particles.size(); i++) {
        for (size_t j = i + 1; j < m_particles.size(); j++) {
            Particle& p1 = m_particles[i];
            Particle& p2 = m_particles[j];

            float dx = p2.position.x - p1.position.x;
            float dy = p2.position.y - p1.position.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            float minDistance = p1.radius + p2.radius;

            // Si collision détectée
            if (distance < minDistance && distance > 0.0001f) {
                // 1. Calcul de la normale et de la tangente
                float nx = dx / distance;
                float ny = dy / distance;

                // 2. Séparation des particules (pour ne pas qu'elles s'agglutinent)
                float overlap = minDistance - distance;
                float moveX = nx * overlap * 0.5f;
                float moveY = ny * overlap * 0.5f;

                p1.position.x -= moveX;
                p1.position.y -= moveY;
                p2.position.x += moveX;
                p2.position.y += moveY;

                // 3. Réponse élastique (Echange d'impulsion)
                // Vitesse relative
                float dvx = p2.velocity.x - p1.velocity.x;
                float dvy = p2.velocity.y - p1.velocity.y;

                // Produit scalaire vitesse relative . normale
                float dotProduct = dvx * nx + dvy * ny;

                // Si les particules s'éloignent déjà, on ne fait rien
                if (dotProduct > 0) continue;

                // Calcul de l'impulsion scalaire
                // On utilise la moyenne des coefficients de restitution des deux particules
                float currentRestitution = m_rebond;
                float impulseScale = -(1.0f + currentRestitution) * dotProduct;

                // On divise par la somme des masses inverses (ici masse = 1 pour tout le monde)
                impulseScale /= 2.0f;

                // Application de l'impulsion
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

    for (const auto& p : m_particles) {
        DrawCircleV(p.position, p.radius, p.color);
    }
    
    // Affichage FPS
    DrawText(TextFormat("%i FPS", m_currentFPS), 10, 10, 20, GREEN);
    //Affichage nombre particules
    DrawText(TextFormat("Count: %i", m_particles.size()), 10, 30, 20, LIGHTGRAY);

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
    if (m_velocityScale <= 0.0001f) {
        m_velocityScale = v;
        return;
    }

	// calcul du ratio de changements
    float ratio = v / m_velocityScale;

	// Mide à jour de la valeur stockée
    m_velocityScale = v;

	// Application au vecteur vitesse de chaque particule
    for (auto& p : m_particles) {
        p.velocity.x *= ratio;
        p.velocity.y *= ratio;
    }
}