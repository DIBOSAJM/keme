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

#include "cambiaci.h"
#include "funciones.h"
#include "basedatos.h"
#include <QSqlQuery>
#include "introci.h"
#include <QMessageBox>

cambiaci::cambiaci() : QDialog() {
 ui.setupUi(this);

 QDate fechaactual;
 QString ejercicio=ejerciciodelafecha(fechaactual.currentDate());
  if (ejercicio.length()>0)
    {
      ui.inicialdateEdit->setDate(inicioejercicio(ejercicio));
      ui.finaldateEdit->setDate(finejercicio(ejercicio));
    }
  else 
        {
           ui.inicialdateEdit->setDate(fechaactual.currentDate());
           ui.finaldateEdit->setDate(fechaactual.currentDate());
        }


   connect(ui.buscapushButton1,SIGNAL(clicked()),this,SLOT(botonci1()));
   connect(ui.buscapushButton2,SIGNAL(clicked()),this,SLOT(botonci2()));
   connect(ui.procesarpushButton,SIGNAL(clicked()),this,SLOT(procesar()));

   connect(ui.fechasgroupBox,SIGNAL(toggled (bool)),this,SLOT(intervalofechascambiado()));
   connect(ui.todocheckBox,SIGNAL(toggled (bool)),this,SLOT(intercambiartodocambiado()));
}


void cambiaci::botonci1()
{

    QString cadena2=ui.origenlineEdit->text();

   introci *c = new introci();
   // c->hazparafiltro();
   if (ciok(cadena2)) c->pasaci(cadena2);
   int cod=c->exec();
   cadena2=c->cadenaci();
    if (cod==QDialog::Accepted)
      ui.origenlineEdit->setText(cadena2);
   delete c;

}


void cambiaci::botonci2()
{
    QString cadena2=ui.porlineEdit->text();

   introci *c = new introci();
   // c->hazparafiltro();
   if (ciok(cadena2)) c->pasaci(cadena2);
   int cod=c->exec();
   cadena2=c->cadenaci();
    if (cod==QDialog::Accepted)
      ui.porlineEdit->setText(cadena2);
   delete c;

}


void cambiaci::procesar()
{
  // comprobar ci correcto
  if (!ciok(ui.origenlineEdit->text()))
       {
        QMessageBox::warning( this, tr("EDITAR CI en diario"),
                             tr("ERROR: El código origen suministrado no es un CI correcto"));
        return;
      }

  if (!ciok(ui.porlineEdit->text()))
       {
        QMessageBox::warning( this, tr("EDITAR CI en diario"),
                             tr("ERROR: El código nuevo suministrado no es un CI correcto"));
        return;
      }


  // comprobamos si en el intervalo de fechas hay algún período bloqueado
  if (ui.fechasgroupBox->isChecked())
    {
      if (basedatos::instancia()->periodo_bloqueado_entre_fechas(ui.inicialdateEdit->date(),
                                                             ui.finaldateEdit->date()))
        {
         QMessageBox::warning( this, tr("Intercambiar cuentas"),
                             tr("ERROR: Hay periodos bloqueados entre las fechas"));
         return;
        }
    }
    else
        if (basedatos::instancia()->hayperiodosbloqueados())
          {
           QMessageBox::warning( this, tr("Intercambiar cuentas"),
                             tr("ERROR: Hay periodos bloqueados"));
           return;
          }
  // comprobamos que cuenta origen y destino no estén bloqueadas
  if (basedatos::instancia()->existe_bloqueo_cta_gen(ui.origenlineEdit->text()))
                {
                 QMessageBox::warning( this, tr("Intercambiar cuentas"),tr("ERROR: La cuenta %1 "
                                               "está bloqueada").arg(ui.origenlineEdit->text()));
                 return;
                }
  
  // comprobamos que cuenta origen y destino no estén bloqueadas
  if (basedatos::instancia()->existe_bloqueo_cta_gen(ui.porlineEdit->text()))
                {
                 QMessageBox::warning( this, tr("Intercambiar cuentas"),tr("ERROR: La cuenta %1 "
                                               "está bloqueada").arg(ui.porlineEdit->text()));
                 return;
                }

  // primero comprobamos si afecta a algún ejercicio cerrado
  if (ui.fechasgroupBox->isChecked())
    {
      if (ejerciciocerrado(ejerciciodelafecha(ui.inicialdateEdit->date())) ||
         ejerciciocerrando(ejerciciodelafecha(ui.inicialdateEdit->date())) )
        {
          QMessageBox::warning( this, tr("Intercambiar cuentas"),
			     tr("ERROR: Alguna de las fechas pertenece a ejercicio cerrado"));
          return;
        }
     }
     else
	 // comprobamos si existen anotaciones de la cuenta en ejercicio cerrado
     {
        QSqlQuery query = basedatos::instancia()->selectMinfechadiario();

        if ( query.isActive() )
	    {
	        if (query.first())
		    {
                        if (ejerciciocerrado(ejerciciodelafecha(query.value(0).toDate())))
                        {
                         QMessageBox::warning( this, tr("Intercambiar cuentas"),
			            tr("ERROR: Alguna de las fechas pertenece a ejercicio cerrado"));
                         return;
		        }
		    }
	    }
     }

  basedatos::instancia()->updateDiarioCI( ui.porlineEdit->text() , ui.origenlineEdit->text() ,
                                          ui.fechasgroupBox->isChecked() , ui.inicialdateEdit->date() ,
                                          ui.finaldateEdit->date() );

  
  QMessageBox::information( this, tr("Intercambiar CI"),
             tr("Operación realizada"));
  accept();
}


void cambiaci::intercambiartodocambiado()
{
//  ui.fechasgroupBox->disconnect(SIGNAL(setChecked(bool)));

  if (ui.todocheckBox->isChecked()) ui.fechasgroupBox->setChecked(false);
    else ui.fechasgroupBox->setChecked(true);

 //  connect(ui.fechasgroupBox,SIGNAL(toggled (bool)),this,SLOT(intervalofechascambiado()));

}



void cambiaci::intervalofechascambiado()
{
//  ui.todocheckBox->disconnect(SIGNAL(setChecked(bool)));

    if (ui.fechasgroupBox->isChecked()) ui.todocheckBox->setChecked(false);
       else ui.todocheckBox->setChecked(true);
//   connect(ui.todocheckBox,SIGNAL(toggled (bool)),this,SLOT(intercambiartodocambiado()));

}

