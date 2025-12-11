#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QVBoxLayout>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    // Accesseurs pour récupérer les valeurs depuis le main.cpp
    int getParticleCount() const;
    float getGravity() const;
    bool isSimulationRunning() const;

private:
    // Widgets
    QPushButton* m_btnPlayPause;
    QPushButton* m_btnReset;
    QSlider* m_sliderGravity;
    QSlider* m_sliderCount;
    QLabel* m_lblGravity;
    QLabel* m_lblCount;

    // État
    bool m_isRunning = false;

private slots:
    void onPlayPauseClicked();
    void onGravityChanged(int value);
};