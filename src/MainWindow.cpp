#include "MainWindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QComboBox>

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

    // Groupe Simulation
    QGroupBox* grpSim = new QGroupBox("Simulation", this);
    QVBoxLayout* laySim = new QVBoxLayout(grpSim);

	    // Boutons Play et Reset
    QPushButton* btnPlay = new QPushButton("Play / Pause", this);
    QPushButton* btnReset = new QPushButton("Reset", this);

	    // ComboBox pour le mode de calcul (CPU/GPU)
    m_comboComputeMode = new QComboBox(this);
    m_comboComputeMode->addItem("CPU");
    m_comboComputeMode->addItem("GPU (CUDA)");

        // Ajout des boutons play et reset au layout
    laySim->addWidget(btnPlay);
    laySim->addWidget(btnReset);
	laySim->addWidget(m_comboComputeMode);

    controlsLayout->addWidget(grpSim); // On l'ajoute en premier

    // 2. Groupe Physique
    QGroupBox* grpPhys = new QGroupBox("Physique and parameters", this);
    QVBoxLayout* layPhys = new QVBoxLayout(grpPhys);

        // -- Nombre de particules (0 à 10000) --
    m_lblCount = new QLabel("Count: 1000", this);
    m_sliderCount = new QSlider(Qt::Horizontal, this);
    m_sliderCount->setRange(0, 10000);
    m_sliderCount->setValue(1000);
    layPhys->addWidget(m_lblCount);
    layPhys->addWidget(m_sliderCount);

        // -- Taille (1.0 à 10.0) --
    m_lblSize = new QLabel("Size: 3.0", this);
    m_sliderSize = new QSlider(Qt::Horizontal, this);
    m_sliderSize->setRange(10, 100);
    m_sliderSize->setValue(30);
    layPhys->addWidget(m_lblSize);
    layPhys->addWidget(m_sliderSize);

	    // -- Gravité --
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

        // -- Viscosité (Friction) --
        // Echelle 0-100 pour représenter 0.00 à 1.00
    m_lblFriction = new QLabel("Viscosity : 0.05", this);
    m_sliderFriction = new QSlider(Qt::Horizontal, this);
    m_sliderFriction->setRange(0, 100);
    m_sliderFriction->setValue(5); // Valeur par défaut 0.05
    layPhys->addWidget(m_lblFriction);
    layPhys->addWidget(m_sliderFriction);

        // -- rebond (Rebond) --
        // Echelle 0-100 pour 0.0 à 1.0
    m_lblrebond = new QLabel("Bounciness : 0.70", this);
    m_sliderrebond = new QSlider(Qt::Horizontal, this);
    m_sliderrebond->setRange(0, 100);
    m_sliderrebond->setValue(70);
    layPhys->addWidget(m_lblrebond);
    layPhys->addWidget(m_sliderrebond);

        // -- Vitesse Initiale --
        // Echelle 0-200% (Multiplicateur)
    m_lblSpeed = new QLabel("Initial speed: 100%", this);
    m_sliderSpeed = new QSlider(Qt::Horizontal, this);
    m_sliderSpeed->setRange(0, 500); // 0 à 5x
    m_sliderSpeed->setValue(100);
    layPhys->addWidget(m_lblSpeed);
    layPhys->addWidget(m_sliderSpeed);

    controlsLayout->addWidget(grpPhys); // On l'ajoute en second
    controlsLayout->addStretch();

    // --- Fenêtre de rendu Raylib ---
    m_renderWidget = new RaylibWidget(this);

    mainLayout->addWidget(controlsWidget);
    mainLayout->addWidget(m_renderWidget);


    //------------------------------------------------------
    //                      Connexions
    //------------------------------------------------------

        // Bouton Play/Pause
    connect(btnPlay, &QPushButton::clicked, this, [this]() {
        if (m_renderWidget) m_renderWidget->togglePause();
        });

        // Bouton Reset
    connect(btnReset, &QPushButton::clicked, this, [this]() {
        if (m_renderWidget) m_renderWidget->reset();
        });

	    // ComboBox Mode de calcul CPU / GPU (Lambda)
    connect(m_comboComputeMode, &QComboBox::currentIndexChanged, this, [this](int index) {
        if (m_renderWidget) {
            // Index 0 = CPU, Index 1 = GPU
            RaylibWidget::ComputeMode mode = (index == 0)
                ? RaylibWidget::CPU
                : RaylibWidget::GPU;
            m_renderWidget->setComputeMode(mode);
        }
        });

        // Viscosité (Lambda)
    connect(m_sliderFriction, &QSlider::valueChanged, this, [this](int val) {
        float f = val / 100.0f;
        m_lblFriction->setText(QString("Viscosity: %1").arg(f, 0, 'f', 2));
        if (m_renderWidget) m_renderWidget->setFriction(f);
        });

        // rebond (Lambda)
    connect(m_sliderrebond, &QSlider::valueChanged, this, [this](int val) {
        float r = val / 100.0f;
        m_lblrebond->setText(QString("Bounciness: %1").arg(r, 0, 'f', 2));
        if (m_renderWidget) m_renderWidget->setrebond(r);
        });

        // Vitesse Initiale (Lambda)
    connect(m_sliderSpeed, &QSlider::valueChanged, this, [this](int val) {
        m_lblSpeed->setText(QString("Initial speed: %1%").arg(val));
        // On divise par 10 pour avoir un facteur (ex: 100 -> 10.0f) ou juste val
        // Ici on envoie le pourcentage directement, on traitera dans RaylibWidget
        if (m_renderWidget) m_renderWidget->setInitialVelocityScale(val / 100.0f);
        });

        // Slider Gravité
    connect(m_sliderGravity, &QSlider::valueChanged, this, &MainWindow::onGravityChanged);

        // Taille
    connect(m_sliderSize, &QSlider::valueChanged, this, [this](int val) {
        float s = val / 10.0f;
        m_lblSize->setText(QString("Size: %1").arg(s, 0, 'f', 1));
        if (m_renderWidget) m_renderWidget->setParticleSize(s);
        });

        // Nombre
    connect(m_sliderCount, &QSlider::valueChanged, this, [this](int val) {
        m_lblCount->setText(QString("Count: %1").arg(val));
        if (m_renderWidget) m_renderWidget->setParticleCount(val);
        });
}

MainWindow::~MainWindow() {}

// Slot appelé lorsque le slider de gravité change
void MainWindow::onGravityChanged(int value) {
    float g = value / 100.0f;
    m_lblGravity->setText(QString("Gravity: %1").arg(g, 0, 'f', 2));
    if (m_renderWidget) m_renderWidget->setGravity(g);
}