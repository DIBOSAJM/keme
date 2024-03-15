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

#include "pidecuenta.h"
#include "funciones.h"
#include "aux_express.h"
#include "buscasubcuenta.h"
#include <QMessageBox>


pidecuenta::pidecuenta() : QDialog() {
    ui.setupUi(this);

connect(ui.auxiliarlineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctacambiada()));
 connect(ui.auxiliarlineEdit,SIGNAL(editingFinished()),this,SLOT(ctafinedicion()));
 connect(ui.buscapushButton,SIGNAL(clicked()),this,SLOT(buscacta()));

}

void pidecuenta::ctacambiada()
{

   QString cadena;
    if (ui.auxiliarlineEdit->text().length()==anchocuentas() && !cod_longitud_variable()) {
	if (!existecodigoplan(ui.auxiliarlineEdit->text(),&cadena)) 
	  {
	   // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
           if (QMessageBox::question(
              this,
              tr("Crear cuenta auxiliar"),
              tr("Esa cuenta no existe, ¿ Desea añadirla ?"),
              tr("&Sí"), tr("&No"),
               QString(), 0, 1 ) ==0 )
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.auxiliarlineEdit->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2);
	           if (!existecodigoplan(ui.auxiliarlineEdit->text(),&cadena))
			ui.auxiliarlineEdit->clear();
		 }
		  else
	            ui.auxiliarlineEdit->clear();
	  }
        }
    // chequeabotonaceptar();
    if (esauxiliar(ui.auxiliarlineEdit->text()))
       {
	ui.auxiliarlabel->setText(descripcioncuenta(ui.auxiliarlineEdit->text()));
        ui.aceptarpushButton->setEnabled(true);
       }
    else 
        {
         ui.auxiliarlabel->setText("- -");
         ui.aceptarpushButton->setEnabled(false);
        }
}

void pidecuenta::ctafinedicion()
{
   ui.auxiliarlineEdit->setText(expandepunto(ui.auxiliarlineEdit->text(),anchocuentas()));
    QString cadena;
    if (ui.auxiliarlineEdit->text().length()>3 && cod_longitud_variable()) {
	if (!existecodigoplan(ui.auxiliarlineEdit->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
           if (QMessageBox::question(
              this,
              tr("Crear cuenta auxiliar"),
              tr("Esa cuenta no existe, ¿ Desea añadirla ?"),
              tr("&Sí"), tr("&No"),
                 QString(), 0, 1 ) ==0 )
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.auxiliarlineEdit->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2);
	           if (!existecodigoplan(ui.auxiliarlineEdit->text(),&cadena))
			ui.auxiliarlineEdit->clear();
		 }
		  else
	            ui.auxiliarlineEdit->clear();
	   }
         if (esauxiliar(ui.auxiliarlineEdit->text())) 
            {
	     ui.auxiliarlabel->setText(descripcioncuenta(ui.auxiliarlineEdit->text()));
             ui.aceptarpushButton->setEnabled(true);
            }
          else 
            {
             ui.auxiliarlabel->setText("- -");
             ui.aceptarpushButton->setEnabled(false);
            }
        }

}


void pidecuenta::buscacta()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.auxiliarlineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.auxiliarlineEdit->setText(cadena2);
       else ui.auxiliarlineEdit->clear();
    delete(labusqueda);
}


QString pidecuenta::cuenta()
{
 return ui.auxiliarlineEdit->text();
}

void pidecuenta::cambia_titulo(QString titulo)
{
    setWindowTitle(titulo);
}

void pidecuenta::pasa_cuenta(QString cuenta)
{
 ui.auxiliarlineEdit->setText(cuenta);
}
