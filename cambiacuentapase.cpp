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

#include "cambiacuentapase.h"
#include <QSqlQuery>
#include "funciones.h"
#include "basedatos.h"
#include "buscasubcuenta.h"
#include <QMessageBox>

cambiacuentapase::cambiacuentapase() : QDialog() {
 ui.setupUi(this);

   connect(ui.codigolineEdit,SIGNAL(editingFinished()),this,SLOT(finedic()));

   connect(ui.codigolineEdit,SIGNAL(textChanged(QString)),this,SLOT(codigo_cambiado()));

   connect(ui.procesarpushButton,SIGNAL(clicked()),this,SLOT(procesar()));

   connect(ui.buscapushButton,SIGNAL(clicked(bool)),this,SLOT(boton_aux_pulsado()));

}

void cambiacuentapase::pasainfo( QString qapunte, QString codigo, QDate qfecha, QString qdebe, QString qhaber )
{
   codigo_origen=codigo;
   apunte=qapunte;
   QString cad1="APUNTE: ";
   cad1+=qapunte;
   ui.textLabel1->setText(cad1);
   cad1="REEMPLAZAR: ";
   cad1+=codigo;
   ui.textLabel2->setText(cad1);
   fechapase=qfecha;
   debe=qdebe;
   haber=qhaber;
}


void cambiacuentapase::procesar()
{
    // pase es de asiento de apertura ?
    if (paseapertura(apunte))
       {
        QMessageBox::warning( this, tr("Intercambiar cuentas"),
			     tr("ERROR: El pase de la cuenta pertenece al asiento de apertura"));
        return;
      }
    // pase en vencimientos
    if (paseconvto(apunte))
       {
        QMessageBox::warning( this, tr("Intercambiar cuentas"),
			     tr("ERROR: El pase de la cuenta posee registros de vencimientos\n"
			         "para cambiar la cuenta hay que borrar el asiento completo."));
        return;
       }
    if (pasepagovto(apunte))
    {
        QMessageBox::warning( this, tr("Intercambiar cuentas"),
			     tr("ERROR: El pase de la cuenta posee registros de vencimientos\n"
			         "para cambiar la cuenta hay que borrar el asiento completo."));
        return;
    }
    // pase en libro IVA eib
    if (paseenlibroivaeib(apunte))
    {
        QMessageBox::warning( this, tr("Intercambiar cuentas"),
			     tr("ERROR: El pase de la cuenta posee registros en el libro de iva\n"
			          "como entrega intracomunitaria."));
        return;
    }
	
   // existe la subcuenta ?
    if (!existesubcuenta(ui.codigolineEdit->text()) || ui.codigolineEdit->text().length()==0)
       {
        QMessageBox::warning( this, tr("Intercambiar cuentas"),
			     tr("ERROR: Código de cuenta no correcto"));
        return;
      }
    //  problemática del iva
   if (escuentadeivasoportado(codigo_origen) &&
       !escuentadeivasoportado(ui.codigolineEdit->text()))
    {
       QMessageBox::warning( this, tr("Editar subcuenta en pase"),
		tr("ERROR: No se puede cambiar una cuenta de IVA soportado por otra\n"
		   "que no sea del mismo tipo"));
       return;
    }
   if (!escuentadeivasoportado(codigo_origen) &&
       escuentadeivasoportado(ui.codigolineEdit->text()))
    {
       QMessageBox::warning( this, tr("Editar subcuenta en pase"),
		tr("ERROR: No se puede cambiar una cuenta que no sea de IVA soportado \n"
		   "por otra de este tipo"));
       return;
    }
   
   if (escuentadeivarepercutido(codigo_origen) &&
       !escuentadeivarepercutido(ui.codigolineEdit->text()))
    {
       QMessageBox::warning( this, tr("Editar subcuenta en pase"),
		tr("ERROR: No se puede cambiar una cuenta de IVA repercutido por otra\n"
		   "que no sea del mismo tipo"));
       return;
    }
   if (!escuentadeivarepercutido(codigo_origen) &&
       escuentadeivarepercutido(ui.codigolineEdit->text()))
    {
       QMessageBox::warning( this, tr("Editar subcuenta en pase"),
		tr("ERROR: No se puede cambiar una cuenta que no sea de IVA repercutido\n"
		   "por otra de este tipo"));
       return;
    }

    // apunte que no sea de ejercicio cerrado
 if (ejerciciocerrado(ejerciciodelafecha(fechapase)))
   {
       QMessageBox::warning( this, tr("Editar subcuenta en pase"),
		tr("ERROR: No se puede editar un pase de ejercicio cerrado"));
       return;
   }

 if (basedatos::instancia()->ejercicio_bloqueado(ejerciciodelafecha(fechapase)))
   {
       QMessageBox::warning( this, tr("Editar subcuenta en pase"),
        tr("ERROR: No se puede editar un apunte de ejercicio bloqueado"));
       return;
   }

 if (basedatos::instancia()->fecha_periodo_bloqueado(fechapase))
   {
       QMessageBox::warning( this, tr("Editar subcuenta en pase"),
                tr("ERROR: El pase corresponde a un periodo cerrado"));
       return;
   }

 if (basedatos::instancia()->existe_bloqueo_cta(codigo_origen, fechapase))
   {
       QMessageBox::warning( this, tr("Editar subcuenta en pase"),
                tr("ERROR: La cuenta de origen está bloqueada"));
       return;
   }

 if (basedatos::instancia()->existe_bloqueo_cta(ui.codigolineEdit->text(),fechapase))
   {
       QMessageBox::warning( this, tr("Editar subcuenta en pase"),
                tr("ERROR: La cuenta de sustitución está bloqueada"));
       return;
   }

 if (basedatos::instancia()->apunte_cotabilizado(apunte)) {
       QMessageBox::warning( this, tr("Editar subcuenta en apunte"),
                            tr("ERROR: El apunte está marcado como definitivo"));
       return;
 }



 basedatos::instancia()->updateDiariocuentapase(ui.codigolineEdit->text() , apunte);

    if (debe.length()==0) debe = "0";
    if (haber.length()==0) haber = "0";
    basedatos::instancia()->updateSaldossubcuentasaldomenosmascodigo(haber,debe,ui.codigolineEdit->text());
  
    basedatos::instancia()->updateSaldossubcuentasaldomenosmascodigo(debe,haber,codigo_origen);


  QMessageBox::information( this, tr("Intercambiar cuentas"),
	     tr("Operación realizada correctamente"));
  accept();

}


void cambiacuentapase::finedic()
{
  ui.codigolineEdit->setText(expandepunto(ui.codigolineEdit->text(),anchocuentas()));
}

void cambiacuentapase::boton_aux_pulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.codigolineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui.codigolineEdit->setText(cadena2);
       else ui.codigolineEdit->clear();
    delete labusqueda;

}

void cambiacuentapase::codigo_cambiado() {
    QString cadena;
    if (existecodigoplan(ui.codigolineEdit->text(),&cadena))
        ui.descriplineEdit->setText(cadena);
      else ui.descriplineEdit->clear();
}
