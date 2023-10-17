#ifndef NOTES_H
#define NOTES_H

#include <QDialog>
#include "accueil.h"

namespace Ui {
class notes;
}

class notes : public QDialog
{
    Q_OBJECT

public:
    explicit notes(QWidget *parent = 0);
    ~notes();

    void calculMoyenne();

private slots:
    void on_btnAjouter_clicked();

    void on_btnModifier_clicked();

    void on_btnSupprimer_clicked();

    void on_btnRetour_clicked();

    void mettreAJourListeNotes();
    
private:
    Ui::notes *ui;
};

#endif // NOTES_H
