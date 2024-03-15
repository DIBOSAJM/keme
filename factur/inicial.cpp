/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)  José Manuel Díez Botella

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

#include "inicial.h"

inicial::inicial() : QDialog() {
    ui.setupUi(this);

#ifdef SMART
   ui.Lb_Keme->setText("ContaSmart-KEME");
 #else
   ui.Lb_Keme->setText("KEME-Contabilidad");
#endif

}


bool inicial::abrir()
{
  if (ui.abrirradioButton->isChecked()) return true;
  return false;
}


bool inicial::crear()
{
  if (ui.crearradioButton->isChecked()) return true;
  return false;
}


bool inicial::salir()
{
  if (ui.salirradioButton->isChecked()) return true;
  return false;
}
