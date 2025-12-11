#include "RaylibWidget.h"
#include <QResizeEvent>

RaylibWidget::RaylibWidget(QWidget* parent) : QWidget(parent) {
    // On demande à Qt de nous laisser dessiner nous-mêmes
    setAttribute(Qt::WA_PaintOnScreen);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);

    // On force le focus pour capter le clavier plus tard
    setFocusPolicy(Qt::StrongFocus);
}

RaylibWidget::~RaylibWidget() {
    if (m_isInitialized) {
        CloseWindow();
    }
}

void RaylibWidget::initRaylib() {
    // L'astuce magique : On initialise Raylib sur la fenêtre native de ce Widget
    // (WId est le handle fenêtre Windows/Linux)
    InitWindow(width(), height(), "");

    // IMPORTANT : Sur certaines versions de Raylib/Qt, il faut attacher le contexte
    // Ici on fait simple pour commencer : InitWindow crée un contexte OpenGL.
    // Dans une intégration avancée, on utiliserait le handle winId().

    SetTargetFPS(60);
    m_isInitialized = true;
}

void RaylibWidget::paintEvent(QPaintEvent*) {
    if (!m_isInitialized) {
        // On initialise Raylib seulement quand la fenêtre est visible à l'écran
        // InitWindow(width(), height(), "Raylib View"); // Version standard
        // Pour l'intégration, on va feinter : on initialise une fois.

        // NOTE : L'intégration parfaite Raylib-in-Qt est complexe.
        // Pour ce projet étudiant, on va souvent ouvrir une fenêtre Raylib "enfant" 
        // ou simplement synchroniser les positions.

        // Approche simplifiée recommandée pour ce TP :
        // On ne va PAS embedder Raylib *dans* le widget pixel-perfect tout de suite 
        // car cela demande de recompiler Raylib.
        // On va lancer la fenêtre Raylib et la coller à côté ou gérer la boucle.

        // Mais essayons l'initialisation standard :
        InitWindow(800, 600, "Rendu Particules");
        SetTargetFPS(60);
        m_isInitialized = true;
    }

    // Boucle de rendu Raylib (Une frame)
    if (m_isInitialized && !WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("Rendu Raylib Hybride", 10, 10, 20, DARKGRAY);
        DrawCircle(width() / 2, height() / 2, 50, MAROON);
        DrawFPS(10, 40);

        EndDrawing();
    }

    // On force Qt à redessiner en boucle pour animer (comme une game loop)
    update();
}