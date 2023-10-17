#include "accueil.h"
#include "ui_accueil.h"
#include "notes.h"
#include "bulletin.h"

accueil::accueil(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::accueil)
{
    ui->setupUi(this);
    ui->tabWidget->setCurrentIndex(0);
    ui->stackedWidget->setCurrentIndex(0);

    if (connOpen()) {
        ui->lblStatus->setText("Connecté a la base de donnée...");
    }
    else {
        ui->lblStatus->setText("Erreur lors de la connection a la base de donnée...");
    }
    createTable();

    connect(ui->cbxNiveauAdmis, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(afficherAdmis()));
    connect(ui->cbxNiveauBulletin, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(afficherListe()));

    afficherNiv();
    afficherEl();
    afficherMat();
    afficherAdmis();
    afficherListe();

}

accueil::~accueil()
{
    delete ui;
}

//AFFICHAGE NIVEAUX----------------------------------------------------------------------------------------------------------------

void accueil::afficherNiv()
{
    QSqlQueryModel *model1 = new QSqlQueryModel();
    QSqlQuery *query1 = new QSqlQuery(myDb);

    query1->prepare("SELECT ref_niv AS Niveaux FROM niveau");

    if (query1->exec())
    {
        qDebug() << "Lines displayed...";
    }

    model1->setQuery(*query1);
    ui->tbvListeNiv->setModel(model1);
    ui->tbvListeNiv->resizeColumnsToContents();

    QSqlQueryModel *model = new QSqlQueryModel();
    QSqlQuery *query = new QSqlQuery(myDb);

    query->prepare("SELECT ref_niv FROM niveau");

    if (query->exec()) {
        qDebug() << "Query executed...";
    }

    model->setQuery(*query);
    ui->cbxNiveauEl->setModel(model);
    ui->cbxNiveauMat->setModel(model);
    ui->cbxNiveauAdmis->setModel(model);
    ui->cbxNiveauBulletin->setModel(model);
}

//AFFICHAGE ELEVES----------------------------------------------------------------------------------------------------------------

void accueil::afficherEl()
{
    QSqlQueryModel *model2 = new QSqlQueryModel();
    QSqlQuery *query2 = new QSqlQuery(myDb);

    query2->prepare("SELECT num_el AS Numéro, nom_el AS Nom, age AS Age, ref_niv AS Niveau FROM eleve");

    if (query2->exec())
    {
        qDebug() << "Lines displayed...";
    }

    model2->setQuery(*query2);
    ui->tbvListeEl->setModel(model2);
    ui->tbvListeEl->resizeColumnsToContents();
}

//AFFICHAGE MATIERES-------------------------------------------------------------------------------------------------------------

void accueil::afficherMat()
{
    QSqlQueryModel *model3 = new QSqlQueryModel();
    QSqlQuery *query3 = new QSqlQuery(myDb);

    query3->prepare("SELECT code_mat AS Code_Matière, design AS Désignation, coeff AS Coefficient, ref_niv AS Niveau FROM matiere");

    if (query3->exec())
    {
        qDebug() << "Lines displayed...";
    }

    model3->setQuery(*query3);
    ui->tbvListeMat->setModel(model3);
    ui->tbvListeMat->resizeColumnsToContents();
}

//AFFICHAGE ADMIS----------------------------------------------------------------------------------------------------------------

void accueil::afficherAdmis()
{
    QString niveau = ui->cbxNiveauAdmis->currentText();

    QSqlQueryModel *model4 = new QSqlQueryModel();

    QSqlQuery query4;
    query4.prepare("SELECT ELEVE.num_el AS Numéro, ELEVE.nom_el AS Nom, ELEVE.ref_niv AS Niveau, ROUND(MOYENNE.moyenne, 2) AS Moyenne "
                   "FROM ELEVE "
                   "INNER JOIN MOYENNE ON ELEVE.num_el = MOYENNE.num_el "
                   "WHERE MOYENNE.moyenne >= 10 AND ELEVE.ref_niv = :niveau");
    query4.bindValue(":niveau", niveau);

    if (query4.exec()) {
        qDebug() << "Query executed...";
        model4->setQuery(query4);
        ui->tbvListeAdmis->setModel(model4);
        ui->tbvListeAdmis->resizeColumnsToContents();
    } else {
        qDebug() << "Query failed:" << query4.lastError().text();
    }
}


//AFFICHAGE LISTE ELEVES---------------------------------------------------------------------------------------------------------

void accueil::afficherListe()
{
    QString niveau = ui->cbxNiveauBulletin->currentText();
    QSqlQueryModel *model5 = new QSqlQueryModel();
    QSqlQuery query5;

    query5.prepare("SELECT num_el AS Numéro, nom_el AS Nom, age AS Age, ref_niv AS Niveau FROM eleve WHERE ref_niv = :niveau");
    query5.bindValue(":niveau", niveau);

    if (query5.exec())
    {
        qDebug() << "Lines displayed...";
    }

    model5->setQuery(query5);
    ui->tbvListeElBulletin->setModel(model5);
    ui->tbvListeElBulletin->resizeColumnsToContents();
}

//AJOUT NIVEAUX------------------------------------------------------------------------------------------------------------------

void accueil::on_btnAjouterNiv_clicked()
{
    QString niveau = ui->txtRefNiv->text();

    if (!connOpen()) {
        qDebug() << "Failed to open the database...";
    }

    QMessageBox msgBox;
    QSqlQuery query;

    if (niveau != "") {
        query.prepare("INSERT INTO niveau (ref_niv) VALUES (:niveau)");
        query.bindValue(":niveau", niveau);
    }

    if (!query.exec()) {
        msgBox.setStyleSheet("color: red");
        msgBox.setText("Erreur lors de l'enregistrement du niveau :\n" + query.lastError().text());
        msgBox.exec();
    }
    else {
        msgBox.setText("Niveau enregistré avec succes");
        msgBox.exec();
        ui->txtRefNiv->clear();
    }

    afficherNiv();

    connClose();

}

//SUPPRESSION NIVEAUX-------------------------------------------------------------------------------------------------------------------------------


void accueil::on_btnSupprimerNiv_clicked()
{
    QString niveau = ui->txtRefNiv->text();

    if (!connOpen()) {
        qDebug() << "Failed to open the database...";
    }

    QSqlQuery query;
    query.prepare("DELETE FROM niveau WHERE ref_niv = :niveau");
    query.bindValue(":niveau", niveau);

    QMessageBox msgBox;
    if (query.exec() && niveau != "") {
        msgBox.setText("Niveau supprimé avec succes");
        msgBox.exec();
        ui->txtRefNiv->clear();
    }
    else {
        msgBox.setStyleSheet("color: red");
        msgBox.setText("Erreur lors de la suppression du niveau :\n" + query.lastError().text());
        msgBox.exec();
    }

    afficherNiv();

    connClose();
}


//AJOUT ELEVES-------------------------------------------------------------------------------------------------------------------------------


void accueil::on_btnAjouterEl_clicked()
{
    QString num, nom, age, niveau;
    num = ui->txtNumero->text();
    nom = ui->txtNom->text() + " " + ui->txtPrenom->text();
    age = ui->spbAge->text();
    niveau = ui->cbxNiveauEl->currentText();

    if (!connOpen()) {
        qDebug() << "Failed to open the database...";
    }

    QSqlQuery query;
    QMessageBox msgBox;
    if (niveau != "" && num != "" && num != "" && num != "") {
        query.prepare("INSERT INTO eleve (num_el, nom_el, age, ref_niv) VALUES (:num, :nom, :age, :niveau)");
        query.bindValue(":num", num);
        query.bindValue(":nom", nom);
        query.bindValue(":age", age);
        query.bindValue(":niveau", niveau);
    }

    if (query.exec()) {
        msgBox.setText("Elève enregistré avec succes");
        msgBox.exec();
        ui->txtNumero->clear();
        ui->txtNom->clear();
        ui->txtPrenom->clear();
        ui->spbAge->clear();
    }
    else {
        msgBox.setStyleSheet("color: red");
        msgBox.setText("Erreur lors de l'enregistrement de l'élève :\n" + query.lastError().text());
        msgBox.exec();
    }

    if (niveau == "") {
        msgBox.setStyleSheet("color: red");
        msgBox.setText("Veuillez d'abord ajouter des niveaux!");
        msgBox.exec();

        ui->tabWidget->setCurrentIndex(0);
    }

    afficherEl();

    connClose();
}


//MODIFICATION ELEVES-------------------------------------------------------------------------------------------------------------------------------


void accueil::on_btnModifierEl_clicked()
{
    QString num, nom, age, niveau;
    num = ui->txtNumero->text();
    nom = ui->txtNom->text() + " " + ui->txtPrenom->text();
    age = ui->spbAge->text();
    niveau = ui->cbxNiveauEl->currentText();

    if (!connOpen()) {
        qDebug() << "Failed to open the database...";
    }

    QSqlQuery query;
    QMessageBox msgBox;
    if (nom != "" && age != "" && niveau != "") {
        query.prepare("UPDATE eleve SET nom_el = :nom, age = :age, ref_niv = :niveau WHERE num_el = :num");
        query.bindValue(":nom", nom);
        query.bindValue(":age", age);
        query.bindValue(":niveau", niveau);
        query.bindValue(":num", num);
    }

    if (query.exec()) {
        msgBox.setText("Élève modifié avec succès");
        msgBox.exec();
        ui->txtNumero->clear();
        ui->txtNom->clear();
        ui->txtPrenom->clear();
        ui->spbAge->clear();
    }
    else {
        msgBox.setStyleSheet("color: red");
        msgBox.setText("Erreur lors de la modification de l'élève :\n" + query.lastError().text());
        msgBox.exec();
    }

    afficherEl();

    connClose();

}


//SUPPRESSION ELEVES-------------------------------------------------------------------------------------------------------------------------------


void accueil::on_btnSupprimerEl_clicked()
{
    QString num = ui->txtNumero->text();

    if (!connOpen()) {
        qDebug() << "Failed to open the database...";
    }

    QSqlQuery query;
    if (num != "") {
        query.prepare("DELETE FROM eleve WHERE num_el = :num");
        query.bindValue(":num", num);
    }

    QMessageBox msgBox;
    if (query.exec()) {
        msgBox.setText("Élève supprimé avec succes");
        msgBox.exec();
        ui->txtNumero->clear();
        ui->txtNom->clear();
        ui->txtPrenom->clear();
        ui->spbAge->clear();
    }
    else {
        msgBox.setStyleSheet("color: red");
        msgBox.setText("Erreur lors de la suppression de l'élève :\n" + query.lastError().text());
        msgBox.exec();
    }

    afficherEl();
    connClose();
}


//AJOUT MATIERES-------------------------------------------------------------------------------------------------------------------------------


void accueil::on_btnAjouterMat_clicked()
{
    QString code_mat, design, niveau;
    int coeff;
    code_mat = ui->txtCodeMat->text();
    design = ui->txtDesign->text();
    coeff = ui->spbCoeff->value();
    niveau = ui->cbxNiveauMat->currentText();

    if (!connOpen()) {
        qDebug() << "Failed to open the database...";
    }

    QMessageBox msgBox;

    QSqlQuery query;
    if (niveau != "" && code_mat != "" && design != "" && coeff != 0) {
        query.prepare("INSERT INTO matiere (code_mat, design, coeff, ref_niv) VALUES (:code_mat, :design, :coeff, :niveau)");
        query.bindValue(":code_mat", code_mat);
        query.bindValue(":design", design);
        query.bindValue(":coeff", QVariant(coeff));
        query.bindValue(":niveau", niveau);
    }

    if (query.exec()) {
        msgBox.setText("Matière enregistré avec succes");
        msgBox.exec();
        ui->txtCodeMat->clear();
        ui->txtDesign->clear();
        ui->spbCoeff->clear();
    }
    else {
        msgBox.setStyleSheet("color: red");
        msgBox.setText("Erreur lors de l'enregistrement de la matière :\n" + query.lastError().text());
        msgBox.exec();
    }

    if (niveau == "") {
        msgBox.setStyleSheet("color: red");
        msgBox.setText("Veuillez d'abord ajouter des niveaux!");
        msgBox.exec();

        ui->tabWidget->setCurrentIndex(0);
    }

    afficherMat();

    connClose();

}



//MODIFICATION MATIERES-------------------------------------------------------------------------------------------------------------------------------


void accueil::on_btnModifierMat_clicked()
{
    QString code_mat, design, niveau;
    int coeff;
    code_mat = ui->txtCodeMat->text();
    design = ui->txtDesign->text();
    coeff = ui->spbCoeff->value();
    niveau = ui->cbxNiveauMat->currentText();

    if (!connOpen()) {
        qDebug() << "Failed to open the database...";
    }

    QSqlQuery query;
    if (code_mat != "" && design != "" && coeff != 0) {
        query.prepare("UPDATE matiere SET design = :design, coeff = :coeff, ref_niv = :niveau WHERE code_mat = :code_mat");
        query.bindValue(":design", design);
        query.bindValue(":coeff", QVariant(coeff));
        query.bindValue(":niveau", niveau);
        query.bindValue(":code_mat", code_mat);
    }

    QMessageBox msgBox;
    if (query.exec()) {
        msgBox.setText("Matière modifié avec succès");
        msgBox.exec();
        ui->txtCodeMat->clear();
        ui->txtDesign->clear();
        ui->spbCoeff->clear();
    } else {
        msgBox.setStyleSheet("color: red");
        msgBox.setText("Erreur lors de la modification de la matière :\n" + query.lastError().text());
        msgBox.exec();
    }

    afficherMat();

    connClose();

}


//SUPPRESSION MATIERES-------------------------------------------------------------------------------------------------------------------------------


void accueil::on_btnSupprimerMat_clicked()
{
    QString code_mat = ui->txtCodeMat->text();

    if (!connOpen()) {
        qDebug() << "Failed to open the database...";
    }

    QSqlQuery query;
    query.prepare("DELETE FROM matiere WHERE code_mat = :code_mat");
    query.bindValue(":code_mat", code_mat);

    QMessageBox msgBox;
    if (query.exec() && code_mat != "") {
        msgBox.setText("Matière supprimé avec succes");
        msgBox.exec();
        ui->txtCodeMat->clear();
        ui->txtDesign->clear();
        ui->spbCoeff->clear();
    }
    else {
        msgBox.setStyleSheet("color: red");
        msgBox.setText("Erreur lors de la suppression de la matière :\n" + query.lastError().text());
        msgBox.exec();
    }

    afficherMat();

    connClose();
}

//AFFICHAGE DU BULLETIN---------------------------------------------------------------------------------------------------------

void accueil::on_btnAfficherBulletin_clicked()
{
    QString num = ui->txtNumeroBulletin->text();
    if (!connOpen()) {
        qDebug() << "Failed to open the database...";
    }

    QSqlQuery query;
    query.prepare("SELECT ELEVE.nom_el AS Numéro, ELEVE.age AS Age, ELEVE.ref_niv AS Niveau, NOTE.code_mat, NOTE.note AS Note "
                  "FROM ELEVE "
                  "INNER JOIN NOTE ON ELEVE.num_el = NOTE.num_el "
                  "WHERE ELEVE.num_el = :numEleve");
    query.bindValue(":numEleve", num);

    if (query.exec()) {
        QString nom, age, niveau;
        QList<QPair<QString, float>> notes;

        while (query.next()) {
            if (nom.isEmpty()) {
                nom = query.value(0).toString();
                age = query.value(1).toString();
                niveau = query.value(2).toString();
            }

            QString codeMatiere = query.value(3).toString();
            float note = query.value(4).toFloat();

            QString nomMatiere = obtenirNomMatiere(codeMatiere);

            notes.append(QPair<QString, float>(nomMatiere, note));
        }

        query.prepare("SELECT moyenne FROM moyenne WHERE num_el = :numEleve");
        query.bindValue(":numEleve", num);

        float moyenne;
        if (query.exec() && query.next()) {
            moyenne = query.value(0).toFloat();
        }

        bulletin *bulletinDialog = new bulletin(this);
        bulletinDialog->afficherBulletin(nom, age, niveau, num, moyenne, notes);
        bulletinDialog->exec();
    } else {
        qDebug() << "Erreur lors de la récupération des données de l'élève : " << query.lastError().text();
    }

    connClose();
}

QString accueil::obtenirNomMatiere(const QString& codeMatiere)
{
    QSqlQuery matiereQuery;
    matiereQuery.prepare("SELECT design FROM MATIERE WHERE code_mat = :codeMatiere");
    matiereQuery.bindValue(":codeMatiere", codeMatiere);

    if (matiereQuery.exec() && matiereQuery.next()) {
        return matiereQuery.value(0).toString();
    } else {
        qDebug() << "Erreur lors de la récupération du nom de la matière : " << matiereQuery.lastError().text();
        return "";
    }
}

void accueil::on_btnEnregistrement_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    ui->btnEnregistrement->setStyleSheet("background-color: #1b1b27");
    ui->btnListe->setStyleSheet("background-color: transparent");
    ui->btnBulletin->setStyleSheet("background-color: transparent");
}

void accueil::on_btnListe_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    ui->btnListe->setStyleSheet("background-color: #1b1b27");
    ui->btnEnregistrement->setStyleSheet("background-color: transparent");
    ui->btnBulletin->setStyleSheet("background-color: transparent");
}

void accueil::on_btnBulletin_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
    ui->btnBulletin->setStyleSheet("background-color: #1b1b27");
    ui->btnEnregistrement->setStyleSheet("background-color: transparent");
    ui->btnListe->setStyleSheet("background-color: transparent");
}

void accueil::on_btnQuitter_clicked()
{
    this->close();
}

void accueil::on_pushButton_4_clicked()
{
    notes *Notes = new notes;
    Notes->show();
    this->close();
}

void accueil::on_pushButton_5_clicked()
{
    accueil *Accueil = new accueil;
    Accueil->show();
    this->close();
}
