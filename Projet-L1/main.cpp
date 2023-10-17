#include "notes.h"
#include "accueil.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    accueil w;
    w.setWindowTitle("Gestion des notes");
    w.show();

    return a.exec();
}
