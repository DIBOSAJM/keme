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

#include "cambiacuenta.h"
#include "funciones.h"
#include "basedatos.h"
#include <QSqlQuery>
#include "buscasubcuenta.h"
#include <QMessageBox>

cambiacuenta::cambiacuenta() : QDialog() {
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


   connect(ui.buscapushButton1,SIGNAL(clicked()),this,SLOT(buscasubcuenta1()));
   connect(ui.buscapushButton2,SIGNAL(clicked()),this,SLOT(buscasubcuenta2()));
   connect(ui.origenlineEdit,SIGNAL(editingFinished()),this,SLOT(origenexpandepunto()));
   connect(ui.porlineEdit,SIGNAL(editingFinished()),this,SLOT(porexpandepunto()));
   connect(ui.procesarpushButton,SIGNAL(clicked()),this,SLOT(procesar()));

   connect(ui.fechasgroupBox,SIGNAL(toggled (bool)),this,SLOT(intervalofechascambiado()));
   connect(ui.todocheckBox,SIGNAL(toggled (bool)),this,SLOT(intercambiartodocambiado()));
}


void cambiacuenta::buscasubcuenta1()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.origenlineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cadena2.length()>0) ui.origenlineEdit->setText(cadena2);
       else ui.origenlineEdit->setText("");
    delete labusqueda;

}


void cambiacuenta::buscasubcuenta2()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.porlineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cadena2.length()>0) ui.porlineEdit->setText(cadena2);
       else ui.porlineEdit->setText("");
    delete labusqueda;

}


void cambiacuenta::origenexpandepunto()
{
  if (ui.origenlineEdit->text().length()==0) return;
  ui.origenlineEdit->setText(expandepunto(ui.origenlineEdit->text(),anchocuentas()));
  // if (ui.origenlineEdit->text().length()<anchocuentas()) buscasubcuenta1();

}


void cambiacuenta::porexpandepunto()
{
  if (ui.porlineEdit->text().length()==0) return;
  ui.porlineEdit->setText(expandepunto(ui.porlineEdit->text(),anchocuentas()));
  // if (ui.porlineEdit->text().length()<anchocuentas()) buscasubcuenta2();

}

void cambiacuenta::procesar()
{
    // si origen = iva soportado, el destino= iva soportado
    // idem con iva repercutido
   if (escuentadeivasoportado(ui.origenlineEdit->text()) &&
       !escuentadeivasoportado(ui.porlineEdit->text()))
    {
       QMessageBox::warning( this, tr("Intercambiar cuentas"),
		tr("ERROR: No se puede cambiar una cuenta de IVA soportado por otra\n"
		   "que no sea del mismo tipo"));
       return;
    }

   if (!escuentadeivasoportado(ui.origenlineEdit->text()) &&
       escuentadeivasoportado(ui.porlineEdit->text()))
    {
       QMessageBox::warning( this, tr("Intercambiar cuentas"),
		tr("ERROR: No se puede cambiar una cuenta que no sea de IVA soportado \n"
		   "por otra de este tipo"));
       return;
    }

   if (escuentadeivarepercutido(ui.origenlineEdit->text()) &&
       !escuentadeivarepercutido(ui.porlineEdit->text()))
    {
       QMessageBox::warning( this, tr("Intercambiar cuentas"),
		tr("ERROR: No se puede cambiar una cuenta de IVA repercutido por otra\n"
		   "que no sea del mismo tipo"));
       return;
    }

   if (!escuentadeivarepercutido(ui.origenlineEdit->text()) &&
       escuentadeivarepercutido(ui.porlineEdit->text()))
    {
       QMessageBox::warning( this, tr("Intercambiar cuentas"),
		tr("ERROR: No se puede cambiar una cuenta que no sea de IVA repercutido\n"
		   "por otra de este tipo"));
       return;
    }

  if (!cuentaendiario(ui.origenlineEdit->text()))
    {
       QMessageBox::warning( this, tr("Intercambiar cuentas"),
			     tr("ERROR: No existe en diario la cuenta a reemplazar"));
       return;
    }

  if (!existesubcuenta(ui.origenlineEdit->text()) || !existesubcuenta(ui.porlineEdit->text()))
    {
       QMessageBox::warning( this, tr("Intercambiar cuentas"),
			     tr("ERROR: Códigos de cuenta no correctos"));
       return;
    }
  
/*  if (cuentaenlibroiva(ui.origenlineEdit->text()) || cuentaenlibroiva(ui.porlineEdit->text()))
    {
       QMessageBox::warning( this, tr("Intercambiar cuentas"),
			     tr("ERROR: Cuentas en libros de iva; no se pueden intercambiar"));
       return;
    }*/

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
        QSqlQuery query = basedatos::instancia()->selectMinfechadiariocuenta( ui.origenlineEdit->text() );

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


  // primero comprobamos si afecta a algún ejercicio bloqueado
  if (ui.fechasgroupBox->isChecked())
    {
      if (basedatos::instancia()->ejercicio_bloqueado(ejerciciodelafecha(ui.inicialdateEdit->date())) ||
         basedatos::instancia()->ejercicio_bloqueado(ejerciciodelafecha(ui.inicialdateEdit->date())) )
        {
          QMessageBox::warning( this, tr("Intercambiar cuentas"),
                 tr("ERROR: Alguna de las fechas pertenece a ejercicio bloqueado"));
          return;
        }
     }
     else
     // comprobamos si existen anotaciones de la cuenta en ejercicio cerrado
     {
        QSqlQuery query = basedatos::instancia()->selectMinfechadiariocuenta( ui.origenlineEdit->text() );

        if ( query.isActive() )
        {
            if (query.first())
            {
                if (basedatos::instancia()->ejercicio_bloqueado(ejerciciodelafecha(query.value(0).toDate())))
                {
                    QMessageBox::warning( this, tr("Intercambiar cuentas"),
                        tr("ERROR: Alguna de las fechas pertenece a ejercicio bloqueado"));
                    return;
                }
            }
        }
     }



    basedatos::instancia()->updateDiariocuentafechasnodiario_apertura( ui.porlineEdit->text() , ui.origenlineEdit->text() , ui.fechasgroupBox->isChecked() , ui.inicialdateEdit->date() , ui.finaldateEdit->date() );

  // actualizamos saldos de las cuentas no ?
  QSqlQuery query = basedatos::instancia()->selectSumdebesumhaberdiariocuenta( ui.origenlineEdit->text() );
  if ( (query.isActive()) && (query.first()) )
  {
           double valor=query.value(0).toDouble();
           QString lacad;
           lacad.setNum(valor,'f',2);
           if (lacad.contains('-')>0)
	       {
	        lacad=lacad.remove('-');
	        pasasaldo(ui.origenlineEdit->text(),lacad,false);
	       }
	  else pasasaldo(ui.origenlineEdit->text(),lacad,true);
  }

  query = basedatos::instancia()->selectSumdebesumhaberdiariocuenta( ui.porlineEdit->text() );
  if ( (query.isActive()) && (query.first()) )
   {
           double valor=query.value(0).toDouble();
           QString lacad;
           lacad.setNum(valor,'f',2);
           if (lacad.contains('-')>0)
    	   {
    	       lacad=lacad.remove('-');
    	       pasasaldo(ui.porlineEdit->text(),lacad,false);
    	   }
	   else pasasaldo(ui.porlineEdit->text(),lacad,true);
   }
  
  QMessageBox::information( this, tr("Intercambiar cuentas"),
	     tr("Operación realizada correctamente"));
  accept();
}


void cambiacuenta::intercambiartodocambiado()
{
//  ui.fechasgroupBox->disconnect(SIGNAL(setChecked(bool)));

  if (ui.todocheckBox->isChecked()) ui.fechasgroupBox->setChecked(false);
    else ui.fechasgroupBox->setChecked(true);

 //  connect(ui.fechasgroupBox,SIGNAL(toggled (bool)),this,SLOT(intervalofechascambiado()));

}



void cambiacuenta::intervalofechascambiado()
{
//  ui.todocheckBox->disconnect(SIGNAL(setChecked(bool)));

    if (ui.fechasgroupBox->isChecked()) ui.todocheckBox->setChecked(false);
       else ui.todocheckBox->setChecked(true);
//   connect(ui.todocheckBox,SIGNAL(toggled (bool)),this,SLOT(intercambiartodocambiado()));

}


