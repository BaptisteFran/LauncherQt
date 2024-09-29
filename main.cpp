#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{

    QApplication app(argc, argv);
    MainWindow window;

    // Désactiver les boutons de fermeture, redimensionnement, etc.
    window.setWindowFlags(Qt::Window | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    // Rendre la fenêtre non redimensionnable
    window.setFixedSize(window.size());

    window.show();
    return app.exec();
}
