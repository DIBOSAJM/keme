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

#include "marcacerrado.h"
#include <QSqlQuery>
#include "basedatos.h"
#include <QMessageBox>

marcacerrado::marcacerrado() : QDialog() {
    ui.setupUi(this);

  QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
  QStringList ej1;

  if ( query.isActive() ) {
          while ( query.next() )
                ej1 << query.value(0).toString();
	  }
  ui.ejerciciocomboBox->addItems(ej1);

  connect(ui.procesarpushButton,SIGNAL(clicked()),SLOT(procesar()));
                void procesar();

}


void marcacerrado::procesar()
{
    basedatos::instancia()->updateEjercicioscerradocodigo(true, ui.ejerciciocomboBox->currentText());
   QMessageBox::information( this, tr("Marcar como cerrado"),
		              tr("El ejercicio seleccionado se ha marcado como cerrado"));

   accept();

}
