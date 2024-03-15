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

#include "eib.h"
#include "funciones.h"
#include <QMessageBox>
#include "aux_express.h"
#include "buscasubcuenta.h"
#include "basedatos.h"
#include "busca_externo.h"

eib::eib(bool concomadecimal, bool condecimales) : QDialog() {
    ui.setupUi(this);

    comadecimal=concomadecimal;
    decimales=condecimales;
    baseoblig=false;

 cargacombooperaciones();

 connect(ui.CtabaselineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctabasecambiada()));
 connect(ui.CtabaselineEdit,SIGNAL(editingFinished()),this,SLOT(ctabasefinedicion()));
 connect(ui.CtafralineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctafracambiada()));
 connect(ui.CtafralineEdit,SIGNAL(editingFinished()),this,SLOT(ctafrafinedicion()));
 connect(ui.AceptarButton,SIGNAL(clicked()),this,SLOT(botonaceptar()));
 connect(ui.ctabasepushButton,SIGNAL(clicked()),this,SLOT(buscactabase()));
 connect(ui.ctafrapushButton,SIGNAL(clicked()),this,SLOT(buscactafra()));
 connect(ui.CtabaselineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));
 connect(ui.CtafralineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));

 connect(ui.FechafradateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(fechafracambiada()));

 connect(ui.externo_pushButton,SIGNAL(clicked(bool)),this,SLOT(buscar_externo()));

 connect(ui.externo_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(externo_cambiado()));

 if (!basedatos::instancia()->hay_externos())
 {
  ui.externo_label->hide();
  ui.externo_lineEdit->hide();
  ui.externo_pushButton->hide();
  ui.descrip_externo_label->hide();
 }

 }

void eib::pasadatos( QString cuentabase, QString baseimponible, QString ctafra,
                     QDate qfechafra, QDate qfechaop, QString claveop )
{
 ui.CtabaselineEdit->setText(cuentabase);
 if (cuentabase.length()>0) ui.basetextLabel->setText(descripcioncuenta(cuentabase));
 ui.baselineEdit->setText(baseimponible);
 ui.CtafralineEdit->setText(ctafra);
 if (ctafra.length()>0) ui.fratextLabel->setText(descripcioncuenta(ctafra));
 ui.FechafradateEdit->setDate(qfechafra);
 ui.fechaoperaciondateEdit->setDate(qfechaop);

  // buscar claveop en el combo
 QString extracto;
 for (int veces=0;veces<ui.clavecomboBox->count();veces++)
     {
        QString cadenaclave=ui.clavecomboBox->itemText(veces);
        extracto=cadenaclave.section(" ",0,0);
        extracto=extracto.remove(' ');
        if (extracto==claveop)
           {
            ui.clavecomboBox->setCurrentIndex(veces);
            break;
           }
     }

}



void eib::ctabasecambiada()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);

    QString cadena;
    if (ui.CtabaselineEdit->text().length()==anchocuentas() && !cod_longitud_variable()) {
	if (!existecodigoplan(ui.CtabaselineEdit->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.CtabaselineEdit->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2);
	           if (!existecodigoplan(ui.CtabaselineEdit->text(),&cadena))
			ui.CtabaselineEdit->setText("");
		 }
		  else
	            ui.CtabaselineEdit->setText("");	     
	   }
         chequeabotonaceptar();
        }
   if (esauxiliar(ui.CtabaselineEdit->text()))
       ui.basetextLabel->setText(descripcioncuenta(ui.CtabaselineEdit->text()));
     else ui.basetextLabel->setText("");
}

void eib::ctabasefinedicion()
{
  ui.CtabaselineEdit->setText(expandepunto(ui.CtabaselineEdit->text(),anchocuentas()));
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
 QString cadena;
  if (ui.CtabaselineEdit->text().length()>3 && cod_longitud_variable()) {
	if (!existecodigoplan(ui.CtabaselineEdit->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.CtabaselineEdit->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2); 
	           if (!esauxiliar(ui.CtabaselineEdit->text()))
			ui.CtabaselineEdit->setText("");	     
		 }
		  else
	            ui.CtabaselineEdit->setText("");	     
            chequeabotonaceptar();
	   }
        if (esauxiliar(ui.CtabaselineEdit->text()))
          ui.basetextLabel->setText(descripcioncuenta(ui.CtabaselineEdit->text()));
         else ui.basetextLabel->setText("");
        }
}


void eib::ctafracambiada()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);

   QString cadena;
    if (ui.CtafralineEdit->text().length()==anchocuentas() && !cod_longitud_variable()) {
	if (!existecodigoplan(ui.CtafralineEdit->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.CtafralineEdit->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2); 
	           if (!existecodigoplan(ui.CtafralineEdit->text(),&cadena))
			ui.CtafralineEdit->setText("");	     
		 }
		  else
	            ui.CtafralineEdit->setText("");	     
	}
        }
    chequeabotonaceptar();
    if (esauxiliar(ui.CtafralineEdit->text())) 
	ui.fratextLabel->setText(descripcioncuenta(ui.CtafralineEdit->text()));
    else ui.fratextLabel->setText("");
}

void eib::ctafrafinedicion()
{
   ui.CtafralineEdit->setText(expandepunto(ui.CtafralineEdit->text(),anchocuentas()));
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
 QString cadena;
   ui.CtafralineEdit->setText(expandepunto(ui.CtafralineEdit->text(),anchocuentas()));
     if (ui.CtafralineEdit->text().length()>3 && cod_longitud_variable()) {
	if (!existecodigoplan(ui.CtafralineEdit->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.CtafralineEdit->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2);
	           if (!esauxiliar(ui.CtafralineEdit->text()))
			ui.CtafralineEdit->setText("");
		 }
		  else
	            ui.CtafralineEdit->setText("");
            chequeabotonaceptar();
	  }
        if (esauxiliar(ui.CtafralineEdit->text()))
	   ui.fratextLabel->setText(descripcioncuenta(ui.CtafralineEdit->text()));
          else ui.fratextLabel->clear();
        }

}


void eib::botonaceptar()
{     
    bool ok;
    double valor=convapunto(ui.baselineEdit->text()).toDouble(&ok);
    if ((valor<0.0001 && valor>-0.0001) || !ok)
	        {
	          QMessageBox::warning( this, "ERROR",
                    "El valor de la base no puede ser nulo");
	          return;
                }


    if (!esauxiliar(ui.CtabaselineEdit->text()))
            {
	      if (ui.CtabaselineEdit->text().length()==0 && baseoblig)
	        {
	          QMessageBox::warning( this, "ERROR",
                    "La cuenta base no es correcta");
	          return;
                }
	      if (ui.CtabaselineEdit->text().length()!=0)
	        {
	         QMessageBox::warning( this, "ERROR",
                    "La cuenta base no es correcta");
	         return;
                }
            }

    if (!esauxiliar(ui.CtafralineEdit->text())) 
            {
	      QMessageBox::warning( this, "ERROR",
               "La cuenta de factura no es correcta");
	      return;
            }

     accept();
}


void eib::recuperadatos( QString *cuentabase, QString *baseimponible,  QString *ctafra,
                         QDate *qfechafra, QDate *qfechaop, QString *claveop,
                         QString *prservicios)
{
*cuentabase=ui.CtabaselineEdit->text();
*baseimponible=ui.baselineEdit->text();
*ctafra=ui.CtafralineEdit->text();
*qfechafra=ui.FechafradateEdit->date();
*qfechaop=ui.fechaoperaciondateEdit->date();
QString cadenaclave=ui.clavecomboBox->currentText();
QString extracto=cadenaclave.section(' ',0,0);
*claveop=extracto.remove(' ');
*prservicios= ui.prservicioscheckBox->isChecked() ? "1" : "";
}


void eib::chequeabotonaceptar()
{
    QString cadena="";
    if (!esauxiliar(ui.CtabaselineEdit->text()))
        { 
	if (ui.CtabaselineEdit->text().length()!=0)
       { ui.AceptarButton->setEnabled(false); return; }
	if (baseoblig && ui.CtabaselineEdit->text().length()==0)
       { ui.AceptarButton->setEnabled(false); return; }
        }

    if (!esauxiliar(ui.CtafralineEdit->text()))
        { ui.AceptarButton->setEnabled(false); return; }

    ui.AceptarButton->setEnabled(true);

}

void eib::buscactabase()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtabaselineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cadena2.length()>0) ui.CtabaselineEdit->setText(cadena2);
       else ui.CtabaselineEdit->setText("");
    delete(labusqueda);
}

void eib::buscactafra()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtafralineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cadena2.length()>0) ui.CtafralineEdit->setText(cadena2);
       else ui.CtafralineEdit->setText("");
    delete(labusqueda);
}




void eib::hazbaseoblig()
{
  baseoblig=true;
  QDate fecha;
  ui.FechafradateEdit->setDate(fecha.currentDate());
}


void eib::cargacombooperaciones()
{
  QStringList lista=basedatos::instancia()->listaoperaciones_expedidas();
  ui.clavecomboBox->addItems (lista);
}

void eib::fechafracambiada()
{
 ui.fechaoperaciondateEdit->setDate(ui.FechafradateEdit->date());
}

void eib::selec_prservicios()
{
  ui.prservicioscheckBox->setChecked(true);
}

void eib::modoconsulta()
{
    ui.baselineEdit->setReadOnly(true);
    ui.clavecomboBox->setEnabled(false);
    ui.CtabaselineEdit->setReadOnly(true);
    ui.ctabasepushButton->setEnabled(false);
    ui.CtafralineEdit->setReadOnly(true);
    ui.ctafrapushButton->setEnabled(false);
    ui.FechafradateEdit->setReadOnly(true);
    ui.fechaoperaciondateEdit->setReadOnly(true);
    ui.prservicioscheckBox->setEnabled(false);
    ui.externo_lineEdit->setReadOnly(true);
    ui.externo_pushButton->setEnabled(false);
}


QString eib::externo()
{
   return ui.externo_lineEdit->text();
}

void eib::esconde_externo()
{
    ui.externo_lineEdit->hide();
    ui.externo_pushButton->hide();
    ui.descrip_externo_label->hide();
}

void eib::buscar_externo()
{
    busca_externo *b = new busca_externo();
    if (b->exec()==QDialog::Accepted)
      {
        ui.externo_lineEdit->setText(b->codigo_elec());
      }
      else ui.externo_lineEdit->clear();
    delete(b);
}


void eib::externo_cambiado()
{
    if (ui.externo_lineEdit->text().isEmpty()) return;

    if (basedatos::instancia()->existe_externo(ui.externo_lineEdit->text()))
       {
        ui.descrip_externo_label->setText(basedatos::instancia()->razon_externo(ui.externo_lineEdit->text()));
        // si campo cuenta proveedor vacío, meter cuenta asociada a externo
        if (ui.CtafralineEdit->text().isEmpty())
            ui.CtafralineEdit->setText(basedatos::instancia()->cuenta_externo(ui.externo_lineEdit->text()));
        return;
       }

    // no existe externo -- probamos si el nif existe en auxiliares
   QString codigo_cuenta=basedatos::instancia()->selectCuentaCifDatossubcuenta(
               ui.externo_lineEdit->text());
   if (!codigo_cuenta.isEmpty())
     {
       ui.CtafralineEdit->setText(codigo_cuenta);
       // borramos contenido en externo
       ui.externo_lineEdit->clear();
     }

    ui.descrip_externo_label->setText("- -");
}


