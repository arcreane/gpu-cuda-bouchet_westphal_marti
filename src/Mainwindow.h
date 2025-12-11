#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include "RaylibWidget.h" // <--- AJOUT

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onGravityChanged(int value);

private:
    RaylibWidget* m_renderWidget; // <--- AJOUT

    // ... vos autres widgets existants
    QSlider* m_sliderGravity;
    QLabel* m_lblGravity;
};