/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)   José Manuel Díez Botella

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

#include "pidefecha.h"


pidefecha::pidefecha() : QDialog() {
    ui.setupUi(this);

}


void pidefecha::asignaetiqueta(QString nombre)
{
    ui.label->setText(nombre);
}

QDate pidefecha::fecha(void)
{
    return ui.calendarWidget->selectedDate();
}

void pidefecha::pasafecha(QDate fecha)
{
  ui.calendarWidget->setSelectedDate(fecha);
}

void pidefecha::asigna_concepto(QString concepto)
{
   ui.conceptolineEdit->setText(concepto);
}

void pidefecha::asigna_documento(QString documento)
{
  ui.documentolineEdit->setText(documento);
}

QString pidefecha::recupera_concepto()
{
  return ui.conceptolineEdit->text();
}

QString pidefecha::recupera_documento()
{
  return ui.documentolineEdit->text();
}

void pidefecha::esconde_concepto_documento()
{
  ui.documentolineEdit->hide();
  ui.conceptolineEdit->hide();
  ui.label_2->hide();
  ui.label_3->hide();
}
