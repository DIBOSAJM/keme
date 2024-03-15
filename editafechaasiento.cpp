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

#include "editafechaasiento.h"
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>

editafechaasiento::editafechaasiento() : QDialog()
 {
    ui.setupUi(this);

   connect(ui.aceptarpushButton,SIGNAL(clicked()),this,SLOT(procesar()));

 }


void editafechaasiento::pasadatos( QString asiento, QDate fecha )
{
   gfecha=fecha;
   gasiento=asiento;
   QString cadena=tr("NÚM. ASIENTO: ");
   cadena+=asiento;
   ui.textLabel1->setText(cadena);
   cadena=tr("FECHA A EDITAR: ");
   cadena+=gfecha.toString("dd/MM/yyyy");
   ui.textLabel2->setText(cadena);
   ui.dateEdit->setDate(gfecha);
}


void editafechaasiento::procesar()
{
 if (ejerciciodelafecha(gfecha)!=ejerciciodelafecha(ui.dateEdit->date()))
    {
        QMessageBox::warning( this, tr("Editar fecha"),
                 tr("ERROR: No se pueden intercambiar ejercicios con esta utilidad.\n"
                     "Utilice la tabla de edición de asientos."));
        return;
    }

  // comprobar si fecha original no está en ejercicio cerrado
 if (ejerciciocerrado(ejerciciodelafecha(gfecha)))
   {
       QMessageBox::warning( this, tr("Editar fecha"),
		tr("ERROR: No se pueden editar asientos de ejercicios cerrados"));
       return;
   }

 // comprobar si fecha original no está en ejercicio bloqueado
if (basedatos::instancia()->ejercicio_bloqueado(ejerciciodelafecha(gfecha)))
  {
      QMessageBox::warning( this, tr("Editar fecha"),
       tr("ERROR: No se pueden editar asientos de ejercicios bloqueados"));
      return;
  }


 if (ejerciciodelafecha(ui.dateEdit->date()).length()==0)
   {
       QMessageBox::warning( this, tr("Editar fecha"),
		tr("ERROR: La fecha especificada no corresponde a un ejercicio definido"));
       return;
   }


 if (ejerciciocerrado(ejerciciodelafecha(ui.dateEdit->date())))
   {
       QMessageBox::warning( this, tr("Editar fecha"),
		tr("ERROR: La fecha especificada corresponde a ejercicio cerrado"));
       return;
   }


 if (basedatos::instancia()->ejercicio_bloqueado(ejerciciodelafecha(ui.dateEdit->date())))
   {
       QMessageBox::warning( this, tr("Editar fecha"),
        tr("ERROR: La fecha especificada corresponde a ejercicio bloqueado"));
       return;
   }



 if (basedatos::instancia()->fecha_periodo_bloqueado(ui.dateEdit->date()))
   {
       QMessageBox::warning( this, tr("Editar fecha"),
                tr("ERROR: La fecha especificada corresponde a un periodo bloqueado"));
       return;
   }

 if (basedatos::instancia()->fecha_periodo_bloqueado(gfecha))
   {
       QMessageBox::warning( this, tr("Editar fecha"),
                tr("ERROR: La fecha a sustituir corresponde a un periodo bloqueado"));
       return;
   }

  // procesamos operación
  basedatos::instancia()->updateDiariofechaasiento(ui.dateEdit->date() , gasiento);
  QMessageBox::information( this, tr("Cambiar fechas"),
	     tr("Operación realizada correctamente"));
  accept();

}
