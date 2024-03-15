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

#include "cambiacipase.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"
#include "introci.h"
#include <QMessageBox>

cambiacipase::cambiacipase() : QDialog() {
 ui.setupUi(this);

   connect(ui.procesarpushButton,SIGNAL(clicked()),this,SLOT(procesar()));

   connect(ui.cipushButton,SIGNAL(clicked()),this,SLOT(botonci()));

}

void cambiacipase::pasainfo( QString qapunte, QString cuenta, QString ci, QDate qfecha)
{
   ci_origen=ci;
   cuenta_pase=cuenta;
   apunte=qapunte;
   QString cad1="APUNTE: ";
   cad1+=qapunte;
   ui.textLabel1->setText(cad1);
   cad1="REEMPLAZAR: ";
   cad1+=ci;
   ui.textLabel2->setText(cad1);
   fechapase=qfecha;
}


void cambiacipase::procesar()
{
    // pase es de asiento de apertura ?
    if (paseapertura(apunte))
       {
        QMessageBox::warning( this, tr("EDITAR CI en apunte"),
                             tr("ERROR: El apunte seleccionado pertenece al asiento de apertura"));
        return;
      }

// CI correcto y cuenta respectiva correcta para CI ?
if (!ciok(ui.codigolineEdit->text()))
       {
        QMessageBox::warning( this, tr("EDITAR CI en apunte"),
                             tr("ERROR: El código suministrado no es un CI correcto"));
        return;
      }
// cuenta correcta para CI ?
if (conanalitica() &&
    (!escuentadegasto(cuenta_pase) && !escuentadeingreso(cuenta_pase)))
  {
    QMessageBox::warning( this, tr("EDITAR CI en apunte"),
                             tr("ERROR: La cuenta de origen no es de ingreso o gasto"));
    return;
  }

if (conanalitica_parc() && !codigo_en_analitica(cuenta_pase))
  {
    QMessageBox::warning( this, tr("EDITAR CI en apunte"),
                             tr("ERROR: La cuenta de origen no es correcta para imputaciones"));
    return;
  }

// apunte que no sea de ejercicio cerrado
if (ejerciciocerrado(ejerciciodelafecha(fechapase)))
   {
       QMessageBox::warning( this, tr("EDITAR CI en apunte"),
                tr("ERROR: No se puede editar un apunte de ejercicio cerrado"));
       return;
   }

 if (basedatos::instancia()->fecha_periodo_bloqueado(fechapase))
   {
       QMessageBox::warning( this, tr("EDITAR CI en apunte"),
                tr("ERROR: El pase corresponde a un periodo cerrado"));
       return;
   }


// ACTUALIZAR PASE EN DIARIO

 basedatos::instancia()->updateDiariocipase(ui.codigolineEdit->text() , apunte);

  QMessageBox::information( this, tr("Edición de CI en apunte"),
	     tr("Operación realizada correctamente"));
  accept();

}



void cambiacipase::botonci()
{
    QString cadena2=ui.codigolineEdit->text();

   introci *c = new introci();
   // c->hazparafiltro();
   if (ciok(cadena2)) c->pasaci(cadena2);
   int cod=c->exec();
   cadena2=c->cadenaci();
    if (cod==QDialog::Accepted)
      ui.codigolineEdit->setText(cadena2);
   delete c;

}

