#include "actividad_edicion.h"
#include "ui_actividad_edicion.h"
#include <QFile>

Actividad_edicion::Actividad_edicion(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Actividad_edicion)
{
    ui->setupUi(this);
    QFile fich_cods_actividades(":/fich/fich/codigos_actividades.csv");
    if (!fich_cods_actividades.open(QIODevice::ReadOnly | QIODevice::Text)) return;

    QTextStream in(&fich_cods_actividades);
    QStringList lista_codigos_actividades;
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split('\t'); // Dividir por tabulador
        if (fields.size() >= 2)
            lista_codigos_actividades.append(fields[0].trimmed()+" - "+ fields[1].trimmed());
    }
    fich_cods_actividades.close();
    ui->codigo_comboBox->addItems(lista_codigos_actividades);


    QFile fich_tipos_actividadesA(":/fich/fich/tipos_actividades_A.csv");
    if (!fich_tipos_actividadesA.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in2(&fich_tipos_actividadesA);
    while (!in2.atEnd()) {
        QString line = in2.readLine();
        QStringList fields = line.split('\t'); // Dividir por tabulador
        if (fields.size() >= 2)
            tipos_actividades_A.append(fields[0].trimmed()+" - "+ fields[1].trimmed());
    }

    QFile fich_tipos_actividadesB(":/fich/fich/tipos_actividades_B.csv");
    if (!fich_tipos_actividadesB.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in3(&fich_tipos_actividadesB);
    while (!in3.atEnd()) {
        QString line = in3.readLine();
        QStringList fields = line.split('\t'); // Dividir por tabulador
        if (fields.size() >= 2)
            tipos_actividades_B.append(fields[0].trimmed()+" - "+ fields[1].trimmed());
    }

    ui->tipo_comboBox->addItems(tipos_actividades_A);

    tipo_actividad_1 << tr("8611 - ALQUILER DE VIVIENDAS") << tr("8612 - ALQUILER DE LOCALES INDUSTRIALES");
    ui->epigrafe_comboBox->addItems(tipo_actividad_1);

    QFile fich_tipo_actividad_2(":/fich/fich/TIPO_ACTIVIDAD_2.csv");
    if (!fich_tipo_actividad_2.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in4(&fich_tipo_actividad_2);
    while (!in4.atEnd()) {
        QString line = in4.readLine();
        QStringList fields = line.split('\t'); // Dividir por tabulador
        if (fields.size() >= 2)
            tipo_actividad_2.append(fields[0].trimmed()+" - "+ fields[1].trimmed());
    }

    QFile fich_tipo_actividad_3(":/fich/fich/TIPO_ACTIVIDAD_3.csv");
    if (!fich_tipo_actividad_3.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in5(&fich_tipo_actividad_3);
    while (!in5.atEnd()) {
        QString line = in5.readLine();
        QStringList fields = line.split('\t'); // Dividir por tabulador
        if (fields.size() >= 2)
            tipo_actividad_3.append(fields[0].trimmed()+" - "+ fields[1].trimmed());
    }

    QFile fich_tipo_actividad_4(":/fich/fich/TIPO_ACTIVIDAD_4.csv");
    if (!fich_tipo_actividad_4.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in6(&fich_tipo_actividad_4);
    while (!in6.atEnd()) {
        QString line = in6.readLine();
        QStringList fields = line.split('\t'); // Dividir por tabulador
        if (fields.size() >= 2)
            tipo_actividad_4.append(fields[0].trimmed()+" - "+ fields[1].trimmed());
    }

    QFile fich_tipo_actividad_5(":/fich/fich/TIPO_ACTIVIDAD_5.csv");
    if (!fich_tipo_actividad_5.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in7(&fich_tipo_actividad_5);
    while (!in7.atEnd()) {
        QString line = in7.readLine();
        QStringList fields = line.split('\t'); // Dividir por tabulador
        if (fields.size() >= 2)
            tipo_actividad_5.append(fields[0].trimmed()+" - "+ fields[1].trimmed());
    }

    QFile fich_cnae2009(":/fich/fich/codigos_cnae2009.csv");
    if (!fich_cnae2009.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in8(&fich_cnae2009);
    lista_cnae_2009.append(" - - ");
    while (!in8.atEnd()) {
        QString line = in8.readLine();
        QStringList fields = line.split('\t'); // Dividir por tabulador
        if (fields.size() >= 2)
            lista_cnae_2009.append(fields[0].trimmed()+" - "+ fields[1].trimmed());
    }
    ui->cnae_comboBox->addItems(lista_cnae_2009);

}

Actividad_edicion::~Actividad_edicion()
{
    delete ui;
}

void Actividad_edicion::pasa_ref_descrip(QString qreferencia, QString qdescripcion)
{
    ui->referencia_label->setText(qreferencia+": ");
    ui->descripcion_label->setText(qdescripcion);
    referencia=qreferencia;
    descripcion=qdescripcion;
}

void Actividad_edicion::pasa_params(QString codigo, QString tipo, QString epigrafe, QString cnae)
{
    for (int i=0;i<ui->codigo_comboBox->count();i++) {
        QString elemento=ui->codigo_comboBox->itemText(i);
        if (elemento.startsWith(codigo)) {
            ui->codigo_comboBox->setCurrentIndex(i);
        }
    }

    if (!tipo.isEmpty()) {
        for (int i=0;i<ui->tipo_comboBox->count();i++) {
            QString elemento=ui->tipo_comboBox->itemText(i);
            if (elemento.startsWith(tipo)) {
                ui->tipo_comboBox->setCurrentIndex(i);
            }
        }
    }

    if (!epigrafe.isEmpty()) {
        for (int i=0;i<ui->epigrafe_comboBox->count();i++) {
            QString elemento=ui->epigrafe_comboBox->itemText(i);
            if (elemento.startsWith(epigrafe)) {
                ui->epigrafe_comboBox->setCurrentIndex(i);
            }
        }
    }

    if (!cnae.isEmpty()) {
        for (int i=0;i<ui->cnae_comboBox->count();i++) {
            QString elemento=ui->cnae_comboBox->itemText(i);
            if (elemento.startsWith(cnae)) {
                ui->cnae_comboBox->setCurrentIndex(i);
            }
        }

    }
}

void Actividad_edicion::devuelve_params(QString *codigo, QString *tipo, QString *epigrafe, QString *cnae)
{
    *codigo=ui->codigo_comboBox->currentText().section(" - ",0,0);

    if (ui->tipo_comboBox->count()>0) *tipo=ui->tipo_comboBox->currentText().section(" - ",0,0);
    else tipo->clear();

    if (ui->epigrafe_comboBox->count()>0) *epigrafe=ui->epigrafe_comboBox->currentText().section(" - ",0,0);
    else epigrafe->clear();

    if (ui->cnae_comboBox->currentIndex()==0) cnae->clear();
    else *cnae=ui->cnae_comboBox->currentText().section(" - ",0,0);

}

void Actividad_edicion::on_codigo_comboBox_currentIndexChanged(int index)
{
    if (index==0) {
        // Actividades A
        ui->tipo_comboBox->clear();
        ui->tipo_comboBox->addItems(tipos_actividades_A);
        ui->epigrafe_comboBox->addItems(tipo_actividad_1);
    }

    if (index==1) {
        // Actividades B
        ui->tipo_comboBox->clear();
        ui->tipo_comboBox->addItems(tipos_actividades_B);
        ui->epigrafe_comboBox->clear();
    }

    if (index>1) {
        ui->tipo_comboBox->clear();
        ui->epigrafe_comboBox->clear();
    }
}


void Actividad_edicion::on_tipo_comboBox_currentIndexChanged(int index)
{
    if (ui->codigo_comboBox->currentIndex()==0) {
        ui->epigrafe_comboBox->clear();
        if (index==0) ui->epigrafe_comboBox->addItems(tipo_actividad_1);
        if (index==1) ui->epigrafe_comboBox->addItems(tipo_actividad_2);
        if (index==2) ui->epigrafe_comboBox->addItems(tipo_actividad_3);
        if (index==3) ui->epigrafe_comboBox->addItems(tipo_actividad_4);
        if (index==4) ui->epigrafe_comboBox->addItems(tipo_actividad_5);
    }
}

