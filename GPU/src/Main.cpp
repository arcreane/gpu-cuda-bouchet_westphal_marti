#include <QApplication>
#include "MainWindow.h"

int main(int argc, char* argv[]) {
    // 1. Initialise le système Qt
    QApplication app(argc, argv);

    // 2. Crée et affiche notre fenêtre
    MainWindow window;
    window.show();

    // 3. Lance la boucle d'événements (attente des clics, etc.)
    return app.exec();
}