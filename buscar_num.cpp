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

#include "buscar_num.h"

buscar_num::buscar_num() : QDialog() {
    ui.setupUi(this);

ui.buscarlineEdit->setFocus();
}


void buscar_num::pasadatos(QString valor, QString margen)
{
  ui.buscarlineEdit->setText(valor);
  ui.margenlineEdit->setText(margen);
}

void buscar_num::activarprincipio(bool activar)
{
  ui.principioradioButton->setChecked(activar);
  ui.cursorradioButton->setChecked(!activar);
}

QString buscar_num::cadnum()
{
 return ui.buscarlineEdit->text();
}

QString buscar_num::margen()
{
 return ui.margenlineEdit->text();
}


bool buscar_num::a_partir_cursor()
{
  return ui.cursorradioButton->isChecked();
}
