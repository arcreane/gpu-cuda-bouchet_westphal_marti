#include "MainWindow.h"
#include <QGroupBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Controls (Qt6)");
    resize(640 , 480); // Fenêtre verticale à gauche

    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout* mainLayout = new QVBoxLayout(central);

    // --- Groupe : Simulation ---
    QGroupBox* grpSim = new QGroupBox("Simulation state", this);
    QVBoxLayout* laySim = new QVBoxLayout(grpSim);

    m_btnPlayPause = new QPushButton("Play", this);
    m_btnReset = new QPushButton("Refresh", this);

    laySim->addWidget(m_btnPlayPause);
    laySim->addWidget(m_btnReset);
    mainLayout->addWidget(grpSim);

    // --- Groupe : Physique ---
    QGroupBox* grpPhys = new QGroupBox("Physics parameters", this);
    QVBoxLayout* layPhys = new QVBoxLayout(grpPhys);

    // Gravité
    m_lblGravity = new QLabel("Gravity: 9.81", this);
    m_sliderGravity = new QSlider(Qt::Horizontal, this);
    m_sliderGravity->setRange(0, 200); // 0.0 à 20.0 (divisé par 10)
    m_sliderGravity->setValue(98);

    layPhys->addWidget(m_lblGravity);
    layPhys->addWidget(m_sliderGravity);
    mainLayout->addWidget(grpPhys);

    // Connections
    connect(m_btnPlayPause, &QPushButton::clicked, this, &MainWindow::onPlayPauseClicked);
    connect(m_sliderGravity, &QSlider::valueChanged, this, &MainWindow::onGravityChanged);

    mainLayout->addStretch();
}

MainWindow::~MainWindow() {}

void MainWindow::onPlayPauseClicked() {
    m_isRunning = !m_isRunning;
    m_btnPlayPause->setText(m_isRunning ? "Pause" : "Play");
}

void MainWindow::onGravityChanged(int value) {
    float g = value / 10.0f;
    m_lblGravity->setText(QString("Gravity : %1").arg(g));
}

int MainWindow::getParticleCount() const { return 1000; } // Placeholder
float MainWindow::getGravity() const { return m_sliderGravity->value() / 10.0f; }
bool MainWindow::isSimulationRunning() const { return m_isRunning; }