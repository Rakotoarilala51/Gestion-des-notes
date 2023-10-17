#ifndef ACCUEIL_H
#define ACCUEIL_H

#include <QMainWindow>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>
#include <QMessageBox>
#include <QStandardItem>

namespace Ui {
class accueil;
}

class accueil : public QMainWindow
{
    Q_OBJECT

public:
    QSqlDatabase myDb;
    void connClose(){
        myDb.close();
        myDb.removeDatabase(QSqlDatabase::defaultConnection);
    }

    bool connOpen() {
        myDb = QSqlDatabase::addDatabase("QSQLITE");
        myDb.setDatabaseName("C:database.db");

        if (!myDb.open()) {
            qDebug() << "Failed to open the database";
            return false;
        } else {
            qDebug() << "Connected...";
            return true;
        }
    }

    void createTable() {
        QSqlQuery query;

        query.exec("CREATE TABLE IF NOT EXISTS niveau ("
                   "ref_niv TEXT(10) PRIMARY KEY NOT NULL"
                   ");");

        query.exec("CREATE TABLE IF NOT EXISTS periode ("
                   "annee NUMERIC(4) PRIMARY KEY NOT NULL"
                   ");");

        query.exec("CREATE TABLE IF NOT EXISTS eleve ("
                   "num_el NUMERIC(6) PRIMARY KEY NOT NULL,"
                   "nom_el TEXT(50) NOT NULL,"
                   "age NUMERIC(2) NOT NULL,"
                   "ref_niv TEXT(10) NOT NULL,"
                   "FOREIGN KEY (ref_niv) REFERENCES niveau (ref_niv)"
                   ");");

        query.exec("CREATE TABLE IF NOT EXISTS matiere ("
                   "code_mat NUMERIC(3) PRIMARY KEY NOT NULL,"
                   "design TEXT(20) NOT NULL,"
                   "coeff INTEGER NOT NULL,"
                   "ref_niv TEXT(10) NOT NULL,"
                   "FOREIGN KEY (ref_niv) REFERENCES niveau (ref_niv)"
                   ");");

        query.exec("CREATE TABLE IF NOT EXISTS moyenne ("
                   "num_el NUMERIC(6) NOT NULL,"
                   "annee NUMERIC(4) NOT NULL,"
                   "moyenne INTEGER(2) NOT NULL,"
                   "FOREIGN KEY (annee) REFERENCES periode (annee),"
                   "FOREIGN KEY (num_el) REFERENCES eleve (num_el),"
                   "PRIMARY KEY (num_el, annee)"
                   ");");

        query.exec("CREATE TABLE IF NOT EXISTS note ("
                   "num_el NUMERIC(6) NOT NULL,"
                   "code_mat NUMERIC(3) NOT NULL,"
                   "annee NUMERIC(4) NOT NULL,"
                   "note REAL NOT NULL,"
                   "FOREIGN KEY (num_el) REFERENCES eleve (num_el),"
                   "FOREIGN KEY (annee) REFERENCES periode (annee),"
                   "FOREIGN KEY (code_mat) REFERENCES matiere (code_mat),"
                   "PRIMARY KEY (num_el, code_mat, annee)"
                   ");");

        if (query.lastError().isValid()) {
            qDebug() << "Error creating tables:" << query.lastError().text();
        } else {
            qDebug() << "Tables created successfully";
        }
    }

    QString obtenirNomMatiere(const QString& codeMatiere);

public:
    explicit accueil(QWidget *parent = 0);
    ~accueil();

private slots:

    void on_btnAjouterNiv_clicked();

    void on_btnSupprimerNiv_clicked();

    void on_btnAjouterEl_clicked();

    void on_btnModifierEl_clicked();

    void on_btnSupprimerEl_clicked();

    void on_btnAjouterMat_clicked();

    void on_btnModifierMat_clicked();

    void on_btnSupprimerMat_clicked();

    void on_btnListe_clicked();

    void on_btnBulletin_clicked();

    void on_btnQuitter_clicked();

    void on_pushButton_4_clicked();

    void on_btnEnregistrement_clicked();

    void on_btnAfficherBulletin_clicked();

    void afficherEl();

    void afficherMat();

    void afficherNiv();

    void afficherAdmis();

    void afficherListe();

    void on_pushButton_5_clicked();

private:
    Ui::accueil *ui;
};

#endif // ACCUEIL_H
