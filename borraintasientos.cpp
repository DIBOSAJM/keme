/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2.1; aplicación para llevar contabilidades

    Copyright (C) 2007  José Manuel Díez Botella

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

#include "borraintasientos.h"
#include "funciones.h"
#include "pideejercicio.h"
#include "basedatos.h"
#include <QMessageBox>

borraintasientos::borraintasientos() : QDialog() {
    ui.setupUi(this);

 ui.iniciallineEdit->setInputMask("000000000000000");
 ui.finallineEdit->setInputMask  ("000000000000000");

    connect(ui.procesarpushButton,SIGNAL(clicked()),this,SLOT(procesar()));

}

void borraintasientos::procesar()
{

    pideejercicio *ej=new pideejercicio();
    ej->asignanombreventana(tr("DIARIO"));
    ej->asignaetiqueta(tr("Código de ejercicio:"));
    ej->exec();
    QString elejercicio=ej->contenido();

  ui.iniciallineEdit->setText(ui.iniciallineEdit->text().remove(' '));
  ui.finallineEdit->setText(ui.finallineEdit->text().remove(' '));
  QString intervalo=ui.iniciallineEdit->text();
  intervalo+=" - ";
  intervalo+=ui.finallineEdit->text();
    if (!QMessageBox::question(
            this,
            tr("¿ Borrar asientos ?"),
            tr("¿ Desea borrar el intervalo de asientos %1 ?").arg(intervalo),
            tr("&Sí"), tr("&No"),
          QString(), 0, 1 ) ==0 )
                          return;
    // falta comprobar si algún asiento corresponde a ejercicio cerrado
    if (ejerciciocerrado(elejercicio))
    {
      QMessageBox::warning( this, tr("¿ Borrar asientos ?"),
                     tr("Error, el intervalo corresponde\n"
                        "a un ejercicio cerrado."));
      return;
    }

    // falta comprobar si ejercicio bloqueado
    if (basedatos::instancia()->ejercicio_bloqueado(elejercicio))
    {
      QMessageBox::warning( this, tr("¿ Borrar asientos ?"),
                     tr("Error, el intervalo corresponde\n"
                        "a un ejercicio bloqueado."));
      return;
    }

    // comprobar si algún asiento corresponde a periodo bloqueado
   if (basedatos::instancia()->int_asientos_per_bloqueado(ui.iniciallineEdit->text(),
                                                          ui.finallineEdit->text(),
                                                          elejercicio))
    {
      QMessageBox::warning( this, tr("¿ Borrar asientos ?"),
                     tr("Error, en el intervalo hay asientos correspondientes\n"
                        "a periodos bloqueados."));
      return;
    }

    // comprobar si hay algún asiento con cuentas bloqueadas
   if (basedatos::instancia()->int_asientos_cuentas_bloqueadas(ui.iniciallineEdit->text(),
                                                               ui.finallineEdit->text(),
                                                               elejercicio))
    {
      QMessageBox::warning( this, tr("¿ Borrar asientos ?"),
                     tr("Error, en el intervalo hay asientos con cuentas "
                        "bloqueadas"));
      return;
    }

   // comprobar si hay algún asiento con enlace externo
  if (basedatos::instancia()->int_asientos_con_enlace(ui.iniciallineEdit->text(),
                                                              ui.finallineEdit->text(),
                                                              elejercicio))
   {
     QMessageBox::warning( this, tr("¿ Borrar asientos ?"),
                    tr("Error, hay asientos con enlace en aplicación externa \n"
                       "que no se pueden borrar"));
     return;
   }

   borraasientos(ui.iniciallineEdit->text(),ui.finallineEdit->text(),elejercicio);
   accept();
}
