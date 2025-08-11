#include "cambia_actividad.h"
#include "ui_cambia_actividad.h"
#include "basedatos.h"

Cambia_actividad::Cambia_actividad(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Cambia_actividad)
{
    ui->setupUi(this);

    // cargamos combo
    ui->cambiar_comboBox->addItems(basedatos::instancia()->activ_ecas());
}

Cambia_actividad::~Cambia_actividad()
{
    delete ui;
}

void Cambia_actividad::pasa_actividad(QString cod_actividad)
{
  // bool basedatos::existecodigotabla_actividades(QString ref, QString *descripcion, QString *codigo, QString *tipo, QString *epigrafe, QString *cnae)

  QString descripcion, codigo, tipo, epigrafe, cnae;
  if (basedatos::instancia()->existecodigotabla_actividades(cod_actividad, &descripcion, &codigo, &tipo, &epigrafe, &cnae)) {
      ui->actividad_lineEdit->setText(cod_actividad + " - " + descripcion);
  }

}

QString Cambia_actividad::nueva_actividad()
{
   return ui->cambiar_comboBox->currentText().section('-',0,0).trimmed();
}


