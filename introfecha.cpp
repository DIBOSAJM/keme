/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)

    Este programa es software libre: usted puede redistribuirlo y/o modificarlo 
    bajo los términos de la Licencia Pública General GNU publicada 
    por la Fundación para el Software Libre, ya sea la versión 3 
    de la Licencia, o (a su elección) cualquier versión posterior.

    Este programa se distribuye con la esperanza de que sea útil, pero 
    SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
    MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
    Consulte los detalles de la Licencia Pública General GNU para obtener 
    una información más detallada. 

    Debería haber recibido una copia de la Licencia Pública General GNU 
    junto a este programa. 
    En caso contrario, consulte <http://www.gnu.org/licenses/>.
  ----------------------------------------------------------------------------------*/

#include "introfecha.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"

introfecha::introfecha(QDate fecha) : QDialog() {
    ui.setupUi(this);

    ui.fechadateEdit->setDate(fecha);
  QStringList diarios;

 diarios << diario_no_asignado();

    QSqlQuery query = basedatos::instancia()->selectCodigoDiariosordercodigo();
    while (query.next())
       {
           diarios << query.value(0).toString();
       }

  ui.diariocomboBox->addItems(diarios);

  herram=0;

  // ui.herramientas_groupBox->setEnabled(false);

  // connect(ui.nocerrarcheckBox,SIGNAL(stateChanged(int)),SLOT(nocerrar_cambiado()));
  connect(ui.varios_sop_pushButton,SIGNAL(clicked()),SLOT(boton_varios_sop()));
  connect(ui.aib_pushButton,SIGNAL(clicked()),SLOT(boton_aib()));
  connect(ui.varios_rep_pushButton,SIGNAL(clicked()),SLOT(boton_varios_rep()));
  connect(ui.eib_pushButton,SIGNAL(clicked()),SLOT(boton_eib()));
  connect(ui.predef_pushButton,SIGNAL(clicked()),SLOT(boton_predef()));
}

QDate introfecha::fecha()
{
  return ui.fechadateEdit->date();
}

QString introfecha::diario()
{
  return ui.diariocomboBox->currentText();
}

bool introfecha::nocerrar()
{
  return ui.nocerrarcheckBox->isChecked();
}

QString introfecha::documento()
{
  return ui.documentolineEdit->text();
}

/*
void introfecha::nocerrar_cambiado()
{
  if (ui.nocerrarcheckBox->isChecked()) ui.herramientas_groupBox->setEnabled(true);
   else ui.herramientas_groupBox->setEnabled(false);
}
*/

void introfecha::esconde_herramientas()
{
  ui.herramientas_groupBox->hide();
  ui.nocerrarcheckBox->hide();
}

void introfecha::pasa_fecha_nocerrar(QDate fecha, bool nocerrar)
{
  ui.fechadateEdit->setDate(fecha);
  ui.nocerrarcheckBox->setChecked(nocerrar);
}

int introfecha::herramienta()
{
  int devolver=herram;
  herram=0;
  return devolver;
}


void introfecha::boton_varios_sop()
{
  herram=1;
  accept();
}

void introfecha::boton_aib()
{
  herram=2;
  accept();
}

void introfecha::boton_varios_rep()
{
  herram=3;
  accept();
}

void introfecha::boton_eib()
{
  herram=4;
  accept();
}

void introfecha::boton_predef()
{
  herram=5;
  accept();
}
