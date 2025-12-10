#pragma once
#include <QMainWindow>
#include <QPushButton>
#include <raylib.h>

class MainWindow : public QMainWindow {
    Q_OBJECT // Macro obligatoire pour que Qt fonctionne

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    QPushButton* m_boutonTest; // Un simple bouton pour l'exemple
};