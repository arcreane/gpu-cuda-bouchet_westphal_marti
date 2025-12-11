#include "MainWindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton> // <--- Ne pas oublier

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Simulateur Hybride (Qt + Raylib)");
    resize(1200, 800);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout* mainLayout = new QHBoxLayout(central);

    // --- Colonne de Gauche : Contrôles ---
    QWidget* controlsWidget = new QWidget(this);
    controlsWidget->setFixedWidth(300);
    QVBoxLayout* controlsLayout = new QVBoxLayout(controlsWidget);

    // 1. Groupe Simulation (Play/Reset) - EN HAUT
    QGroupBox* grpSim = new QGroupBox("Simulation", this);
    QVBoxLayout* laySim = new QVBoxLayout(grpSim);

    QPushButton* btnPlay = new QPushButton("Play / Pause", this);
    QPushButton* btnReset = new QPushButton("Reset", this);

        // Ajout des boutons play et reset au layout
    laySim->addWidget(btnPlay);
    laySim->addWidget(btnReset);

    controlsLayout->addWidget(grpSim); // On l'ajoute en premier

    // 2. Groupe Physique
    QGroupBox* grpPhys = new QGroupBox("Physique", this);
    QVBoxLayout* layPhys = new QVBoxLayout(grpPhys);

	    //slider Gravité
    m_lblGravity = new QLabel("Gravity: 9.81", this);
    m_sliderGravity = new QSlider(Qt::Horizontal, this);
	// Mapper un slider de 0 à 2500 pour représenter 0.00 à 25.00
    m_sliderGravity->setRange(0, 2500);
	// Valeur de départ à 9.81 = gravité terrestre
    m_sliderGravity->setValue(981);
        
	    // Ajout des contrôles de physique au layout
    layPhys->addWidget(m_lblGravity);
    layPhys->addWidget(m_sliderGravity);

    controlsLayout->addWidget(grpPhys); // On l'ajoute en second
    controlsLayout->addStretch();

    // --- Fenêtre de rendu Raylib ---
    m_renderWidget = new RaylibWidget(this);

    mainLayout->addWidget(controlsWidget);
    mainLayout->addWidget(m_renderWidget);

    // --- Connexions ---

    // Slider Gravité
    connect(m_sliderGravity, &QSlider::valueChanged, this, &MainWindow::onGravityChanged);

    // Bouton Play/Pause
    connect(btnPlay, &QPushButton::clicked, this, [this]() {
        if (m_renderWidget) m_renderWidget->togglePause();
        });

    // Bouton Reset
    connect(btnReset, &QPushButton::clicked, this, [this]() {
        if (m_renderWidget) m_renderWidget->reset();
        });
}

MainWindow::~MainWindow() {}

// Slot appelé lorsque le slider de gravité change
void MainWindow::onGravityChanged(int value) {
    float g = value / 100.0f;
    m_lblGravity->setText(QString("Gravity: %1").arg(g, 0, 'f', 2));
    if (m_renderWidget) m_renderWidget->setGravity(g);
}