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

#include "renumerar.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"
#include <QMessageBox>

renumerar::renumerar() : QDialog() {
    ui.setupUi(this);

  QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
  QStringList ej1;

  if ( query.isActive() ) {
          while ( query.next() )
                ej1 << query.value(0).toString();
	  }
  ui.ejerciciocomboBox->addItems(ej1);

  connect(ui.aceptarpushButton,SIGNAL(clicked()),SLOT(procesar()));
  connect(ui.ejerciciogroupBox,SIGNAL(toggled(bool)),SLOT(ejercicioboxcambiado()));
  connect(ui.apfechagroupBox,SIGNAL(toggled(bool)),SLOT(apboxcambiado()));

  ui.fechadateEdit->setDate(QDate::currentDate());

}

void renumerar::ejercicioboxcambiado()
{
  ui.apfechagroupBox->disconnect(SIGNAL(toggled(bool) ));
  if (ui.ejerciciogroupBox->isChecked())
      ui.apfechagroupBox->setChecked(false);
    else
        ui.apfechagroupBox->setChecked(true);
  connect(ui.apfechagroupBox,SIGNAL(toggled(bool)),SLOT(apboxcambiado()));
}

void renumerar::apboxcambiado()
{
  ui.ejerciciogroupBox->disconnect(SIGNAL(toggled(bool) ));
  if (ui.apfechagroupBox->isChecked())
      ui.ejerciciogroupBox->setChecked(false);
    else
        ui.ejerciciogroupBox->setChecked(true);
  connect(ui.ejerciciogroupBox,SIGNAL(toggled(bool)),SLOT(ejercicioboxcambiado()));
}

void renumerar::procesar()
{
  // la renumeración se va a reducir a los números de asientos
  // no vamos a tocar los pases

    // averiguamos último número de asiento
    QDate inicioej;
    ui.ejerciciogroupBox->isChecked() ? inicioej=inicioejercicio(ui.ejerciciocomboBox->currentText()) :
                                          inicioej=ui.fechadateEdit->date();
    qlonglong vnum=0;
    // máximo número de asiento con fecha menor que inicioej
    // sólo válido para renumerar por fecha
    QString ejercicio;
    if (!ui.ejerciciogroupBox->isChecked())
       {
         vnum = basedatos::instancia()->selectMaxasientofecha(inicioej);
         vnum++;
         ejercicio=ejerciciodelafecha(inicioej);
       }
      else
         {
          vnum=1;
          // vnum = basedatos::instancia()->min_asiento(ui.ejerciciocomboBox->currentText());
          ejercicio=ui.ejerciciocomboBox->currentText();
         }


    // reemplazamos los asientos a partir de la fecha de inicio de ejercicio
    int aprocesar = basedatos::instancia()->selectCountasientodiariofecha(inicioej); // del ejercicio de la fecha
    ui.progressBar->setMaximum(aprocesar);

    QSqlQuery query = basedatos::instancia()->selectAsientopasediariofechaorderfechaasientopase(inicioej);

    qlonglong asientoguarda=0;
    int pos=0;
    bool actualizadoamort=false;
    if ( query.isActive() ) {
          while (query.next() )
                {
                 pos++;
          	 ui.progressBar->setValue(pos);
	          if (asientoguarda==0) asientoguarda=query.value(0).toLongLong();
		  if (asientoguarda!=query.value(0).toLongLong()) 
		     { 
		        vnum++;
	                asientoguarda=query.value(0).toLongLong();
		     }


	          QString cadnum; cadnum.setNum(vnum);

              // asiento en amortcontable ??
              if (basedatos::instancia()->esasientodeamort(query.value(0).toString(),
                                                           ejercicio))
                 {
                  // QMessageBox::information( this, tr("RENUMERAR"),);
                  if (!actualizadoamort)
                     {
                      basedatos::instancia()->renum_amortiz (query.value(0).toString(), cadnum,
                                                             ejercicio);
                      actualizadoamort=true;
                     }
                 }
              basedatos::instancia()->cambia_asiento_a_pase (query.value(1).toString(),
                                                             cadnum);
	      }
          }
    // actualizamos los valores de próximo asiento y próximo pase en configuración
    vnum++;
    QString cadnum;
    cadnum.setNum(vnum);
    basedatos::instancia()->update_ejercicio_prox_asiento(ejercicio, cadnum);
    QMessageBox::information( this, tr("RENUMERAR"),
		              tr("El proceso ha concluido."));
    ui.progressBar->reset();
    accept();
}
