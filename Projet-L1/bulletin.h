#ifndef BULLETIN_H
#define BULLETIN_H

#include <QDialog>

namespace Ui {
class bulletin;
}

class bulletin : public QDialog
{
    Q_OBJECT

public:
    explicit bulletin(QWidget *parent = 0);
    ~bulletin();

public:
    void afficherBulletin(const QString& nom, const QString& age, const QString& niveau, const QString& num, const float& moyenne, const QList<QPair<QString, float>>& notes);


private slots:
    void on_btnRetour_clicked();
    
private:
    Ui::bulletin *ui;
};

#endif // BULLETIN_H
