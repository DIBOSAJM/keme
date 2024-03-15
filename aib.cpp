/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)

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

#include "aib.h"
#include "funciones.h"
#include "subcuentas.h"
#include "buscasubcuenta.h"
#include "basedatos.h"
#include "aux_express.h"
#include <QMessageBox>
#include "busca_externo.h"

aib::aib(bool concomadecimal, bool condecimales) : QDialog() {
    ui.setupUi(this);

    comadecimal=concomadecimal;
    decimales=condecimales;

    baseoblig=false;
  // Tenemos que cargar clave IVA por defecto y resto de IVA'S en el combo box
    QString pasalinea="";
    QString Clavedefecto, laclave;
    double tipo;
    double re;
    QString qdescrip;
    QString convert;
    Clavedefecto=clave_iva_defecto();
    if (Clavedefecto.length()>0 && existeclavetipoiva(Clavedefecto,&tipo,&re,&qdescrip))
          {
	pasalinea=Clavedefecto;
	pasalinea+=tr(" // IVA:");
	convert.setNum(tipo,'f',2);
	if (comadecimal) ui.TipolineEdit->setText(convacoma(convert));
            else ui.TipolineEdit->setText(convert);
	if (comadecimal) pasalinea+=convacoma(convert);
            else pasalinea+=convert;
	pasalinea+=tr(" RE:");
	convert.setNum(re,'f',2);
	if (comadecimal) pasalinea+=convacoma(convert);
            else pasalinea+=convert;
	pasalinea += " ";
	pasalinea += qdescrip;
	ui.ClaveivacomboBox->insertItem ( 0, pasalinea ) ;
          }
    // ahora cargamos el resto de los tipos de IVA
    
 QSqlQuery query = basedatos::instancia()->selectTodoTiposivanoclave(Clavedefecto);
 

 if ( query.isActive() )
    {
      while (query.next())
         {
	  laclave=query.value(0).toString();
	  tipo=query.value(1).toDouble();
	  re=query.value(2).toDouble();
               qdescrip=query.value(3).toString();
	  pasalinea=laclave;
	  pasalinea+=tr(" // IVA:");
	  convert.setNum(tipo,'f',2);
	  if (comadecimal) pasalinea+=convacoma(convert);
              else pasalinea+=convert;
	  pasalinea+=tr(" RE:");
	  convert.setNum(re,'f',2);
	  if (comadecimal) pasalinea+=convacoma(convert);
              else pasalinea+=convert;
	  pasalinea += " ";
	  pasalinea += qdescrip;
          // ui.ClaveivacomboBox->insertItem (-1 , pasalinea ) ;
          ui.ClaveivacomboBox->addItem (pasalinea ) ;
         }
      }
 QString cadcombo=tipoivaexento();
 cadcombo+=" // ";
 cadcombo+=tr("IVA");
 cadcombo+=tr(": 0 ");
 cadcombo+=tr("RE");
 cadcombo+=tr(": 0");
 ui.ClaveivacomboBox->addItem ("EXENTO // IVA: 0  RE: 0" ) ;

 // averiguamos si hay prorrata y si es especial
 double prorrata=prorrata_iva();
 if (prorrata>0.001)
    {
     baseoblig=true;
     ui.prorratalineEdit->setEnabled(true);
     ui.prorratacheckBox->setEnabled(true);
     QString cadnum; cadnum.setNum(prorrata,'f',0);
     ui.prorratalineEdit->setText(cadnum);
     if (!prorrata_especial())
        {
         ui.prorratacheckBox->setChecked(true);
         // ui.prorratalineEdit->setReadOnly(true);
        }
    }

 basedatos::instancia()->recupera_cuentas_inv_sp(&cta_iva_sop_AIB, &cta_iva_rep_AIB,
                                                 &cta_iva_sop_AIS, &cta_iva_rep_AIS,
                                                 &cta_iva_sop_AES, &cta_iva_rep_AES,
                                                 &cta_iva_sop_OI,  &cta_iva_rep_OI);

 // cta_iva_sop_AIB, cta_iva_rep_AIB
 if (!cta_iva_sop_AIB.isEmpty()) ui.CtaivalineEdit->setText(cta_iva_sop_AIB);
 if (!cta_iva_rep_AIB.isEmpty()) ui.cuentareperlineEdit->setText(cta_iva_rep_AIB);

 connect(ui.ClaveivacomboBox,SIGNAL(activated(int)),this,SLOT(comboivacambiado()));
 connect(ui.CtabaselineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctabasecambiada()));
 connect(ui.CtabaselineEdit,SIGNAL(editingFinished()),this,SLOT(ctabasefinedicion()));
 connect(ui.CtaivalineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctaivacambiada()));
 connect(ui.CtaivalineEdit,SIGNAL(editingFinished()),this,SLOT(ctaivafinedicion()));
 connect(ui.CtafralineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctafracambiada()));
 connect(ui.CtafralineEdit,SIGNAL(editingFinished()),this,SLOT(ctafrafinedicion()));
 connect(ui.cuentareperlineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuentarepcambiada()));
 connect(ui.cuentareperlineEdit,SIGNAL(editingFinished()),this,SLOT(ctarepfinedicion()));
 connect(ui.baselineEdit,SIGNAL(textChanged(QString)),this,SLOT(basecambiada()));
 connect(ui.baselineEdit,SIGNAL(editingFinished()),this,SLOT(basefinedicion()));
 connect(ui.AceptarButton,SIGNAL(clicked()),this,SLOT(botonaceptar()));
// connect(ui.CuotalineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuotacambiada()));
 connect(ui.ctabasepushButton,SIGNAL(clicked()),this,SLOT(buscactabase()));
 connect(ui.ctaivapushButton,SIGNAL(clicked()),this,SLOT(buscactaiva()));
 connect(ui.ctafrapushButton,SIGNAL(clicked()),this,SLOT(buscactafra()));
 connect(ui.reppushButton,SIGNAL(clicked()),this,SLOT(buscactarep()));
 connect(ui.CtabaselineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));
 connect(ui.CtaivalineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));
 connect(ui.CtafralineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));
 connect(ui.cuentareperlineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));

 connect(ui.FechafradateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(fechafracambiada()));

 connect(ui.bicostelineEdit,SIGNAL(textChanged(QString)),this,SLOT(bicostecambiada()));

 connect(ui.autofacturacheckBox,SIGNAL( stateChanged ( int )),this,SLOT(autofacturaue_cambiada()));
 connect(ui.autofacturaNoUecheckBox,SIGNAL( stateChanged ( int )),this,SLOT(autofacturanoue_cambiada()));

 connect(ui.isp_interiorescheckBox,SIGNAL( stateChanged ( int )),this,SLOT(isp_interiores_cambiada()));
 connect(ui.aibcheckBox,SIGNAL( stateChanged ( int )),this,SLOT(aib_check_cambiada()));

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


void aib::pasadatos( QString cuentabase, QString baseimponible, QString qcuentaivasop,
                     QString qcuentaivarep, QString cuotaiva, QString ctafra,
                     QDate qfechafra, QString qprorrata)
{
 ui.CtabaselineEdit->setText(cuentabase);
 if (cuentabase.length()>0) ui.basetextLabel->setText(descripcioncuenta(cuentabase));
ui.CuotalineEdit->setText(cuotaiva);
ui.baselineEdit->setText(baseimponible);
ui.CtaivalineEdit->setText(qcuentaivasop);
ui.cuentareperlineEdit->setText(qcuentaivarep);
ui.CtafralineEdit->setText(ctafra);
if (ctafra.length()>0) ui.fratextLabel->setText(descripcioncuenta(ctafra));
ui.FechafradateEdit->setDate(qfechafra);
if (qprorrata.length()>0 && qprorrata.toDouble()>0.001)
   ui.prorratalineEdit->setText(qprorrata);
comboivacambiado();
}



void aib::comboivacambiado()
{
QString cadenaiva=ui.ClaveivacomboBox->currentText();
QString extracto=cadenaiva.section(':',1,1);
ui.TipolineEdit->setText(extracto.remove(tr(" RE")));

    bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
    if (!bien) bi=0;
    double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
    if (!bien) tipo=0;
    ui.CuotalineEdit->setText( formateanumero(bi*tipo,comadecimal,decimales));
    if (tipo>-0.00001 and tipo<0.00001)
       {
        // desactivamos cuentas de IVA y las dejamos en blanco
        ui.CtaivalineEdit->clear();
        ui.cuentareperlineEdit->clear();
        ui.CtaivalineEdit->setEnabled(false);
        ui.cuentareperlineEdit->setEnabled(false);
        ui.ctaivapushButton->setEnabled(false);
        ui.reppushButton->setEnabled(false);
        chequeabotonaceptar();
       }
        else
            {
              // nos aseguramos de que las cuentas de IVA están activadas
              ui.CtaivalineEdit->setEnabled(true);
              ui.cuentareperlineEdit->setEnabled(true);
              ui.ctaivapushButton->setEnabled(true);
              ui.reppushButton->setEnabled(true);
              chequeabotonaceptar();
            }
    // chequeabotonaceptar();
}


void aib::ctabasecambiada()
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
            chequeabotonaceptar();
	   }
        }
   if (esauxiliar(ui.CtabaselineEdit->text()))
       ui.basetextLabel->setText(descripcioncuenta(ui.CtabaselineEdit->text()));
     else ui.basetextLabel->setText("");
}

void aib::ctabasefinedicion()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);

 QString cadena;

  ui.CtabaselineEdit->setText(expandepunto(ui.CtabaselineEdit->text(),anchocuentas()));

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
	           if (!existecodigoplan(ui.CtabaselineEdit->text(),&cadena))
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


void aib::ctaivacambiada()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);

   QString cadena;
    if (ui.CtaivalineEdit->text().length()==anchocuentas() && !cod_longitud_variable()) {
	if (!existecodigoplan(ui.CtaivalineEdit->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
                   lasubcuenta2->pasacodigo(ui.CtaivalineEdit->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2);
	           if (!existecodigoplan(ui.CtaivalineEdit->text(),&cadena))
			ui.CtaivalineEdit->setText("");	     
		 }
  		else
                    ui.CtaivalineEdit->setText("");
            bool bien;
            double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
            if (!bien) tipo=0;
            if (tipo<-0.00001 || tipo > 0.00001)
               chequeabotonaceptar();
	  }
        }
  if (esauxiliar(ui.CtaivalineEdit->text()))
       ui.ivatextLabel->setText(descripcioncuenta(ui.CtaivalineEdit->text()));
    else ui.ivatextLabel->setText("");
}

void aib::ctaivafinedicion()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);

 QString cadena;

   ui.CtaivalineEdit->setText(expandepunto(ui.CtaivalineEdit->text(),anchocuentas()));
    if (ui.CtaivalineEdit->text().length()>3 && cod_longitud_variable()) {
	if (!existecodigoplan(ui.CtaivalineEdit->text(),&cadena))
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.CtaivalineEdit->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2);
	           if (!existecodigoplan(ui.CtaivalineEdit->text(),&cadena))
			ui.CtaivalineEdit->setText("");	     
		 }
  		else
	            ui.CtaivalineEdit->setText("");	     
            bool bien;
            double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
            if (!bien) tipo=0;
            if (tipo<-0.00001 || tipo > 0.00001)
               chequeabotonaceptar();
	  }
        if (esauxiliar(ui.CtaivalineEdit->text()))
          ui.ivatextLabel->setText(descripcioncuenta(ui.CtaivalineEdit->text()));
         else ui.ivatextLabel->setText("");
        }

}


void aib::ctafracambiada()
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
            chequeabotonaceptar();
	  }
        }
    // chequeabotonaceptar();
    if (esauxiliar(ui.CtafralineEdit->text())) 
	ui.fratextLabel->setText(descripcioncuenta(ui.CtafralineEdit->text()));
    else ui.fratextLabel->setText("");
}

void aib::ctafrafinedicion()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);

   ui.CtafralineEdit->setText(expandepunto(ui.CtafralineEdit->text(),anchocuentas()));
   QString cadena;
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
	           if (!existecodigoplan(ui.CtafralineEdit->text(),&cadena))
			ui.CtafralineEdit->setText("");	     
		 }
		  else
	            ui.CtafralineEdit->setText("");	     
            chequeabotonaceptar();
	  }
        if (esauxiliar(ui.CtafralineEdit->text())) 
	   ui.fratextLabel->setText(descripcioncuenta(ui.CtafralineEdit->text()));
         else ui.fratextLabel->setText("");
        }
   // chequeabotonaceptar();

}


void aib::cuentarepcambiada()
{

 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);

   QString cadena;
    if (ui.cuentareperlineEdit->text().length()==anchocuentas() && !cod_longitud_variable()) {
	if (!existecodigoplan(ui.cuentareperlineEdit->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
                   lasubcuenta2->pasacodigo(ui.cuentareperlineEdit->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2); 
	           if (!existecodigoplan(ui.cuentareperlineEdit->text(),&cadena))
			ui.cuentareperlineEdit->setText("");
		 }
  		else
	            ui.cuentareperlineEdit->setText("");
            bool bien;
            double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
            if (!bien) tipo=0;
            if (tipo<-0.00001 || tipo > 0.00001)
               chequeabotonaceptar();
	   }
        }
  if (esauxiliar(ui.cuentareperlineEdit->text()))
       ui.reptextLabel->setText(descripcioncuenta(ui.cuentareperlineEdit->text()));
    else ui.reptextLabel->setText("");


}

void aib::ctarepfinedicion()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
   ui.cuentareperlineEdit->setText(expandepunto(ui.cuentareperlineEdit->text(),anchocuentas()));
   QString cadena;
    if (ui.cuentareperlineEdit->text().length()>3 && cod_longitud_variable()) {
	if (!existecodigoplan(ui.cuentareperlineEdit->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.cuentareperlineEdit->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2); 
	           if (!existecodigoplan(ui.cuentareperlineEdit->text(),&cadena))
			ui.cuentareperlineEdit->setText("");
		 }
  		else
	            ui.cuentareperlineEdit->setText("");
            bool bien;
            double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
            if (!bien) tipo=0;
            if (tipo<-0.00001 || tipo > 0.00001)
               chequeabotonaceptar();
	  }
        if (esauxiliar(ui.cuentareperlineEdit->text()))
          ui.reptextLabel->setText(descripcioncuenta(ui.cuentareperlineEdit->text()));
         else ui.reptextLabel->setText("");
        }

}


void aib::basecambiada()
{
    bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     QString cadena;
     int ndecimales=2;
     if (!decimales) ndecimales=0;
     cadena.setNum(bi*tipo,'f',ndecimales);
     if (comadecimal)
          ui.CuotalineEdit->setText( convacoma(cadena));
        else
          ui.CuotalineEdit->setText( cadena);
    // if (comadecimal) ui.baselineEdit->setText(ui.baselineEdit->text().replace('.',','));
  chequeabotonaceptar();
}

void aib::basefinedicion()
{
    bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;
     QString cadena;
     int ndecimales=2;
     if (!decimales) ndecimales=0;
     cadena.setNum(bi,'f',ndecimales);
     if (comadecimal)
         ui.baselineEdit->setText(convacoma(cadena));
        else
         ui.baselineEdit->setText(cadena);

}


void aib::botonaceptar()
{
    bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     double cuota = convapunto(ui.CuotalineEdit->text()).toDouble(& bien);
     if (!bien) cuota=0;
     
     if (bi*tipo-cuota>=0.005 || bi*tipo-cuota<=-0.005)
            {
	 QMessageBox::warning( this, tr("ERROR"),
               tr("La cuota es incorrecta"));
	 return;
            }
	 
     if (bi<0.0001 && bi>-0.0001) 
            {
	 QMessageBox::warning( this, tr("ERROR"),
               tr("La base imponible no puede ser nula"));
	 return;
            }
     if (cuota<0.0001 && cuota >-0.0001 && tipo>0.00001)
            {
	 QMessageBox::warning( this, tr("ERROR"),
               tr("La cuota no puede ser nula"));
	 return;
            }
     
     QString cadena="";
    if (ui.CtabaselineEdit->text().length()<anchocuentas() ||  
	!existecodigoplan(ui.CtabaselineEdit->text(),&cadena) )  
            {
	if (ui.CtabaselineEdit->text().length()==0 && baseoblig)
	  {
	     QMessageBox::warning( this, tr("ERROR"),
                    tr("La cuenta base no es correcta"));
	   return;
                  }
	if (ui.CtabaselineEdit->text().length()!=0)
	  {
	     QMessageBox::warning( this, tr("ERROR"),
                    tr("La cuenta base no es correcta"));
	   return;
                  }
            }
     
    if ((ui.CtaivalineEdit->text().length()<anchocuentas() ||
        !existecodigoplan(ui.CtaivalineEdit->text(),&cadena)) && tipo>0.00001)
            {
	 QMessageBox::warning( this, tr("ERROR"),
               tr("La cuenta de iva no es correcta"));
	 return;
            }
    
    if (tipo>0.00001 && !escuentadeivasoportado(ui.CtaivalineEdit->text()))
            {
	     QMessageBox::warning( this, tr("ERROR"),
               tr("La cuenta de iva soportado no es correcta"));
	     return;
            }
    
    if (tipo>0.00001 && !escuentadeivarepercutido(ui.cuentareperlineEdit->text()))
            {
	     QMessageBox::warning( this, tr("ERROR"),
               tr("La cuenta de iva repercutido no es correcta"));
	     return;
            }
	    
	    
     accept();
}


void aib::recuperadatos(QString *cuentabase, QString *baseimponible, QString *qclaveiva,
                         QString *qtipoiva, QString *cuentaivasop, QString *cuentaivarep,
                         QString *cuotaiva, QString *ctafra, QDate *qfechafra,
                         QString *qprorrata, QString *qautofactura, QDate *fechaop,
                         QString *bicoste, QString *qautofacturanoue, QString *bieninversion,
                         QString *isp_op_interiores, QString *oro_inversion)
{
*cuentabase=ui.CtabaselineEdit->text();
*baseimponible=ui.baselineEdit->text();
QString cadenaiva=ui.ClaveivacomboBox->currentText();
QString extracto=cadenaiva.section("//",0,0);
*qclaveiva=extracto.remove(' ');
if (*qclaveiva==tipoivaexento()) *qclaveiva="";
*qtipoiva=ui.TipolineEdit->text();
*cuentaivasop=ui.CtaivalineEdit->text();
*cuentaivarep=ui.cuentareperlineEdit->text();
*cuotaiva=ui.CuotalineEdit->text();
*ctafra=ui.CtafralineEdit->text();
*qfechafra=ui.FechafradateEdit->date();
if (ui.prorratacheckBox->isChecked()) *qprorrata=ui.prorratalineEdit->text();
   else *qprorrata="";
if (ui.autofacturacheckBox->isChecked())
   *qautofactura="1";
  else
      *qautofactura="";
*fechaop=ui.fechaoperaciondateEdit->date();
*bicoste=ui.bicostelineEdit->text();
*qautofacturanoue=ui.autofacturaNoUecheckBox->isChecked() ? "1" : "";
*bieninversion=ui.bieninversioncheckBox->isChecked() ? "1" : "0";
*isp_op_interiores=ui.isp_interiorescheckBox->isChecked() ? "1" : "";
*oro_inversion=ui.oro_inversion_checkBox->isChecked() ? "1" : "";
}


void aib::chequeabotonaceptar()
{
    bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     double cuota = convapunto(ui.CuotalineEdit->text()).toDouble(& bien);
     if (!bien) cuota=0;
     
     if (tipo>0.0001)
       {
         if (!escuentadeivasoportado(ui.CtaivalineEdit->text()))
           { ui.AceptarButton->setEnabled(false); return; }
       }

     if (tipo>0.0001)
       {
        if (!escuentadeivarepercutido(ui.cuentareperlineEdit->text()))
          { ui.AceptarButton->setEnabled(false); return; }
       }

     if ((bi*tipo-cuota>=0.009 || bi*tipo-cuota<=-0.009) && tipo>0.0001)
        { ui.AceptarButton->setEnabled(false); return; }
	 
     if (bi<0.001 && bi>-0.001)
        { ui.AceptarButton->setEnabled(false); return; }
	 
     if (cuota<0.001 && cuota >-0.001 && tipo>0.0001)
        { ui.AceptarButton->setEnabled(false); return; }
     
     if (ui.CtabaselineEdit->text().length()==0 && baseoblig)
       { ui.AceptarButton->setEnabled(false); return; }

     if (ui.CtabaselineEdit->text().length()>0 && !esauxiliar(ui.CtabaselineEdit->text()))
       { ui.AceptarButton->setEnabled(false); return; }

    if (!esauxiliar(ui.CtafralineEdit->text()))
        {
         ui.AceptarButton->setEnabled(false); return;
        }

    if (!esauxiliar(ui.CtaivalineEdit->text()) && tipo>0.0001)
        { ui.AceptarButton->setEnabled(false); return; }
    
    if (!esauxiliar(ui.cuentareperlineEdit->text()) && tipo>0.0001)
        { ui.AceptarButton->setEnabled(false); return; }
    
    ui.AceptarButton->setEnabled(true);

}


/*void aib::cuotacambiada()
{
  bool bien=false;
  double cuota = convapunto(ui.CuotalineEdit->text()).toDouble(& bien);
  ui.CuotalineEdit->setText(formateanumero(cuota,comadecimal,decimales));
  chequeabotonaceptar();
}*/


void aib::pasadatos2( QString cuentabase, QString baseimponible, QString qclaveiva,
                      QString qtipoiva, QString cuentaiva, QString cuentareper,
                      QString cuotaiva, QString ctafra, QDate qfechafra, QDate fechaop,
                      QString bicoste,
                      bool binversion, bool oro_inversion)
{
ui.CtabaselineEdit->setText(cuentabase);
 if (cuentabase.length()>0) ui.basetextLabel->setText(descripcioncuenta(cuentabase));
ui.baselineEdit->setText(baseimponible);

QString cadenaiva;
QString extracto;

ui.CuotalineEdit->setText(cuotaiva); // pasamos de la cuota IVA

if (qclaveiva.length()==0) qclaveiva=tipoivaexento();
  for (int veces=0;veces<ui.ClaveivacomboBox->count();veces++)
     {
        cadenaiva=ui.ClaveivacomboBox->itemText(veces);
        extracto=cadenaiva.section("//",0,0);
        extracto=extracto.remove(' ');
        if (extracto==qclaveiva)
	{
	    ui.ClaveivacomboBox->setCurrentIndex(veces);
            comboivacambiado();
	    break;
	}
     }

if (qtipoiva.length()>0) ui.TipolineEdit->setText(qtipoiva);
ui.CtaivalineEdit->setText(cuentaiva);
ui.cuentareperlineEdit->setText(cuentareper);
// if (cuentaiva.length()>0) ui.ivatextLabel->setText(descripcioncuenta(cuentaiva));
// if (cuentareper.length()>0) ui.cuentarepertextLabel->setText(descripcioncuenta(cuentareper));
ui.CtafralineEdit->setText(ctafra);
if (ctafra.length()>0) ui.fratextLabel->setText(descripcioncuenta(ctafra));
ui.FechafradateEdit->setDate(qfechafra);
ui.fechaoperaciondateEdit->setDate(fechaop);
ui.bicostelineEdit->setText(bicoste);

ui.bieninversioncheckBox->setChecked(binversion);

ui.oro_inversion_checkBox->setChecked(oro_inversion);

}

//Función común para la busqueda de cuentas
void aib::buscacta(QLineEdit * lineedit)
{
    buscasubcuenta *labusqueda=new buscasubcuenta(lineedit->text());
    labusqueda->exec();
    QString cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) lineedit->setText(cadena2);
    else lineedit->setText("");
    delete labusqueda;
}

void aib::buscactabase()
{
    buscacta(ui.CtabaselineEdit);
}


void aib::buscactaiva()
{
    buscacta(ui.CtaivalineEdit);
}


void aib::buscactafra()
{
    buscacta(ui.CtafralineEdit);
}


void aib::buscactarep()
{
    buscacta(ui.cuentareperlineEdit);
}


void aib::hazbaseoblig()
{
  baseoblig=true;
  QDate fecha;
  ui.FechafradateEdit->setDate(fecha.currentDate());
  comboivacambiado();
}


void aib::selec_autofactura()
{
 ui.autofacturacheckBox->setChecked(true);
}

void aib::selec_autofactura_no_ue()
{
 ui.autofacturaNoUecheckBox->setChecked(true);
}

void aib::selec_isp_op_interiores()
{
 ui.isp_interiorescheckBox->setChecked(true);
}

void aib::fechafracambiada()
{
 ui.fechaoperaciondateEdit->setDate(ui.FechafradateEdit->date());
}


void aib::bicostecambiada()
{
  if (comadecimal)
          ui.bicostelineEdit->setText( convacoma(ui.bicostelineEdit->text()));
}

void aib::autofacturaue_cambiada()
{
    if (ui.autofacturacheckBox->isChecked())
      {
        ui.autofacturaNoUecheckBox->setChecked(false);
        ui.isp_interiorescheckBox->setChecked(false);
        ui.aibcheckBox->setChecked(false);
        // cta_iva_sop_AIS,  cta_iva_rep_AIS
        if (!cta_iva_sop_AIS.isEmpty()) ui.CtaivalineEdit->setText(cta_iva_sop_AIS);
        if (!cta_iva_rep_AIS.isEmpty()) ui.cuentareperlineEdit->setText(cta_iva_rep_AIS);
      }
      else chequea_checks();
}

void aib::autofacturanoue_cambiada()
{
    if (ui.autofacturaNoUecheckBox->isChecked())
    {
        ui.autofacturacheckBox->setChecked(false);
        ui.isp_interiorescheckBox->setChecked(false);
        ui.aibcheckBox->setChecked(false);
        // cta_iva_sop_AES, cta_iva_rep_AES
        if (!cta_iva_sop_AES.isEmpty()) ui.CtaivalineEdit->setText(cta_iva_sop_AES);
        if (!cta_iva_rep_AES.isEmpty()) ui.cuentareperlineEdit->setText(cta_iva_rep_AES);
    }
    else chequea_checks();
}

void aib::isp_interiores_cambiada()
{
   if (ui.isp_interiorescheckBox->isChecked())
    {
        ui.autofacturacheckBox->setChecked(false);
        ui.autofacturaNoUecheckBox->setChecked(false);
        ui.aibcheckBox->setChecked(false);
        // cta_iva_sop_OI, cta_iva_rep_OI
        if (!cta_iva_sop_OI.isEmpty()) ui.CtaivalineEdit->setText(cta_iva_sop_OI);
        if (!cta_iva_rep_OI.isEmpty()) ui.cuentareperlineEdit->setText(cta_iva_rep_OI);
    }
   else chequea_checks();
}

void aib::aib_check_cambiada()
{
 if (ui.aibcheckBox->isChecked())
    {
     ui.autofacturacheckBox->setChecked(false);
     ui.autofacturaNoUecheckBox->setChecked(false);
     ui.isp_interiorescheckBox->setChecked(false);
     // cta_iva_sop_AIB, cta_iva_rep_AIB
     if (!cta_iva_sop_AIB.isEmpty()) ui.CtaivalineEdit->setText(cta_iva_sop_AIB);
     if (!cta_iva_rep_AIB.isEmpty()) ui.cuentareperlineEdit->setText(cta_iva_rep_AIB);
    }
 else chequea_checks();
}

void aib::chequea_checks()
{
    if (!ui.autofacturacheckBox->isChecked() &&
        !ui.autofacturaNoUecheckBox->isChecked() &&
        !ui.isp_interiorescheckBox->isChecked() &&
        !ui.aibcheckBox->isChecked())
        ui.aibcheckBox->setChecked(true);
}

void aib::modoconsulta()
{
    ui.autofacturacheckBox->setEnabled(false);
    ui.autofacturaNoUecheckBox->setEnabled(false);
    ui.aibcheckBox->setEnabled(false);
    ui.isp_interiorescheckBox->setEnabled(false);
    ui.baselineEdit->setReadOnly(true);
    ui.bicostelineEdit->setEnabled(false);
    ui.bieninversioncheckBox->setEnabled(false);
    ui.ClaveivacomboBox->setEnabled(false);
    ui.CtabaselineEdit->setReadOnly(true);
    ui.ctabasepushButton->setEnabled(false);
    ui.ClaveivacomboBox->setEnabled(false);
    ui.CtafralineEdit->setReadOnly(true);
    ui.CtaivalineEdit->setReadOnly(true);
    ui.ctaivapushButton->setEnabled(false);
    ui.cuentareperlineEdit->setReadOnly(true);
    ui.CuotalineEdit->setReadOnly(true);
    ui.FechafradateEdit->setReadOnly(true);
    ui.fechaoperaciondateEdit->setReadOnly(true);
    ui.prorratacheckBox->setEnabled(false);
    ui.prorratalineEdit->setReadOnly(true);
    ui.reppushButton->setEnabled(false);
    ui.TipolineEdit->setReadOnly(true);

    ui.externo_lineEdit->setReadOnly(true);
    ui.externo_pushButton->setEnabled(false);
    ui.oro_inversion_checkBox->setEnabled(false);
}

QString aib::externo()
{
   return ui.externo_lineEdit->text();
}

void aib::esconde_externo()
{
    ui.externo_lineEdit->hide();
    ui.externo_pushButton->hide();
    ui.descrip_externo_label->hide();
}

void aib::buscar_externo()
{
    busca_externo *b = new busca_externo();
    if (b->exec()==QDialog::Accepted)
      {
        ui.externo_lineEdit->setText(b->codigo_elec());
      }
      else ui.externo_lineEdit->clear();
    delete(b);
}


void aib::externo_cambiado()
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

