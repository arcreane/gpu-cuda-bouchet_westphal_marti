#include <QApplication>
#include "MainWindow.h"

// Cette ligne exporte un symbole que le driver NVIDIA recherche au démarrage.
// Si elle est presente et vaut 1, l'application se lance sur la carte dédiée.
extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}

// Optionnel : La même chose pour les cartes AMD (au cas où)
extern "C" {
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}


int main(int argc, char* argv[]) {
    // 1. Initialise le système Qt
    QApplication app(argc, argv);

    // 2. Crée et affiche notre fenêtre
    MainWindow window;
    window.show();

    // 3. Lance la boucle d'événements (attente des clics, etc.)
    return app.exec();
}