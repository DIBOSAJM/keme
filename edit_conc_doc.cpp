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

#include "edit_conc_doc.h"
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>


edit_conc_doc::edit_conc_doc() : QDialog() {
    ui.setupUi(this);
   asiento=false;
   connect(ui.aceptarpushButton,SIGNAL(clicked()),this,SLOT(botonaceptar()));

}


void edit_conc_doc::pasadatos( QString qnumpase, QDate qfecha, QString concepto, QString documento )
{
  ui.conceptolineEdit->setText(concepto);
  ui.documentolineEdit->setText(documento);
  fecha=qfecha;
  numpase=qnumpase;
}


void edit_conc_doc::botonaceptar()
{
 if (ejerciciocerrado(ejerciciodelafecha(fecha)))
   {
       QMessageBox::warning( this, tr("Editar apunte"),
		tr("ERROR: No se puede editar un pase de ejercicio cerrado"));
       return;
   }

    basedatos::instancia()->updateDiarioconceptodocumentopase( ui.conceptolineEdit->text() ,
                                                               ui.documentolineEdit->text() , numpase, asiento);

  QMessageBox::information( this, tr("Editar apunte"),
	     tr("Operación realizada correctamente"));
  accept();

}

void edit_conc_doc::todo_el_asiento() {
    asiento=true;
    setWindowTitle(tr("Cambiar documento y concepto en asiento"));
}
