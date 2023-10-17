#include "notes.h"
#include "ui_notes.h"
#include "accueil.h"

notes::notes(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::notes)
{
    ui->setupUi(this);

    for (int i = 2023; i <= 2033; i++) {
        ui->cbxAnnee->addItem(QString::number(i));
    }

    accueil conn;
    if (!conn.connOpen()) {
        qDebug() << "Failed to open the database...";
    }

    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery *query = new QSqlQuery(conn.myDb);

    query->prepare("SELECT ref_niv FROM niveau");

    if (query->exec()) {
        qDebug() << "Query executed...";
    }

    model->setQuery(*query);
    ui->cbxNiveauNote->setModel(model);
    connect(ui->cbxNiveauNote, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(mettreAJourListeNotes()));

    mettreAJourListeNotes();

    conn.connClose();
}

notes::~notes()
{
    delete ui;
}


//AJOUT NOTE-------------------------------------------------------------------------------------------------------------------------------


void notes::on_btnAjouter_clicked()
{
    QString num, code_mat, annee;
    float note;
    num = ui->txtNumero->text();
    code_mat = ui->txtCodeMat->text();
    annee = ui->cbxAnnee->currentText();
    note = ui->dsbNote->value();

    accueil conn;
    if (!conn.connOpen()) {
        qDebug() << "Failed to open the database...";
    }

    QMessageBox msgBox;
    QSqlQuery query, nivMat, nivEl;
    if (num != "" && code_mat != "" && annee != "") {
        query.prepare("INSERT INTO note (num_el, code_mat, annee, note) VALUES (:num, :code_mat, :annee, :note)");
        query.bindValue(":num", num);
        query.bindValue(":code_mat", code_mat);
        query.bindValue(":annee", annee);
        query.bindValue(":note", note);

        nivMat.prepare("SELECT ref_niv FROM matiere WHERE code_mat = :code_mat");
        nivMat.bindValue(":code_mat", code_mat);

        nivEl.prepare("SELECT ref_niv FROM eleve WHERE num_el = :num_el");
        nivEl.bindValue(":num_el", num);
    }
    else {
        msgBox.setStyleSheet("color: red");
        msgBox.setText("Veuillez remplir les champs!");
        msgBox.exec();
    }

    if (nivMat.exec() && nivMat.next() && nivEl.exec() && nivEl.next() && num != "" && code_mat != "" && annee != "") {
        QVariant refNivMat = nivMat.value(0);
        QVariant refNivEl = nivEl.value(0);

        if (refNivMat == refNivEl) {
            if (query.exec()) {
                msgBox.setText("Note enregistrée avec succès");
                msgBox.exec();

                calculMoyenne();

                ui->txtCodeMat->clear();
                ui->dsbNote->clear();
            }
            else {
                msgBox.setStyleSheet("color: red");
                msgBox.setText("Erreur lors de l'enregistrement de la note :\n" + query.lastError().text());
                msgBox.exec();
            }
        }
        else {
            msgBox.setStyleSheet("color: red");
            msgBox.setText("Cette matière n'appartient pas au niveau de l'élève");
            msgBox.exec();
        }
    }

    mettreAJourListeNotes();

    conn.connClose();
}


//MODIFICATION NOTE-------------------------------------------------------------------------------------------------------------------------------


void notes::on_btnModifier_clicked()
{
    QString num, code_mat, annee;
    float note;
    num = ui->txtNumero->text();
    code_mat = ui->txtCodeMat->text();
    annee = ui->cbxAnnee->currentText();
    note = ui->dsbNote->value();

    accueil conn;
    if (!conn.connOpen()) {
        qDebug() << "Failed to open the database...";
    }

    QMessageBox msgBox;
    QSqlQuery query, nivMat, nivEl;
    if (num != "" && code_mat != "" && annee != "") {
        query.prepare("UPDATE note SET note = :note WHERE num_el = :num AND code_mat = :code_mat AND annee = :annee");
        query.bindValue(":note", note);
        query.bindValue(":num", num);
        query.bindValue(":code_mat", code_mat);
        query.bindValue(":annee", annee);

        nivMat.prepare("SELECT ref_niv FROM matiere WHERE code_mat = :code_mat");
        nivMat.bindValue(":code_mat", code_mat);

        nivEl.prepare("SELECT ref_niv FROM eleve WHERE num_el = :num_el");
        nivEl.bindValue(":num_el", num);
    }
    else {
        msgBox.setStyleSheet("color: red");
        msgBox.setText("Veuillez remplir les champs!");
        msgBox.exec();
    }


    if (nivMat.exec() && nivMat.next() && nivEl.exec() && nivEl.next() && num != "" && code_mat != "" && annee != "") {
        QVariant refNivMat = nivMat.value(0);
        QVariant refNivEl = nivEl.value(0);

        if (refNivMat == refNivEl) {
            if (query.exec()) {
                msgBox.setText("Note modifié avec succès");
                msgBox.exec();

                calculMoyenne();

                ui->txtCodeMat->clear();
                ui->dsbNote->clear();
            } else {
                msgBox.setStyleSheet("color: red");
                msgBox.setText("Erreur lors de la modification de la note :\n" + query.lastError().text());
                msgBox.exec();
            }
        }
        else {
            msgBox.setStyleSheet("color: red");
            msgBox.setText("Cette matière n'appartient pas au niveau de l'élève");
            msgBox.exec();
        }
    }

    mettreAJourListeNotes();

    conn.connClose();
}


//SUPPRESSION NOTE-------------------------------------------------------------------------------------------------------------------------------


void notes::on_btnSupprimer_clicked()
{
    QString num, code_mat, annee;
    num = ui->txtNumero->text();
    code_mat = ui->txtCodeMat->text();
    annee = ui->cbxAnnee->currentText();

    accueil conn;
    if (!conn.connOpen()) {
        qDebug() << "Failed to open the database...";
    }

    QMessageBox msgBox;
    QSqlQuery query;
    query.prepare("DELETE FROM note WHERE num_el = :num AND code_mat = :code_mat AND annee = :annee");
    query.bindValue(":num", num);
    query.bindValue(":code_mat", code_mat);
    query.bindValue(":annee", annee);

    if (num != "" && code_mat != "" && annee != "") {

        if (query.exec()) {
            msgBox.setText("Note supprimé avec succes");
            msgBox.exec();

            calculMoyenne();

            ui->txtNumero->clear();
            ui->txtCodeMat->clear();
            ui->dsbNote->clear();
        }
        else {
            msgBox.setStyleSheet("color: red");
            msgBox.setText("Erreur lors de la suppression de la note :\n" + query.lastError().text());
            msgBox.exec();
        }
    }
    else {
        msgBox.setStyleSheet("color: red");
        msgBox.setText("Veuillez remplir les champs!");
        msgBox.exec();
    }

    mettreAJourListeNotes();

    conn.connClose();
}

//CALCUL DE LA MOYENNE-----------------------------------------------------------------------------------------------------------------------------

void notes::calculMoyenne() {
    QString num, annee;
    num = ui->txtNumero->text();
    annee = ui->cbxAnnee->currentText();
    QSqlQuery query;
    QSqlQuery coeffQuery;

    QString niveauEleve;
    QSqlQuery niveauQuery;
    niveauQuery.prepare("SELECT ref_niv FROM eleve WHERE num_el = :num");
    niveauQuery.bindValue(":num", num);

    if (niveauQuery.exec() && niveauQuery.next()) {
        niveauEleve = niveauQuery.value(0).toString();
    } else {
        qDebug() << "Erreur lors de la récupération du niveau de l'élève : " << niveauQuery.lastError().text();
        return;
    }

    coeffQuery.prepare("SELECT SUM(coeff) FROM matiere WHERE ref_niv = :niveau");
    coeffQuery.bindValue(":niveau", niveauEleve);

    float sommeCoeff = 0.0;

    if (coeffQuery.exec() && coeffQuery.next()) {
        sommeCoeff = coeffQuery.value(0).toFloat();
    } else {
        qDebug() << "Erreur lors de la récupération de la somme des coefficients : " << coeffQuery.lastError().text();
        return;
    }

    QHash<QString, float> moyenneEleves;

    query.prepare("SELECT note.num_el, SUM(note.note * matiere.coeff) / :sommeCoeff AS moyenne "
                  "FROM note "
                  "INNER JOIN matiere ON note.code_mat = matiere.code_mat "
                  "WHERE matiere.ref_niv = :niveauEleve "
                  "GROUP BY note.num_el");
    query.bindValue(":sommeCoeff", sommeCoeff);
    query.bindValue(":niveauEleve", niveauEleve);

    if (query.exec()) {
        while (query.next()) {
            QString numEl = query.value(0).toString();
            float moyenne = query.value(1).toFloat();
            moyenneEleves[numEl] = moyenne;
        }
    } else {
        qDebug() << "Erreur lors de l'exécution de la requête :" << query.lastError().text();
    }

    QSqlQuery insertQuery;

    foreach (const QString& numEl, moyenneEleves.keys()) {
        float moyenne = moyenneEleves.value(numEl);
        insertQuery.prepare("INSERT OR REPLACE INTO moyenne (num_el, annee, moyenne) VALUES (:num_el, :annee, :moyenne)");
        insertQuery.bindValue(":num_el", numEl);
        insertQuery.bindValue(":annee", annee);
        insertQuery.bindValue(":moyenne", moyenne);

        if (insertQuery.exec()) {
            qDebug() << "Moyenne pour l'élève" << numEl << "insérée avec succès dans la table MOYENNE";
        } else {
            qDebug() << "Erreur lors de l'insertion de la moyenne pour l'élève" << numEl << ":" << insertQuery.lastError().text();
        }
    }
}


//MISE A JOUR DE LA LISTE--------------------------------------------------------------------------------------------------------------------------------

void notes::mettreAJourListeNotes() {
    accueil conn;
    if (!conn.connOpen()) {
        qDebug() << "Failed to open the database...";
    }

    QString niveau = ui->cbxNiveauNote->currentText();
    QSqlQuery matieresQuery;
    matieresQuery.prepare("SELECT matiere.code_mat, matiere.design FROM matiere "
                          "WHERE matiere.ref_niv = :niveau");
    matieresQuery.bindValue(":niveau", niveau);
    matieresQuery.exec();

    QStandardItemModel *model = new QStandardItemModel();

    QStringList headerLabels = { "Numéro d'élève", "Nom d'élève" };
    QMap<QString, int> matiereColumnMap;
    int columnIndex = 2;

    while (matieresQuery.next()) {
        QString codeMatiere = matieresQuery.value(0).toString();
        QString nomMatiere = matieresQuery.value(1).toString() + "(" + codeMatiere + ")";
        headerLabels.append(nomMatiere);

        matiereColumnMap[codeMatiere] = columnIndex;
        ++columnIndex;
    }
    model->setHorizontalHeaderLabels(headerLabels);

    QString pivotQuery = "SELECT eleve.num_el, eleve.nom_el";
    for (const QString &codeMatiere : matiereColumnMap.keys()) {
        pivotQuery += QString(", MAX(CASE WHEN note.code_mat = '%1' THEN note.note END) AS 'Matière %2'")
                      .arg(codeMatiere)
                      .arg(matiereColumnMap[codeMatiere]);
    }
    pivotQuery += " FROM eleve LEFT JOIN note ON eleve.num_el = note.num_el WHERE eleve.ref_niv = '" + niveau + "' GROUP BY eleve.num_el, eleve.nom_el";

    QSqlQuery pivotNotesQuery(pivotQuery);

    while (pivotNotesQuery.next()) {
        QList<QStandardItem*> rowItems;
        for (int i = 0; i < pivotNotesQuery.record().count(); ++i) {
            QStandardItem *item = new QStandardItem(pivotNotesQuery.value(i).toString());
            rowItems.append(item);
        }
        model->appendRow(rowItems);
    }

    ui->tbvListeNote->setModel(model);
    ui->tbvListeNote->resizeColumnsToContents();
}

void notes::on_btnRetour_clicked()
{
    accueil *Accueil = new accueil;
    Accueil->show();
    this->close();
}
