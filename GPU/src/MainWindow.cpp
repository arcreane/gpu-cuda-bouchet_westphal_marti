#include "MainWindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    // Configuration de la fenêtre principale
    setWindowTitle("Simulateur Particules");
    resize(400, 300);

}

MainWindow::~MainWindow() {
}