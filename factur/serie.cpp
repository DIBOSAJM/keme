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


#include "serie.h"
#include "basedatos.h"

serie::serie() : QDialog() {
    ui.setupUi(this);

ui.proxdoclineEdit->setFocus();

connect(ui.aceptarpushButton,SIGNAL(clicked()),this,SLOT(actualizar()));

}

void serie::pasadatos(QString codigo, QString descrip, qlonglong num)
{
    ui.codigolineEdit->setText(codigo);
    ui.descriplineEdit->setText(descrip);
    QString cadnum;
    cadnum.setNum(num);
    ui.proxdoclineEdit->setText(cadnum);
}


void serie::actualizar()
{
    basedatos::instancia()->actualizaserie(ui.codigolineEdit->text(),
                                           ui.descriplineEdit->text(),
                                           ui.proxdoclineEdit->text());
    accept();
}
