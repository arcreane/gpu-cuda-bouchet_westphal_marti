#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include "RaylibWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onGravityChanged(int value);

private:
    RaylibWidget* m_renderWidget;

	// Gravité
    QSlider* m_sliderGravity;
    QLabel* m_lblGravity;

    // Viscosité (Friction)
    QSlider* m_sliderFriction;
    QLabel* m_lblFriction;

    // rebond (Rebond)
    QSlider* m_sliderrebond;
    QLabel* m_lblrebond;

    // Vitesse Initiale
    QSlider* m_sliderSpeed;
    QLabel* m_lblSpeed;

    // Taille des particules
    QSlider* m_sliderSize;
    QLabel* m_lblSize;

    // Nombre de particules
    QSlider* m_sliderCount;
    QLabel* m_lblCount;
};