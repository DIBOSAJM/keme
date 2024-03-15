/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C) José Manuel Díez Botella

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

#include "editapaseconci.h"
#include "funciones.h"


editapaseconci::editapaseconci() : QDialog() {
    ui.setupUi(this);

 ui.fechadateEdit->setDate(QDate::currentDate());
 comadecimal=haycomadecimal();
 connect(ui.debelineEdit,SIGNAL(textChanged(QString)),this,SLOT(debecambiado()));
 connect(ui.haberlineEdit,SIGNAL(textChanged(QString)),this,SLOT(habercambiado()));

}


void editapaseconci::pasadatos(QDate fecha,QString descripcion, QString debe, QString haber)
{
 ui.haberlineEdit->disconnect(SIGNAL(textChanged(QString)));
 ui.debelineEdit->disconnect(SIGNAL(textChanged(QString)));

ui.fechadateEdit->setDate(fecha);
ui.descripcionlineEdit->setText(descripcion);
ui.debelineEdit->setText(debe);
ui.haberlineEdit->setText(haber);

 connect(ui.debelineEdit,SIGNAL(textChanged(QString)),this,SLOT(debecambiado()));
 connect(ui.haberlineEdit,SIGNAL(textChanged(QString)),this,SLOT(habercambiado()));

}

void editapaseconci::recuperadatos(QDate *fecha,QString *descripcion, QString *debe, QString *haber)
{
 *fecha=ui.fechadateEdit->date();
 *descripcion=ui.descripcionlineEdit->text();
 *debe=ui.debelineEdit->text();
 *haber=ui.haberlineEdit->text();
}


void editapaseconci::debecambiado()
{
 if (comadecimal) 
   if (ui.debelineEdit->text().contains('.'))
      ui.debelineEdit->setText(ui.debelineEdit->text().replace('.',','));
 ui.haberlineEdit->disconnect(SIGNAL(textChanged(QString)));
 ui.haberlineEdit->clear();
 connect(ui.haberlineEdit,SIGNAL(textChanged(QString)),this,SLOT(habercambiado()));
}


void editapaseconci::habercambiado()
{
 if (comadecimal)
  if (ui.haberlineEdit->text().contains('.'))
     ui.haberlineEdit->setText(ui.haberlineEdit->text().replace('.',','));
 ui.debelineEdit->disconnect(SIGNAL(textChanged(QString)));
 ui.debelineEdit->clear();
 connect(ui.debelineEdit,SIGNAL(textChanged(QString)),this,SLOT(debecambiado()));
}
