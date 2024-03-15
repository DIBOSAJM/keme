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

#include "suprimecierre.h"
#include <QSqlQuery>
#include <QSqlDatabase>
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>

suprimecierre::suprimecierre() : QDialog() {
  ui.setupUi(this);


  QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
  QStringList ej1;

  if ( query.isActive() ) {
          while ( query.next() )
                ej1 << query.value(0).toString();
	  }
  ui.ejerciciocomboBox->addItems(ej1);

   connect(ui.aceptarpushButton,SIGNAL(clicked()),SLOT(procesar()));

}


void suprimecierre::procesar()
{
   QDate inicioej; inicioej=inicioejercicio(ui.ejerciciocomboBox->currentText());
   QDate finej=finejercicio(ui.ejerciciocomboBox->currentText());

    basedatos::instancia()->deleteDiariodiariofechas( diario_cierre() , inicioej, finej );

    basedatos::instancia()->deleteDiariodiariofechas( diario_regularizacion() , inicioej, finej );

    basedatos::instancia()->updateEjercicioscerradocodigo(false, ui.ejerciciocomboBox->currentText());

   QDate fecha_sig_ejercicio=finej.addDays(1);
   QString sig_ejercicio=ejerciciodelafecha(fecha_sig_ejercicio);
   if (!sig_ejercicio.isEmpty())
      {
       basedatos::instancia()->deleteDiariodiariofechas( diario_apertura() ,
                                                         inicioejercicio(sig_ejercicio),
                                                         finejercicio(sig_ejercicio));
      }

   actualizasaldos();

   QMessageBox::information( this, tr("Deshacer cierre"),
		              tr("Se ha eliminado el cierre para el ejercicio seleccionado"));
 
   accept();

}
