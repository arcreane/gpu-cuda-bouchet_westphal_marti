#include "MainWindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QPushButton>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Simulateur Hybride (Qt + Raylib)");
    resize(1200, 800);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout* mainLayout = new QHBoxLayout(central);

    // --- Colonne de Gauche : Contrôles ---
    QWidget* controlsWidget = new QWidget(this);
    controlsWidget->setFixedWidth(320); // Un peu plus large pour les nouveaux contrôles
    QVBoxLayout* controlsLayout = new QVBoxLayout(controlsWidget);

    // 1. Groupe Simulation
    QGroupBox* grpSim = new QGroupBox("Simulation", this);
    QVBoxLayout* laySim = new QVBoxLayout(grpSim);

    QPushButton* btnPlay = new QPushButton("Play / Pause", this);
    QPushButton* btnReset = new QPushButton("Reset", this);
    laySim->addWidget(btnPlay);
    laySim->addWidget(btnReset);
    controlsLayout->addWidget(grpSim);

    // 2. Groupe Interaction Curseur (NOUVEAU)
    QGroupBox* grpCursor = new QGroupBox("Interaction Souris", this);
    QVBoxLayout* layCursor = new QVBoxLayout(grpCursor);

    // Activation
    m_chkCursorActive = new QCheckBox("Activer l'effet", this);
    layCursor->addWidget(m_chkCursorActive);

    // Force (Négatif = Repousse, Positif = Attire)
    m_lblCursorStrength = new QLabel("Force: 0 (Neutre)", this);
    m_sliderCursorStrength = new QSlider(Qt::Horizontal, this);
    m_sliderCursorStrength->setRange(-200, 200);
    m_sliderCursorStrength->setValue(0);
    layCursor->addWidget(m_lblCursorStrength);
    layCursor->addWidget(m_sliderCursorStrength);

    // Rayon
    m_lblCursorRadius = new QLabel("Rayon: 150 px", this);
    m_sliderCursorRadius = new QSlider(Qt::Horizontal, this);
    m_sliderCursorRadius->setRange(50, 600);
    m_sliderCursorRadius->setValue(150);
    layCursor->addWidget(m_lblCursorRadius);
    layCursor->addWidget(m_sliderCursorRadius);

    controlsLayout->addWidget(grpCursor);


    // 3. Groupe Physique
    QGroupBox* grpPhys = new QGroupBox("Physique et Paramètres", this);
    QVBoxLayout* layPhys = new QVBoxLayout(grpPhys);

    m_lblCount = new QLabel("Count: 1000", this);
    m_sliderCount = new QSlider(Qt::Horizontal, this);
    m_sliderCount->setRange(0, 10000);
    m_sliderCount->setValue(1000);
    layPhys->addWidget(m_lblCount);
    layPhys->addWidget(m_sliderCount);

    m_lblSize = new QLabel("Size: 3.0", this);
    m_sliderSize = new QSlider(Qt::Horizontal, this);
    m_sliderSize->setRange(10, 100);
    m_sliderSize->setValue(30);
    layPhys->addWidget(m_lblSize);
    layPhys->addWidget(m_sliderSize);

    m_lblGravity = new QLabel("Gravity: 9.81", this);
    m_sliderGravity = new QSlider(Qt::Horizontal, this);
    m_sliderGravity->setRange(0, 2500);
    m_sliderGravity->setValue(981);
    layPhys->addWidget(m_lblGravity);
    layPhys->addWidget(m_sliderGravity);

    m_lblFriction = new QLabel("Viscosity : 0.05", this);
    m_sliderFriction = new QSlider(Qt::Horizontal, this);
    m_sliderFriction->setRange(0, 100);
    m_sliderFriction->setValue(5);
    layPhys->addWidget(m_lblFriction);
    layPhys->addWidget(m_sliderFriction);

    m_lblrebond = new QLabel("Bounciness : 0.70", this);
    m_sliderrebond = new QSlider(Qt::Horizontal, this);
    m_sliderrebond->setRange(0, 100);
    m_sliderrebond->setValue(70);
    layPhys->addWidget(m_lblrebond);
    layPhys->addWidget(m_sliderrebond);

    m_lblSpeed = new QLabel("Initial speed: 100%", this);
    m_sliderSpeed = new QSlider(Qt::Horizontal, this);
    m_sliderSpeed->setRange(0, 500);
    m_sliderSpeed->setValue(100);
    layPhys->addWidget(m_lblSpeed);
    layPhys->addWidget(m_sliderSpeed);

    controlsLayout->addWidget(grpPhys);
    controlsLayout->addStretch();

    // --- Fenêtre de rendu Raylib ---
    m_renderWidget = new RaylibWidget(this);
    mainLayout->addWidget(controlsWidget);
    mainLayout->addWidget(m_renderWidget);


    // --- CONNEXIONS ---

    // Boutons Play/Reset
    connect(btnPlay, &QPushButton::clicked, this, [this]() {
        if (m_renderWidget) m_renderWidget->togglePause();
        });
    connect(btnReset, &QPushButton::clicked, this, [this]() {
        if (m_renderWidget) m_renderWidget->reset();
        });

    // --- NOUVEAU : Connexions Curseur ---
    connect(m_chkCursorActive, &QCheckBox::toggled, this, [this](bool checked) {
        if (m_renderWidget) m_renderWidget->setCursorActive(checked);
        });

    connect(m_sliderCursorStrength, &QSlider::valueChanged, this, [this](int val) {
        // Val est entre -200 et 200. On divise par 50.0 pour avoir une force entre -4.0 et +4.0
        float strength = val / 50.0f;

        QString type;
        if (val > 0) type = "Attraction";
        else if (val < 0) type = "Repulsion";
        else type = "Neutre";

        m_lblCursorStrength->setText(QString("Force: %1 (%2)").arg(strength, 0, 'f', 2).arg(type));

        if (m_renderWidget) m_renderWidget->setCursorStrength(strength);
        });

    connect(m_sliderCursorRadius, &QSlider::valueChanged, this, [this](int val) {
        m_lblCursorRadius->setText(QString("Rayon: %1 px").arg(val));
        if (m_renderWidget) m_renderWidget->setCursorRadius((float)val);
        });
    // ------------------------------------


    // Params Physique
    connect(m_sliderFriction, &QSlider::valueChanged, this, [this](int val) {
        float f = val / 100.0f;
        m_lblFriction->setText(QString("Viscosity: %1").arg(f, 0, 'f', 2));
        if (m_renderWidget) m_renderWidget->setFriction(f);
        });

    connect(m_sliderrebond, &QSlider::valueChanged, this, [this](int val) {
        float r = val / 100.0f;
        m_lblrebond->setText(QString("Bounciness: %1").arg(r, 0, 'f', 2));
        if (m_renderWidget) m_renderWidget->setrebond(r);
        });

    connect(m_sliderSpeed, &QSlider::valueChanged, this, [this](int val) {
        m_lblSpeed->setText(QString("Initial speed: %1%").arg(val));
        if (m_renderWidget) m_renderWidget->setInitialVelocityScale(val / 100.0f);
        });

    connect(m_sliderGravity, &QSlider::valueChanged, this, &MainWindow::onGravityChanged);

    connect(m_sliderSize, &QSlider::valueChanged, this, [this](int val) {
        float s = val / 10.0f;
        m_lblSize->setText(QString("Size: %1").arg(s, 0, 'f', 1));
        if (m_renderWidget) m_renderWidget->setParticleSize(s);
        });

    connect(m_sliderCount, &QSlider::valueChanged, this, [this](int val) {
        m_lblCount->setText(QString("Count: %1").arg(val));
        if (m_renderWidget) m_renderWidget->setParticleCount(val);
        });
}

MainWindow::~MainWindow() {}

void MainWindow::onGravityChanged(int value) {
    float g = value / 100.0f;
    m_lblGravity->setText(QString("Gravity: %1").arg(g, 0, 'f', 2));
    if (m_renderWidget) m_renderWidget->setGravity(g);
}