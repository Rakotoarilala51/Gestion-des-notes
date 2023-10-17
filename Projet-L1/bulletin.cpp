#include "bulletin.h"
#include "ui_bulletin.h"
#include <QStandardItem>

bulletin::bulletin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::bulletin)
{
    ui->setupUi(this);
}

bulletin::~bulletin()
{
    delete ui;
}

void bulletin::on_btnRetour_clicked()
{
    this->close();
}

void bulletin::afficherBulletin(const QString& nom, const QString& age, const QString& niveau, const QString& num, const float& moyenne, const QList<QPair<QString, float>>& notes)
{
    ui->lblNom->setText(nom);
    ui->lblAge->setText(age);
    ui->lblNiveau->setText(niveau);
    ui->lblNumero->setText(num);
    ui->lblMoyenne->setText(QString::number(moyenne, 'f', 2));

    QString observation;

    if (moyenne >= 10) {
        observation = "Admis";
    }
    else {
        observation = "Ajourné";
    }

    ui->lblObservation->setText(observation);

    QStandardItemModel *model = new QStandardItemModel(notes.count(), 2, this);

    model->setHorizontalHeaderItem(0, new QStandardItem("Matière"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Note"));

    for (int i = 0; i < notes.count(); ++i) {
        model->setItem(i, 0, new QStandardItem(notes[i].first));
        model->setItem(i, 1, new QStandardItem(QString::number(notes[i].second)));
    }

    ui->tbvNotes->setModel(model);
    ui->tbvNotes->resizeColumnsToContents();
}
