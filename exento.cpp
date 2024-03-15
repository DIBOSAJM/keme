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

#include "exento.h"
#include "funciones.h"
#include <QMessageBox>
#include "aux_express.h"
#include "buscasubcuenta.h"
#include "basedatos.h"
#include <QJsonDocument>

exento::exento(bool concomadecimal, bool condecimales) : QDialog() {
    ui.setupUi(this);

    comadecimal=concomadecimal;
    decimales=condecimales;
 cargacombooperaciones();

  ui.tipo_rectificativa_comboBox->addItems(tipos_rectificativas);

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

  connect(ui.recibidasradioButton,SIGNAL(toggled(bool)),SLOT(cheqrecibidas_activado()));
  connect(ui.emitidasradioButton,SIGNAL(toggled(bool)),SLOT(cheqemitidas_activado()));

  connect(ui.exportacionheckBox,SIGNAL(toggled(bool)),SLOT(exportacion_activado()));
  connect(ui.ventas_fuera_tac_checkBox,SIGNAL(toggled(bool)),SLOT(vtas_fuera_tac_activado()));

  cheqrecibidas_activado();
 }

void exento::fuerzaemitidas()
{
  ui.emitidasradioButton->setChecked(true);
}


void exento::pasadatos(QString cuentabase, QString baseimponible, QString ctafra,
                        QDate qfechafra, QDate qfechaop,
                                    QString qclaveop,
                                    QString rectificativa, QString frectif,  QString numfact,
                               QString facini, QString facfinal, QString binversion,
                               QString op_no_sujeta, QString cliente, QString cif,
                               QString importacion, QString exportacion,
                               QString exenta_no_deduc, QString ventas_fuera_tac, QString emitida_isp_int, QString info_donacion, QString dua)
{
 QJsonDocument doc;
 QJsonObject obj_donacion;

 if (!info_donacion.isEmpty()) {
    doc = QJsonDocument::fromJson(info_donacion.toUtf8());
    obj_donacion=doc.object();
    ui.donacion_checkBox->setChecked(true);
    QString cod_clave=obj_donacion["clave"].toString();
    for (int v=0; v< ui.clave_donacion_comboBox->count(); v++) {
        if (ui.clave_donacion_comboBox->itemText(v).left(1)==cod_clave) {
            ui.clave_donacion_comboBox->setCurrentIndex(v);
            break;
        }
    }
    ui.donacion_especie_checkBox->setChecked(obj_donacion["especie"].toBool());
    QString ca=obj_donacion["ca"].toString();
    for (int v=0; v< ui.ca_comboBox->count(); v++) {
        if (ui.ca_comboBox->itemText(v).left(2)==ca) {
            ui.ca_comboBox->setCurrentIndex(v);
            break;
        }
    }
    ui.recurrente_checkBox->setChecked(obj_donacion["recurrente"].toBool());
    ui.porcentaje_deduccion_ca_lineEdit->setText(formateanumero_ndec(obj_donacion["deduccion_ca"].toDouble(),comadecimal,2));
 }


 ui.CtabaselineEdit->setText(cuentabase);
 if (cuentabase.length()>0) ui.basetextLabel->setText(descripcioncuenta(cuentabase));
 ui.baselineEdit->setText(baseimponible);
 ui.CtafralineEdit->setText(ctafra);
 if (ctafra.length()>0) ui.fratextLabel->setText(descripcioncuenta(ctafra));
 ui.FechafradateEdit->setDate(qfechafra);
 ui.fechaoperaciondateEdit->setDate(qfechaop);
 for (int veces=0;veces<ui.clavecomboBox->count();veces++)
     {
        QString cadenaclave=ui.clavecomboBox->itemText(veces);
        QString extracto=cadenaclave.section(" ",0,0);
        extracto=extracto.remove(' ');
        if (extracto==qclaveop)
           {
            ui.clavecomboBox->setCurrentIndex(veces);
            break;
           }
     }

  if (rectificativa=="1") {
     ui.rectificativagroupBox->setChecked(true);
     for (int veces=0; veces<ui.tipo_rectificativa_comboBox->count(); veces++)
       {
        if (ui.tipo_rectificativa_comboBox->itemText(veces).left(2)==frectif.left(2))
          ui.tipo_rectificativa_comboBox->setCurrentIndex(veces);
        if (frectif.length()>3) ui.rectificada_lineEdit->setText(frectif.mid(3));
       }
    }
    else ui.rectificativagroupBox->setChecked(false);

 ui.imp_exentacheckBox->setChecked(importacion=="1");
 ui.exportacionheckBox->setChecked(exportacion=="1");
 ui.exenta_no_deduccheckBox->setChecked(exenta_no_deduc=="1");

 ui.numfacturaslineEdit->setText(numfact);
 ui.iniciallineEdit->setText(facini);
 ui.finallineEdit->setText(facfinal);

 ui.bieninversioncheckBox->setChecked(binversion=="1");
 ui.nosujetacheckBox->setChecked(op_no_sujeta=="1");

 ui.nombrelineEdit->setText(cliente);
 ui.ciflineEdit->setText(cif);

 if (ventas_fuera_tac=="1")
     ui.ventas_fuera_tac_checkBox->setChecked(true);

 ui.expedida_isp_checkBox->setChecked(emitida_isp_int=="1");

 if (!dua.isEmpty()) ui.DUAlineEdit->setText(dua);

}



void exento::ctabasecambiada()
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

void exento::ctabasefinedicion()
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


void exento::ctafracambiada()
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

void exento::ctafrafinedicion()
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


void exento::botonaceptar()
{     
    bool ok;
    double valor=convapunto(ui.baselineEdit->text()).toDouble(&ok);
    if ((valor<0.0001 && valor>-0.0001) || !ok)
	        {
              //QMessageBox::warning( this, "ERROR",
              //      tr("El valor de la base no puede ser nulo"));
              //return;
            }


    if (!esauxiliar(ui.CtabaselineEdit->text()))
            {
	      if (ui.CtabaselineEdit->text().length()==0)
	        {
	          QMessageBox::warning( this, "ERROR",
                    tr("La cuenta base no es correcta"));
	          return;
                }
	      if (ui.CtabaselineEdit->text().length()!=0)
	        {
	         QMessageBox::warning( this, "ERROR",
                    tr("La cuenta base no es correcta"));
	         return;
                }
            }

    if (!esauxiliar(ui.CtafralineEdit->text())) 
            {
	      QMessageBox::warning( this, "ERROR",
               tr("La cuenta de factura no es correcta"));
	      return;
            }

     accept();
}


void exento::recuperadatos(QString *cuentabase, QString *baseimponible,  QString *ctafra,
                            QDate *qfechafra, QDate *qfechaop,
                            QString *qclaveop, QString *rectificativa, QString *frectif,
                                    QString *numfact,
                                    QString *facini, QString *facfinal, QString *bieninversion,
                                    QString *op_no_sujeta, QString *nombre, QString *cif,
                                    QString *importacion, QString *exportacion,
                                    QString *exenta_no_deduc, QString *vta_fuera_tac, QString *emitida_isp_int, QString *info_donacion, QString *dua)
{
*cuentabase=ui.CtabaselineEdit->text();
*baseimponible=convapunto(ui.baselineEdit->text());
if (baseimponible->isEmpty() || *baseimponible=="-") *baseimponible="0";
*ctafra=ui.CtafralineEdit->text();
*qfechafra=ui.FechafradateEdit->date();
*qfechaop=ui.fechaoperaciondateEdit->date();
QString cadenaclave=ui.clavecomboBox->currentText();
QString extracto=cadenaclave.section(' ',0,0);
*qclaveop=extracto.remove(' ');

if (ui.rectificativagroupBox->isChecked()) *rectificativa="1";
   else *rectificativa="0";

if (ui.rectificativagroupBox->isChecked()) {
    *frectif=ui.tipo_rectificativa_comboBox->currentText().left(2);
    if (!ui.rectificada_lineEdit->text().isEmpty()) *frectif+=":"+ui.rectificada_lineEdit->text();
}
  else *frectif="";
*numfact=ui.numfacturaslineEdit->text();
*facini=ui.iniciallineEdit->text();
*facfinal=ui.finallineEdit->text();
*bieninversion=ui.bieninversioncheckBox->isChecked() ? "1" : "0";
*op_no_sujeta=ui.nosujetacheckBox->isChecked() ? "1" : "0";
*nombre=ui.nombrelineEdit->text();
*cif=ui.ciflineEdit->text();
*importacion=ui.imp_exentacheckBox->isChecked() ? "1" : "";
*exportacion=ui.exportacionheckBox->isChecked() ? "1" : "";
*exenta_no_deduc=ui.exenta_no_deduccheckBox->isChecked() ? "1" : "";
*vta_fuera_tac=ui.ventas_fuera_tac_checkBox->isChecked() ? "1" :"";
*emitida_isp_int=ui.expedida_isp_checkBox->isChecked() ? "1" : "";
if (ui.donacion_checkBox->isChecked()) {
   QJsonObject reg_donacion;
   reg_donacion.insert("clave",ui.clave_donacion_comboBox->currentText().left(1));
   reg_donacion.insert("especie",ui.donacion_especie_checkBox->isChecked());
   reg_donacion.insert("recurrente",ui.recurrente_checkBox->isChecked());
   reg_donacion.insert("ca",ui.ca_comboBox->currentText().left(2));
   reg_donacion.insert("deduccion_ca",convapunto(ui.porcentaje_deduccion_ca_lineEdit->text()).toDouble());
   QJsonDocument doc(reg_donacion);
   *info_donacion=doc.toJson();
  }
if (!ui.DUAlineEdit->text().isEmpty()) *dua=ui.DUAlineEdit->text();
   else *dua=QString();
}


void exento::chequeabotonaceptar()
{
    // QString cadena="";
    if (!esauxiliar(ui.CtabaselineEdit->text()))
        { 
	if (ui.CtabaselineEdit->text().length()!=0)
       { ui.AceptarButton->setEnabled(false); return; }
	if (ui.CtabaselineEdit->text().length()==0)
       { ui.AceptarButton->setEnabled(false); return; }
        }

    if (!esauxiliar(ui.CtafralineEdit->text()))
        { ui.AceptarButton->setEnabled(false); return; }

    ui.AceptarButton->setEnabled(true);

}

void exento::buscactabase()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtabaselineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cadena2.length()>0) ui.CtabaselineEdit->setText(cadena2);
       else ui.CtabaselineEdit->setText("");
    delete(labusqueda);
}

void exento::buscactafra()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtafralineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cadena2.length()>0) ui.CtafralineEdit->setText(cadena2);
       else ui.CtafralineEdit->setText("");
    delete(labusqueda);
}

bool exento::eslibroemitidas()
{
 return ui.emitidasradioButton->isChecked();
}

void exento::cargacombooperaciones()
{
  QStringList lista=basedatos::instancia()->listaoperaciones_expedidas();
  QStringList lista2=basedatos::instancia()->listaoperaciones_recibidas();
  for (int veces=0;veces<lista2.count();veces++)
     {
        QString cadenaclave=lista2.at(veces);
        QString extracto=cadenaclave.section(" ",0,0);
        extracto=extracto.remove(' ');
        int veces2;
        for (veces2=0;veces2<lista.count();veces2++)
           {
            QString extracto2=lista.at(veces2).section(" ",0,0);
            if (extracto2==extracto) break;
           }
        if (veces2>=lista.count())
            // añadimos cadenaclave a lista
            lista << cadenaclave;
     }
  lista.sort();
  ui.clavecomboBox->addItems (lista);

}

void exento::fechafracambiada()
{
 ui.fechaoperaciondateEdit->setDate(ui.FechafradateEdit->date());
}


void exento::modoconsulta()
{
  ui.baselineEdit->setReadOnly(true);
  ui.bieninversioncheckBox->setEnabled(false);
  ui.imp_exentacheckBox->setEnabled(false);
  ui.exportacionheckBox->setEnabled(false);
  ui.exenta_no_deduccheckBox->setEnabled(false);
  ui.clavecomboBox->setEnabled(false);
  ui.CtabaselineEdit->setReadOnly(true);
  ui.ctabasepushButton->setEnabled(false);
  ui.ctabasepushButton->setEnabled(false);
  ui.CtafralineEdit->setReadOnly(true);
  ui.ctafrapushButton->setEnabled(false);
  ui.emitidasradioButton->setEnabled(false);
  ui.FechafradateEdit->setReadOnly(true);
  ui.fechaoperaciondateEdit->setReadOnly(true);
  ui.finallineEdit->setReadOnly(true);
  ui.iniciallineEdit->setReadOnly(true);
  ui.nosujetacheckBox->setEnabled(false);
  ui.numfacturaslineEdit->setReadOnly(true);
  ui.recibidasradioButton->setEnabled(false);
  ui.tipo_rectificativa_comboBox->setEnabled(false);
  ui.rectificativagroupBox->setEnabled(false);
  ui.nombrelineEdit->setReadOnly(true);
  ui.ciflineEdit->setReadOnly(true);
  ui.ventas_fuera_tac_checkBox->setEnabled(false);
  ui.expedida_isp_checkBox->setEnabled(false);
  ui.donacion_checkBox->setEnabled(false);
  ui.donacion_groupBox->setEnabled(false);
  ui.DUAlineEdit->setReadOnly(true);

}


void exento::cheqrecibidas_activado()
{
    if (ui.recibidasradioButton->isChecked())
       {
        ui.exenta_no_deduccheckBox->setChecked(false);
        ui.ventas_fuera_tac_checkBox->setChecked(false);
        ui.exportacionheckBox->setChecked(false);
        ui.exenta_no_deduccheckBox->setEnabled(false);
        ui.ventas_fuera_tac_checkBox->setEnabled(false);
        ui.exportacionheckBox->setEnabled(false);
        ui.expedida_isp_checkBox->setEnabled(false);

        ui.imp_exentacheckBox->setEnabled(true);

        ui.donacion_checkBox->setChecked(false);
        ui.donacion_checkBox->hide();
        ui.donacion_groupBox->hide();
       }
}

void exento::cheqemitidas_activado()
{
    if (ui.emitidasradioButton->isChecked())
      {
       ui.imp_exentacheckBox->setChecked(false);
       ui.imp_exentacheckBox->setEnabled(false);

       ui.exenta_no_deduccheckBox->setEnabled(true);
       ui.ventas_fuera_tac_checkBox->setEnabled(true);
       ui.exportacionheckBox->setEnabled(true);
       ui.expedida_isp_checkBox->setEnabled(true);
       ui.donacion_checkBox->show();
      }
}


void exento::exportacion_activado()
{
    if (ui.exportacionheckBox->isChecked())
        ui.ventas_fuera_tac_checkBox->setChecked(false);
}

void exento::vtas_fuera_tac_activado()
{
    if (ui.ventas_fuera_tac_checkBox->isChecked())
        ui.exportacionheckBox->setChecked(false);
}

void exento::on_donacion_checkBox_toggled(bool checked)
{
    if (checked) ui.donacion_groupBox->show();
       else ui.donacion_groupBox->hide();
}

void exento::on_imp_exentacheckBox_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    if (ui.imp_exentacheckBox->isChecked()) {
        ui.dua_label->setEnabled(true);
        ui.DUAlineEdit->setEnabled(true);
    }
    else {
          ui.dua_label->setEnabled(false);
          ui.DUAlineEdit->setEnabled(false);
    }
}
