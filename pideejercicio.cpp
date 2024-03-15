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

#include "pideejercicio.h"
#include "basedatos.h"
#include <QSqlQuery>

pideejercicio::pideejercicio() : QDialog() {
    ui.setupUi(this);

  QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
  QStringList ej1;

  if ( query.isActive() ) {
          while ( query.next() )
                ej1 << query.value(0).toString();
	  }
  ui.ejerciciocomboBox->addItems(ej1);
  ui.cancelarpushButton->setVisible(false);

}

void pideejercicio::activa_cancelar()
{
  ui.cancelarpushButton->setVisible(true);
}

void pideejercicio::asignanombreventana( QString nombre )
{
  setWindowTitle(nombre);
}


void pideejercicio::asignaetiqueta( QString etiqueta )
{
  ui.textLabel->setText(etiqueta);
}


QString pideejercicio::contenido()
{
   return ui.ejerciciocomboBox->currentText();
}

