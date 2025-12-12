#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QCheckBox> // <--- Ajouté
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

    // --- Physique Generale ---
    QSlider* m_sliderGravity;
    QLabel* m_lblGravity;

    QSlider* m_sliderFriction;
    QLabel* m_lblFriction;

    QSlider* m_sliderrebond;
    QLabel* m_lblrebond;

    QSlider* m_sliderSpeed;
    QLabel* m_lblSpeed;

    QSlider* m_sliderSize;
    QLabel* m_lblSize;

    QSlider* m_sliderCount;
    QLabel* m_lblCount;

    // --- Interaction Curseur (NOUVEAU) ---
    QCheckBox* m_chkCursorActive;

    QSlider* m_sliderCursorStrength;
    QLabel* m_lblCursorStrength;

    QSlider* m_sliderCursorRadius;
    QLabel* m_lblCursorRadius;
};