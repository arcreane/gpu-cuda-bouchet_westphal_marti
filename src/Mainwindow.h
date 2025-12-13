#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QCheckBox>
#include "RaylibWidget.h"
#include <QComboBox>


class QComboBox;


class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onGravityChanged(int value);

private:
    RaylibWidget* m_renderWidget;

	// Mode de calcul CPU / GPU
    QComboBox* m_comboComputeMode;

	// Gravité
    QSlider* m_sliderGravity;
    QLabel* m_lblGravity;

	// Viscosité
    QSlider* m_sliderFriction;
    QLabel* m_lblFriction;

	// Rebond
    QSlider* m_sliderrebond;
    QLabel* m_lblrebond;

	// Vitesse Initiale
    QSlider* m_sliderSpeed;
    QLabel* m_lblSpeed;

	// Taille des Particules
    QSlider* m_sliderSize;
    QLabel* m_lblSize;

	// Nombre de Particules
    QSlider* m_sliderCount;
    QLabel* m_lblCount;

    // Interaction Curseur
    QCheckBox* m_chkCursorActive;
    QSlider* m_sliderCursorStrength;
    QLabel* m_lblCursorStrength;

	// Rayon d'Action Curseur
    QSlider* m_sliderCursorRadius;
    QLabel* m_lblCursorRadius;
};