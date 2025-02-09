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

#include "ivasop_aut.h"
#include "funciones.h"
#include "basedatos.h"
#include "aux_express.h"
#include "busca_externo.h"
#include <QMessageBox>
#include <QFileDialog>

ivasop_aut::ivasop_aut(bool concomadecimal, bool condecimales) : QDialog() {
    ui.setupUi(this);


    etiqueta = new CustomQLabel;
    etiqueta->setHidden(false);
    etiqueta->setMinimumWidth(150);
    etiqueta->setMaximumWidth(150);
    etiqueta->setMinimumHeight(60);
    etiqueta->setText(tr("Soltar archivo aquí"));
    etiqueta->setAlignment(Qt::AlignHCenter);
    etiqueta->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    ui.horizontalLayout_7->addWidget(etiqueta);

    if (conigic())
       {
        setWindowTitle(tr("IGIC soportado"));
        ui.TextLabel3->setText(tr("Clave IGIC"));
        ui.textLabel4->setText(tr("Cuenta IGIC"));
        ui.textLabel7->setText(tr("Cuota IGIC"));

        ui.TextLabel3_3->setText(tr("Clave IGIC"));
        ui.textLabel4_3->setText(tr("Cuenta IGIC"));
        ui.textLabel7_3->setText(tr("Cuota IGIC"));

        ui.TextLabel3_5->setText(tr("Clave IGIC"));
        ui.textLabel4_5->setText(tr("Cuenta IGIC"));
        ui.textLabel7_5->setText(tr("Cuota IGIC"));

        ui.groupBox->setTitle("IGIC 1");
        ui.groupBox2->setTitle("IGIC 2");
        ui.groupBox3->setTitle("IGIC 3");
       }
    comadecimal=concomadecimal;
    decimales=condecimales;

    ui.tipo_rectificativa_comboBox->addItems(tipos_rectificativas);

  // Tenemos que cargar clave IVA por defecto y resto de IVA'S en el combo box
    ui.exento_groupBox->hide();
    // QString pasalinea;
    QString Clavedefecto; //, laclave;
    double tipo;
    double re;
    QString qdescrip;
    // QString convert;
    Clavedefecto=clave_iva_defecto();
    if (Clavedefecto.length()>0 && existeclavetipoiva(Clavedefecto,&tipo,&re,&qdescrip))
        {
	ui.ClaveivacomboBox->insertItem ( 0,Clavedefecto) ;
	ui.ClaveivacomboBox2->insertItem ( 0,Clavedefecto) ;
	ui.ClaveivacomboBox3->insertItem ( 0,Clavedefecto) ;
        }

    ui.FechafradateEdit->setDate(QDate::currentDate());
    ui.fechacontdateEdit->setDate(QDate::currentDate());
    ui.fechaoperaciondateEdit->setDate(QDate::currentDate());

    // ahora cargamos el resto de los tipos de IVA
 QSqlQuery query = basedatos::instancia()->selectClavetiposivanoclave(Clavedefecto);
 
 if ( query.isActive() )
    {
      while (query.next())
         {
	  ui.ClaveivacomboBox->insertItem ( -1,query.value(0).toString()) ;
	  ui.ClaveivacomboBox2->insertItem ( -1,query.value(0).toString()) ;
	  ui.ClaveivacomboBox3->insertItem ( -1,query.value(0).toString()) ;
         }
      }
 comboivacambiado();
 comboivacambiado2();
 comboivacambiado3();
 cargacombooperaciones();

 ui.clavecomboBox->addItems(basedatos::instancia()->listaoperaciones_ret());;

 connect(ui.ClaveivacomboBox,SIGNAL(activated(int)),this,SLOT(comboivacambiado()));
 connect(ui.ClaveivacomboBox2,SIGNAL(activated(int)),this,SLOT(comboivacambiado2()));
 connect(ui.ClaveivacomboBox3,SIGNAL(activated(int)),this,SLOT(comboivacambiado3()));

 connect(ui.CtabaselineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctabasecambiada()));
 connect(ui.CtabaselineEdit,SIGNAL(editingFinished()),this,SLOT(ctabasefinedicion()));
 connect(ui.CtabaselineEdit_2,SIGNAL(textChanged(QString)),this,SLOT(ctabasecambiada2()));
 connect(ui.CtabaselineEdit_2,SIGNAL(editingFinished()),this,SLOT(ctabasefinedicion2()));
 connect(ui.CtabaselineEdit_3,SIGNAL(textChanged(QString)),this,SLOT(ctabasecambiada3()));
 connect(ui.CtabaselineEdit_3,SIGNAL(editingFinished()),this,SLOT(ctabasefinedicion3()));
 connect(ui.Ctabase_exenta_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctabase_exenta_cambiada()));
 connect(ui.Ctabase_exenta_lineEdit,SIGNAL(editingFinished()),this,SLOT(ctabase_exenta_finedicion()));

 connect(ui.CtaivalineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctaivacambiada()));
 connect(ui.CtaivalineEdit,SIGNAL(editingFinished()),this,SLOT(ctaivafinedicion()));
 connect(ui.CtaivalineEdit2,SIGNAL(textChanged(QString)),this,SLOT(ctaivacambiada2()));
 connect(ui.CtaivalineEdit2,SIGNAL(editingFinished()),this,SLOT(ctaivafinedicion2()));
 connect(ui.CtaivalineEdit3,SIGNAL(textChanged(QString)),this,SLOT(ctaivacambiada3()));
 connect(ui.CtaivalineEdit3,SIGNAL(editingFinished()),this,SLOT(ctaivafinedicion3()));

 connect(ui.CtafralineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctafracambiada()));
 connect(ui.CtafralineEdit,SIGNAL(editingFinished()),this,SLOT(ctafrafinedicion()));

 connect(ui.baselineEdit,SIGNAL(textChanged(QString)),this,SLOT(basecambiada()));
 connect(ui.baselineEdit,SIGNAL(editingFinished()),this,SLOT(basefinedicion()));
 connect(ui.baselineEdit2,SIGNAL(textChanged(QString)),this,SLOT(basecambiada2()));
 connect(ui.baselineEdit2,SIGNAL(editingFinished()),this,SLOT(basefinedicion2()));
 connect(ui.baselineEdit3,SIGNAL(textChanged(QString)),this,SLOT(basecambiada3()));
 connect(ui.baselineEdit3,SIGNAL(editingFinished()),this,SLOT(basefinedicion3()));
 connect(ui.base_exentalineEdit,SIGNAL(textChanged(QString)),this,SLOT(base_exenta_cambiada()));
 connect(ui.base_exentalineEdit,SIGNAL(editingFinished()),this,SLOT(base_exenta_finedicion()));

 connect(ui.totalfralineEdit,SIGNAL(editingFinished()),this,SLOT(totalfrafinedicion()));
 connect(ui.AceptarButton,SIGNAL(clicked()),this,SLOT(botonaceptar()));

 connect(ui.ctabasepushButton,SIGNAL(clicked()),this,SLOT(buscactabase()));
 connect(ui.ctaivapushButton,SIGNAL(clicked()),this,SLOT(buscactaiva()));
 connect(ui.ctafrapushButton,SIGNAL(clicked()),this,SLOT(buscactafra()));

 connect(ui.ctabasepushButton_2,SIGNAL(clicked()),this,SLOT(buscactabase2()));
 connect(ui.ctaivapushButton2,SIGNAL(clicked()),this,SLOT(buscactaiva2()));
 
 connect(ui.ctabasepushButton_3,SIGNAL(clicked()),this,SLOT(buscactabase3()));
 connect(ui.ctaivapushButton3,SIGNAL(clicked()),this,SLOT(buscactaiva3()));
 
 connect(ui.ctabase_exenta_pushButton,SIGNAL(clicked()),this,SLOT(buscactabase_exenta()));


 connect(ui.CtabaselineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));
 connect(ui.CtabaselineEdit_2,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));
 connect(ui.CtabaselineEdit_3,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));
 connect(ui.Ctabase_exenta_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));

 connect(ui.CtaivalineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));
 connect(ui.CtaivalineEdit2,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));
 connect(ui.CtaivalineEdit3,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));
 connect(ui.CtafralineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));

connect(ui.fichdocpushButton ,SIGNAL(clicked()),SLOT(buscacopiadoc()));
connect(ui.visdocpushButton ,SIGNAL(clicked()),SLOT(visdoc()));
connect(ui.borraasdocpushButton ,SIGNAL(clicked()),SLOT(borraasignaciondoc()));

connect(ui.groupBox2,SIGNAL(toggled(bool)),this,SLOT(grboxcambiado1()));
 connect(ui.groupBox3,SIGNAL(toggled(bool)),this,SLOT(grboxcambiado2()));

 connect(ui.FechafradateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(fechafracambiada()));

 connect(ui.afectacionlineEdit,SIGNAL(editingFinished()),this,SLOT(afectacionfinedicion()));
 connect(ui.documentolineEdit,SIGNAL(textChanged(QString)),this,SLOT(concepto_cambiado()));

 connect(ui.ctaretpushButton,SIGNAL(clicked()),this,SLOT(buscactaret()));
 connect(ui.cuentaretlineEdit,SIGNAL(editingFinished()),this,SLOT(ctaretfinedic()));
 connect(ui.tiporetlineEdit,SIGNAL(editingFinished()),this,SLOT(tiporetfinedic()));

 connect(ui.arrendamientocheckBox,SIGNAL(stateChanged(int)),this,SLOT(arrendamiento_cambiado()));

 connect(ui.exento_checkBox,SIGNAL(stateChanged(int)),this,SLOT(iva_exento_cambiado()));

 connect(ui.externo_pushButton,SIGNAL(clicked(bool)),this,SLOT(buscar_externo()));

 connect(ui.externo_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(externo_cambiado()));

 connect(etiqueta, SIGNAL(filedroped()),SLOT(fichero_soltado()));

 // averiguamos si hay prorrata y si es especial
 double prorrata=prorrata_iva();
 bool hay_prorrata=basedatos::instancia()->hay_prorrata();
 if (hay_prorrata)
    {
     ui.prorratalineEdit->setEnabled(true);
     ui.prorratacheckBox->setEnabled(true);
     QString cadnum; cadnum.setNum(prorrata,'f',0);
     ui.prorratalineEdit->setText(cadnum);
     if (!prorrata_especial())
        {
         ui.prorratacheckBox->setChecked(true);
         ui.prorratalineEdit->setReadOnly(true);
         ui.prorratacheckBox->setEnabled(false);
        }
    }
 ui.CtabaselineEdit->setFocus();
 if (basedatos::instancia()->hay_criterio_caja_config())
     ui.cajacheckBox->setChecked(true);

 if (!basedatos::instancia()->hay_externos())
 {
  ui.externo_label->hide();
  ui.externo_lineEdit->hide();
  ui.externo_pushButton->hide();
  ui.descrip_externo_label->hide();
 }

}


void ivasop_aut::actutotalfra()
{
     // ctaretfinedic();
     QString cadena;
     double total=convapunto(ui.baselineEdit->text()).toDouble()+
                  convapunto(ui.CuotalineEdit->text()).toDouble()+
                  convapunto(ui.baselineEdit2->text()).toDouble()+
                  convapunto(ui.CuotalineEdit2->text()).toDouble()+
                  convapunto(ui.baselineEdit3->text()).toDouble()+
                  convapunto(ui.CuotalineEdit3->text()).toDouble()+
                  convapunto(ui.base_exentalineEdit->text()).toDouble();

     QString retencion;
     double tipo_ret_cta=0;
     if (!ui.cuentaretlineEdit->text().isEmpty() && !ui.tiporetlineEdit->text().isEmpty())
       {
        tipo_ret_cta=convapunto(ui.tiporetlineEdit->text()).toDouble();
        retencion.setNum(redond(tipo_ret_cta*(
                          convapunto(ui.baselineEdit->text()).toDouble() +
                          convapunto(ui.baselineEdit2->text()).toDouble() +
                          convapunto(ui.baselineEdit3->text()).toDouble() +
                          convapunto(ui.base_exentalineEdit->text()).toDouble()
                          )/100,2),'f',2);
         ui.retencionlineEdit->setText(comadecimal ? convacoma(retencion) : retencion);
       }
       else
           {
            ui.retencionlineEdit->clear();
           }

     if (!ui.retencionlineEdit->text().isEmpty())
         total=total-convapunto(ui.retencionlineEdit->text()).toDouble();
     if (decimales)
         cadena.setNum(total,'f',2);
       else cadena.setNum(total,'f',0);
     if (comadecimal) ui.totalfralineEdit->setText( convacoma(cadena));
         else ui.totalfralineEdit->setText(cadena);
}

void ivasop_aut::comboivacambiado()
{
QString codiva=ui.ClaveivacomboBox->currentText();
QString tipoc,re;
cadvalorestipoiva(codiva,&tipoc,&re);
if (comadecimal) tipoc=convacoma(tipoc);
ui.TipolineEdit->setText(tipoc);

ui.CtaivalineEdit->setText(basedatos::instancia()->cuenta_tipo_iva_sop(codiva));

    bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     int ndecimales= decimales ? 2 : 0;
     double cuota=CutAndRoundNumberToNDecimals (bi*tipo, ndecimales);
     QString cadena;
     if (decimales)
         cadena.setNum(cuota,'f',2);
       else cadena.setNum(cuota,'f',0);
     if (comadecimal) ui.CuotalineEdit->setText( convacoma(cadena));
         else ui.CuotalineEdit->setText(cadena);
 
  actutotalfra();

   chequeabotonaceptar();
}


void ivasop_aut::comboivacambiado2()
{
QString codiva=ui.ClaveivacomboBox2->currentText();
QString tipoc,re;
cadvalorestipoiva(codiva,&tipoc,&re);
if (comadecimal) tipoc=convacoma(tipoc);
ui.TipolineEdit2->setText(tipoc);

ui.CtaivalineEdit2->setText(basedatos::instancia()->cuenta_tipo_iva_sop(codiva));

    bool bien;
    double bi = convapunto(ui.baselineEdit2->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit2->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     int ndecimales= decimales ? 2 : 0;
     double cuota=CutAndRoundNumberToNDecimals (bi*tipo, ndecimales);
     QString cadena;
     if (decimales)
         cadena.setNum(cuota,'f',2);
       else cadena.setNum(cuota,'f',0);
     if (comadecimal) ui.CuotalineEdit2->setText( convacoma(cadena));
         else ui.CuotalineEdit2->setText(cadena);

  actutotalfra();

   chequeabotonaceptar();
}


void ivasop_aut::comboivacambiado3()
{
QString codiva=ui.ClaveivacomboBox3->currentText();
QString tipoc,re;
cadvalorestipoiva(codiva,&tipoc,&re);
if (comadecimal) tipoc=convacoma(tipoc);
ui.TipolineEdit3->setText(tipoc);

ui.CtaivalineEdit3->setText(basedatos::instancia()->cuenta_tipo_iva_sop(codiva));

    bool bien;
    double bi = convapunto(ui.baselineEdit3->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit3->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     int ndecimales= decimales ? 2 : 0;
     double cuota=CutAndRoundNumberToNDecimals (bi*tipo, ndecimales);
     QString cadena;
     if (decimales)
         cadena.setNum(cuota,'f',2);
       else cadena.setNum(cuota,'f',0);
     if (comadecimal) ui.CuotalineEdit3->setText( convacoma(cadena));
         else ui.CuotalineEdit3->setText(cadena);

  actutotalfra();

   chequeabotonaceptar();
}


void ivasop_aut::ctabasecambiada()
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
	           if (!esauxiliar(ui.CtabaselineEdit->text()))
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


void ivasop_aut::ctabasecambiada2()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);

    QString cadena;
    if (ui.CtabaselineEdit_2->text().length()==anchocuentas() && !cod_longitud_variable()) {
	if (!existecodigoplan(ui.CtabaselineEdit_2->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.CtabaselineEdit_2->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2); 
	           if (!esauxiliar(ui.CtabaselineEdit_2->text()))
			ui.CtabaselineEdit_2->setText("");	     
		 }
		  else
	            ui.CtabaselineEdit_2->setText("");	     
            chequeabotonaceptar();
	   }
        }
   if (esauxiliar(ui.CtabaselineEdit_2->text()))
       ui.basetextLabel_2->setText(descripcioncuenta(ui.CtabaselineEdit_2->text()));
     else ui.basetextLabel_2->setText("");
}

void ivasop_aut::ctabasecambiada3()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
 QString cadena;
    if (ui.CtabaselineEdit_3->text().length()==anchocuentas() && !cod_longitud_variable()) {
	if (!existecodigoplan(ui.CtabaselineEdit_3->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.CtabaselineEdit_3->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2); 
	           if (!esauxiliar(ui.CtabaselineEdit_3->text()))
			ui.CtabaselineEdit_3->setText("");
		 }
		  else
	            ui.CtabaselineEdit_3->setText("");
            chequeabotonaceptar();
	   }
        }
   if (esauxiliar(ui.CtabaselineEdit_3->text()))
       ui.basetextLabel_3->setText(descripcioncuenta(ui.CtabaselineEdit_3->text()));
     else ui.basetextLabel_3->setText("");
}


void ivasop_aut::ctabase_exenta_cambiada()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
 QString cadena;
    if (ui.Ctabase_exenta_lineEdit->text().length()==anchocuentas() && !cod_longitud_variable()) {
    if (!existecodigoplan(ui.Ctabase_exenta_lineEdit->text(),&cadena))
      {
     // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
        QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
        if (msgBox.clickedButton() == aceptarButton)
             {
              aux_express *lasubcuenta2=new aux_express();
              lasubcuenta2->pasacodigo(ui.Ctabase_exenta_lineEdit->text());
              lasubcuenta2->exec();
              delete(lasubcuenta2);
              if (!esauxiliar(ui.Ctabase_exenta_lineEdit->text()))
                 ui.Ctabase_exenta_lineEdit->setText("");
             }
             else
                ui.Ctabase_exenta_lineEdit->setText("");
            chequeabotonaceptar();
       }
        }
   if (esauxiliar(ui.Ctabase_exenta_lineEdit->text()))
       ui.base_exenta_textLabel->setText(descripcioncuenta(ui.Ctabase_exenta_lineEdit->text()));
     else ui.base_exenta_textLabel->setText("");
}


void ivasop_aut::ctabasefinedicion()
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
         {
          ui.basetextLabel->setText(descripcioncuenta(ui.CtabaselineEdit->text()));
          // if (!ui.CtaivalineEdit->text().isEmpty()) ui.baselineEdit->setFocus();
         }
         else ui.basetextLabel->setText("");
      }
}

void ivasop_aut::ctabasefinedicion2()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
 QString cadena;
  ui.CtabaselineEdit_2->setText(expandepunto(ui.CtabaselineEdit_2->text(),anchocuentas()));
  if (ui.CtabaselineEdit_2->text().length()>3 && cod_longitud_variable()) {
	if (!existecodigoplan(ui.CtabaselineEdit_2->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.CtabaselineEdit_2->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2); 
	           if (!esauxiliar(ui.CtabaselineEdit_2->text()))
			ui.CtabaselineEdit_2->setText("");	     
		 }
		  else
	            ui.CtabaselineEdit_2->setText("");	     
            chequeabotonaceptar();
	   }
         if (esauxiliar(ui.CtabaselineEdit_2->text()))
           ui.basetextLabel_2->setText(descripcioncuenta(ui.CtabaselineEdit_2->text()));
          else ui.basetextLabel_2->setText("");
        }
}

void ivasop_aut::ctabasefinedicion3()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
 QString cadena;
  ui.CtabaselineEdit_3->setText(expandepunto(ui.CtabaselineEdit_3->text(),anchocuentas()));
     if (ui.CtabaselineEdit_3->text().length()>3 && cod_longitud_variable()) {
	if (!existecodigoplan(ui.CtabaselineEdit_3->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.CtabaselineEdit_3->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2); 
	           if (!esauxiliar(ui.CtabaselineEdit_3->text()))
			ui.CtabaselineEdit_3->setText("");
		 }
		  else
	            ui.CtabaselineEdit_3->setText("");
            chequeabotonaceptar();
	   }
        if (esauxiliar(ui.CtabaselineEdit_3->text()))
          ui.basetextLabel_3->setText(descripcioncuenta(ui.CtabaselineEdit_3->text()));
         else ui.basetextLabel_3->setText("");
        }
}

void ivasop_aut::ctabase_exenta_finedicion()
{
  ui.Ctabase_exenta_lineEdit->setText(expandepunto(ui.Ctabase_exenta_lineEdit->text(),anchocuentas()));
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
 QString cadena;
  if (ui.Ctabase_exenta_lineEdit->text().length()>3 && cod_longitud_variable()) {
    if (!existecodigoplan(ui.Ctabase_exenta_lineEdit->text(),&cadena))
      {
     // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
        QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
        if (msgBox.clickedButton() == aceptarButton)
             {
               aux_express *lasubcuenta2=new aux_express();
               lasubcuenta2->pasacodigo(ui.Ctabase_exenta_lineEdit->text());
               lasubcuenta2->exec();
               delete(lasubcuenta2);
               if (!esauxiliar(ui.Ctabase_exenta_lineEdit->text()))
                  ui.Ctabase_exenta_lineEdit->setText("");
             }
             else
                ui.Ctabase_exenta_lineEdit->setText("");
        chequeabotonaceptar();
       }
      if (esauxiliar(ui.Ctabase_exenta_lineEdit->text()))
         {
          ui.base_exenta_textLabel->setText(descripcioncuenta(ui.Ctabase_exenta_lineEdit->text()));
          // if (!ui.CtaivalineEdit->text().isEmpty()) ui.baselineEdit->setFocus();
         }
         else ui.base_exenta_textLabel->setText("");
      }
}



void ivasop_aut::ctaivacambiada()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);

   QString cadena;
    if ((ui.CtaivalineEdit->text().length()==anchocuentas() && !cod_longitud_variable())) {
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
	           if (!esauxiliar(ui.CtaivalineEdit->text()))
			ui.CtaivalineEdit->setText("");
		 }
  		else
	            ui.CtaivalineEdit->setText("");
            chequeabotonaceptar();
	  }
        }
}


void ivasop_aut::ctaivacambiada2()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);

   QString cadena;
    if ((ui.CtaivalineEdit2->text().length()==anchocuentas() && !cod_longitud_variable())) {
	if (!existecodigoplan(ui.CtaivalineEdit2->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.CtaivalineEdit2->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2);
	           if (!esauxiliar(ui.CtaivalineEdit2->text()))
			ui.CtaivalineEdit2->setText("");
		 }
  		else
	            ui.CtaivalineEdit2->setText("");
            chequeabotonaceptar();
	  }
        }
}


void ivasop_aut::ctaivacambiada3()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);

   QString cadena;
    if (ui.CtaivalineEdit3->text().length()==anchocuentas() && !cod_longitud_variable()) {
	if (!existecodigoplan(ui.CtaivalineEdit3->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.CtaivalineEdit3->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2);
	           if (!esauxiliar(ui.CtaivalineEdit3->text()))
			ui.CtaivalineEdit3->setText("");
		 }
  		else
	            ui.CtaivalineEdit3->setText("");
            chequeabotonaceptar();
	  }
        }
}


void ivasop_aut::ctaivafinedicion()
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
	           if (!esauxiliar(ui.CtaivalineEdit->text()))
			ui.CtaivalineEdit->setText("");
		 }
  		else
	            ui.CtaivalineEdit->setText("");
            chequeabotonaceptar();
	  }
        }

}

void ivasop_aut::ctaivafinedicion2()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
 QString cadena;
   ui.CtaivalineEdit2->setText(expandepunto(ui.CtaivalineEdit2->text(),anchocuentas()));
      if (ui.CtaivalineEdit2->text().length()>3 && cod_longitud_variable()) {
	if (!existecodigoplan(ui.CtaivalineEdit2->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.CtaivalineEdit2->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2);
	           if (!esauxiliar(ui.CtaivalineEdit2->text()))
			ui.CtaivalineEdit2->setText("");
		 }
  		else
	            ui.CtaivalineEdit2->setText("");
            chequeabotonaceptar();
	  }
        }

}

void ivasop_aut::ctaivafinedicion3()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
 QString cadena;
   ui.CtaivalineEdit3->setText(expandepunto(ui.CtaivalineEdit3->text(),anchocuentas()));
      if (ui.CtaivalineEdit3->text().length()>3 && cod_longitud_variable()) {
	if (!existecodigoplan(ui.CtaivalineEdit3->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.CtaivalineEdit3->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2);
	           if (!esauxiliar(ui.CtaivalineEdit3->text()))
			ui.CtaivalineEdit3->setText("");
		 }
  		else
	            ui.CtaivalineEdit3->setText("");
            chequeabotonaceptar();
	  }
        }

}


void ivasop_aut::ctafracambiada()
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
	           if (!esauxiliar(ui.CtafralineEdit->text()))
			ui.CtafralineEdit->setText("");
		 }
		  else
	            ui.CtafralineEdit->setText("");
            chequeabotonaceptar();
	  }
        }
    // chequeabotonaceptar();
    if (esauxiliar(ui.CtafralineEdit->text()))
       {
	ui.fratextLabel->setText(descripcioncuenta(ui.CtafralineEdit->text()));
        if (basedatos::instancia()->hay_criterio_caja_aux(ui.CtafralineEdit->text()))
            ui.cajacheckBox->setChecked(true);
       }
    else ui.fratextLabel->setText("");
    formaconcepto();
}

void ivasop_aut::concepto_cambiado()
{
    formaconcepto();
}

void ivasop_aut::ctafrafinedicion()
{
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
          else ui.fratextLabel->setText("");
        }
    // chequeabotonaceptar();
}


void ivasop_aut::basecambiada()
{
    bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     QString cadena;
     int ndecimales=2;
     if (!decimales) ndecimales=0;
     double bixtipo=CutAndRoundNumberToNDecimals (bi*tipo, ndecimales);
     cadena.setNum(bixtipo,'f',ndecimales);
     if (comadecimal)
          ui.CuotalineEdit->setText( convacoma(cadena));
        else
          ui.CuotalineEdit->setText( cadena);
     if (comadecimal) ui.baselineEdit->setText(ui.baselineEdit->text().replace('.',','));

     actutotalfra();

  chequeabotonaceptar();
}

void ivasop_aut::basefinedicion()
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


void ivasop_aut::basecambiada2()
{
    bool bien;
    double bi = convapunto(ui.baselineEdit2->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit2->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     QString cadena;
     int ndecimales=2;
     if (!decimales) ndecimales=0;
     double bixtipo=CutAndRoundNumberToNDecimals (bi*tipo, ndecimales);
     cadena.setNum(bixtipo,'f',ndecimales);
     if (comadecimal)
          ui.CuotalineEdit2->setText( convacoma(cadena));
        else
          ui.CuotalineEdit2->setText( cadena);
     if (comadecimal) ui.baselineEdit2->setText(ui.baselineEdit2->text().replace('.',','));

     actutotalfra();

  chequeabotonaceptar();
}

void ivasop_aut::basefinedicion2()
{
    bool bien;
    double bi = convapunto(ui.baselineEdit2->text()).toDouble(& bien);
     if (!bien) bi=0;
     QString cadena;
     int ndecimales=2;
     if (!decimales) ndecimales=0;
     cadena.setNum(bi,'f',ndecimales);
     if (comadecimal)
         ui.baselineEdit2->setText(convacoma(cadena));
        else
         ui.baselineEdit2->setText(cadena);

}


void ivasop_aut::basecambiada3()
{
    bool bien;
    double bi = convapunto(ui.baselineEdit3->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit3->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     QString cadena;
     int ndecimales=2;
     if (!decimales) ndecimales=0;
     double bixtipo=CutAndRoundNumberToNDecimals (bi*tipo, ndecimales);
     cadena.setNum(bixtipo,'f',ndecimales);
     if (comadecimal)
          ui.CuotalineEdit3->setText( convacoma(cadena));
        else
          ui.CuotalineEdit3->setText( cadena);
     if (comadecimal) ui.baselineEdit3->setText(ui.baselineEdit3->text().replace('.',','));

     actutotalfra();

  chequeabotonaceptar();
}

void ivasop_aut::basefinedicion3()
{
    bool bien;
    double bi = convapunto(ui.baselineEdit3->text()).toDouble(& bien);
     if (!bien) bi=0;
     QString cadena;
     int ndecimales=2;
     if (!decimales) ndecimales=0;
     cadena.setNum(bi,'f',ndecimales);
     if (comadecimal)
         ui.baselineEdit3->setText(convacoma(cadena));
        else
         ui.baselineEdit3->setText(cadena);

}

void ivasop_aut::base_exenta_cambiada()
{
     if (comadecimal) ui.baselineEdit->setText(ui.baselineEdit->text().replace('.',','));

     actutotalfra();

  chequeabotonaceptar();
}


void ivasop_aut::base_exenta_finedicion()
{
    bool bien;
    double bi = convapunto(ui.base_exentalineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;
     QString cadena;
     int ndecimales=2;
     if (!decimales) ndecimales=0;
     cadena.setNum(bi,'f',ndecimales);
     if (comadecimal)
         ui.base_exentalineEdit->setText(convacoma(cadena));
        else
         ui.base_exentalineEdit->setText(cadena);
}


void ivasop_aut::totalfrafinedicion()
{
    bool bien;
    double total = convapunto(ui.totalfralineEdit->text()).toDouble(& bien);
    if (!bien) total=0;
    ui.totalfralineEdit->setText(formateanumero(total,comadecimal,decimales));
}

void ivasop_aut::botonaceptar()
{
    bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;
     //double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
     //if (!bien) tipo=0;
     //double cuota = convapunto(ui.CuotalineEdit->text()).toDouble(& bien);
     // if (!bien) cuota=0;

     double bi2 = convapunto(ui.baselineEdit2->text()).toDouble(& bien);
     if (!bien) bi2=0;
     //double tipo2 = convapunto(ui.TipolineEdit2->text()).toDouble(& bien)/100;
     //if (!bien) tipo2=0;
     //double cuota2 = convapunto(ui.CuotalineEdit2->text()).toDouble(& bien);
     // if (!bien) cuota2=0;

    double bi3 = convapunto(ui.baselineEdit3->text()).toDouble(& bien);
     if (!bien) bi3=0;
     // double tipo3 = convapunto(ui.TipolineEdit3->text()).toDouble(& bien)/100;
     // if (!bien) tipo3=0;
     // double cuota3 = convapunto(ui.CuotalineEdit3->text()).toDouble(& bien);
     // if (!bien) cuota3=0;
     
/*
 if (bi<0.0001 && bi>-0.0001 && ui.groupBox->isChecked())
      {
        QMessageBox::warning( this, tr("ERROR"),
         tr("La primera base imponible no puede ser nula"));
         return;
      }

if (ui.groupBox2->isChecked())
   {
     if (bi2<0.0001 && bi2>-0.0001) 
        {
             QMessageBox::warning( this, tr("ERROR"),
               tr("La segunda base imponible no puede ser nula"));
             return;
        }
   }

if (ui.groupBox3->isChecked())
   {

     if (bi3<0.0001 && bi3>-0.0001) 
            {
              QMessageBox::warning( this, tr("ERROR"),
               tr("La tercera base imponible no puede ser nula"));
              return;
            }
   }
*/

    if ((ui.groupBox->isChecked() && !esauxiliar(ui.CtabaselineEdit->text())) ||
        (ui.groupBox2->isChecked() && !esauxiliar(ui.CtabaselineEdit_2->text())) ||
        (ui.groupBox3->isChecked() && !esauxiliar(ui.CtabaselineEdit_3->text())))
	     QMessageBox::warning( this, tr("ERROR"),
                    tr("Alguna de las cuentas base no es correcta"));
     
    if ((ui.groupBox->isChecked() && !esauxiliar(ui.CtaivalineEdit->text())) ||
	(ui.groupBox2->isChecked() && !esauxiliar(ui.CtaivalineEdit2->text())) ||
        (ui.groupBox3->isChecked() && !esauxiliar(ui.CtaivalineEdit3->text())) )
            {
	     QMessageBox::warning( this, tr("ERROR"),
               tr("Alguna cuenta de iva no es correcta"));
	     return;
            }
    
    if (ui.groupBox->isChecked() && !escuentadeivasoportado(ui.CtaivalineEdit->text()))
            {
	     QMessageBox::warning( this, tr("ERROR"),
               tr("La primera cuenta de iva no es correcta"));
	     return;
            }

    if (ui.groupBox2->isChecked() && !escuentadeivasoportado(ui.CtaivalineEdit2->text()))
            {
	     QMessageBox::warning( this, tr("ERROR"),
               tr("La segunda cuenta de iva no es correcta"));
	     return;
            }

    if (ui.groupBox3->isChecked() && !escuentadeivasoportado(ui.CtaivalineEdit3->text()))
            {
	     QMessageBox::warning( this, tr("ERROR"),
               tr("La tercera cuenta de iva no es correcta"));
	     return;
            }

    if (!esauxiliar(ui.CtafralineEdit->text()))
            {
	     QMessageBox::warning( this, tr("ERROR"),
               tr("La cuenta de factura no es correcta"));
	     return;
            }

    QString cad_lista=basedatos::instancia()->selectCuentasapagarconfiguracion().remove(' ');
    if (cad_lista.isEmpty())
      {
        accept();
        return;
       }
    QStringList lista=cad_lista.split(",");
    bool correcta=false;
    for (int veces=0; veces<lista.count(); veces++)
      {
        if (ui.CtafralineEdit->text().startsWith(lista.at(veces)))
          {
           correcta=true;
           break;
          }
      }
   if (!correcta)
    {
      if (QMessageBox::question(
          this,
          tr("Cuenta de factura"),tr("ATENCIÓN: La cuenta de factura '")+
          ui.CtafralineEdit->text()+
          tr("' puede no ser correcta\n")+
          tr("¿ Desea borrar el contenido del campo ?")) ==QMessageBox::No )
                     {
                        accept();
                        return;
                     }
         else
              {
               ui.AceptarButton->setEnabled(false);
               ui.CtafralineEdit->clear();
               return;
              }
     }

     accept();
}




void ivasop_aut::recuperadatos(QString *cuentabase, QString *baseimponible, QString *qclaveiva, QString *qtipoiva,
   QString *cuentaiva, QString *cuotaiva,
 QString *cuentabase2, QString *baseimponible2, QString *qclaveiva2, QString *qtipoiva2,
   QString *cuentaiva2, QString *cuotaiva2,
 QString *cuentabase3, QString *baseimponible3, QString *qclaveiva3, QString *qtipoiva3,
   QString *cuentaiva3, QString *cuotaiva3,
 QString *ctafra, QString *qprorrata, QDate *qfechafra, QDate *qfechacontable,
 QString *qdocumento,
 QDate *qfechaop, QString *qclaveop, bool *rectificativa,
 QString *rectificada, QString *nfacturas, QString *finicial, QString *ffinal,
 bool *verifica, QString *binversion, QString *afectacion, bool *agrario,
 QString *nombre, QString *cif, QString *import, QString *cajaiva,
 QString *cuenta_ret, QString *tipo_ret, QString *retencion, QString *clave_ret, bool *ret_arrendamiento,
 bool *iva_exento, QString *cta_base_exento, QString *base_imponible_exento, QString *externo)
{
    if (ui.groupBox->isChecked()) {
       *cuentabase=ui.CtabaselineEdit->text();
       *baseimponible=ui.baselineEdit->text();
       if (baseimponible->isEmpty()) *baseimponible="0";
       *qclaveiva=ui.ClaveivacomboBox->currentText();
       *qtipoiva=ui.TipolineEdit->text();
       *cuentaiva=ui.CtaivalineEdit->text();
       *cuotaiva=ui.CuotalineEdit->text();
    }
    else {
        *cuentabase=QString();
        *baseimponible=QString();
        *qclaveiva=QString();
        *qtipoiva=QString();
        *cuentaiva=QString();
        *cuotaiva=QString();
    }
if (ui.rectificativacheckBox->isChecked())
  {
   *rectificada=ui.tipo_rectificativa_comboBox->currentText().left(2);
    if (!ui.rectificada_lineEdit->text().isEmpty()) *rectificada+=":"+ui.rectificada_lineEdit->text();
  }
 else *rectificada="";
*nfacturas=ui.nfacturaslineEdit->text();
*finicial=ui.iniciallineEdit->text();
*ffinal=ui.finallineEdit->text();
if (ui.groupBox2->isChecked())
  {
   *cuentabase2=ui.CtabaselineEdit_2->text();
   *baseimponible2=ui.baselineEdit2->text();
    if (baseimponible2->isEmpty()) *baseimponible2="0";
   *qclaveiva2=ui.ClaveivacomboBox2->currentText();
   *qtipoiva2=ui.TipolineEdit2->text();
   *cuentaiva2=ui.CtaivalineEdit2->text();
   *cuotaiva2=ui.CuotalineEdit2->text();
  }
  else
     {
      *cuentabase2="";
      *baseimponible2="";
      *qclaveiva2="";
      *qtipoiva2="";
      *cuentaiva2="";
      *cuotaiva2="";
     }

if (ui.groupBox3->isChecked())
  {
   *cuentabase3=ui.CtabaselineEdit_3->text();
   *baseimponible3=ui.baselineEdit3->text();
   if (baseimponible3->isEmpty()) *baseimponible3="0";
   *qclaveiva3=ui.ClaveivacomboBox3->currentText();
   *qtipoiva3=ui.TipolineEdit3->text();
   *cuentaiva3=ui.CtaivalineEdit3->text();
   *cuotaiva3=ui.CuotalineEdit3->text();
  }
  else
     {
      *cuentabase3="";
      *baseimponible3="";
      *qclaveiva3="";
      *qtipoiva3="";
      *cuentaiva3="";
      *cuotaiva3="";
     }

*ctafra=ui.CtafralineEdit->text();
if (ui.prorratacheckBox->isChecked()) *qprorrata=ui.prorratalineEdit->text();
   else *qprorrata="";
*qfechafra=ui.FechafradateEdit->date();
*qfechacontable=ui.fechacontdateEdit->date();
*qdocumento=ui.documentolineEdit->text();

*qfechaop=ui.fechaoperaciondateEdit->date();

QString cadenaclave=ui.claveoperacioncomboBox->currentText();
QString extracto=cadenaclave.section(' ',0,0);
*qclaveop=extracto.remove(' ');

*rectificativa=ui.rectificativacheckBox->isChecked();

*verifica=ui.verificacheckBox->isChecked();
*binversion=ui.bieninversioncheckBox->isChecked() ? "1" : "0";
*afectacion=ui.afectacionlineEdit->text();

*agrario=ui.agrariocheckBox->isChecked();
*nombre=ui.nombrelineEdit->text();
*cif=ui.ciflineEdit->text();
*import=ui.importcheckBox->isChecked() ? "1" : "";
*cajaiva=ui.cajacheckBox->isChecked() ? "1" : "";

if (convapunto(ui.retencionlineEdit->text()).toDouble()>0.001)
  *cuenta_ret=ui.cuentaretlineEdit->text();
 else *cuenta_ret="";
*tipo_ret=ui.tiporetlineEdit->text();
*retencion=ui.retencionlineEdit->text();
*clave_ret=ui.clavecomboBox->currentText().left(1);
*ret_arrendamiento=ui.arrendamientocheckBox->isChecked();

*iva_exento=ui.exento_checkBox->isChecked();

 *cta_base_exento=ui.Ctabase_exenta_lineEdit->text();
 *base_imponible_exento=ui.base_exentalineEdit->text();
*externo=ui.externo_lineEdit->text();
}


void ivasop_aut::chequeabotonaceptar()
{

     if (!escuentadeivasoportado(ui.CtaivalineEdit->text()) && !ui.exento_checkBox->isChecked())
        { ui.AceptarButton->setEnabled(false); return; }

     if (!escuentadeivasoportado(ui.CtaivalineEdit2->text()) && ui.groupBox2->isChecked())
        { ui.AceptarButton->setEnabled(false); return; }

     if (!escuentadeivasoportado(ui.CtaivalineEdit3->text()) && ui.groupBox3->isChecked())
        { ui.AceptarButton->setEnabled(false); return; }

    if (ui.CtabaselineEdit->text().length()==0 ||
	!esauxiliar(ui.CtabaselineEdit->text()))
        {
         if (!ui.exento_checkBox->isChecked())
            {
             ui.AceptarButton->setEnabled(false);
             return;
            }
         if (ui.exento_checkBox->isChecked() && !ui.CtabaselineEdit->text().isEmpty() &&
                 !esauxiliar(ui.CtabaselineEdit->text()))
            {
             ui.AceptarButton->setEnabled(false);
             return;
            }
        }

    if ((ui.groupBox2->isChecked()) && (ui.CtabaselineEdit_2->text().length()==0 ||
	!esauxiliar(ui.CtabaselineEdit_2->text())) )
        {
         ui.AceptarButton->setEnabled(false);
         return;
        }

    if ((ui.groupBox3->isChecked()) && (ui.CtabaselineEdit_3->text().length()==0 ||
	!esauxiliar(ui.CtabaselineEdit_3->text())) )
        {
         ui.AceptarButton->setEnabled(false);
         return;
        }

    if (ui.exento_checkBox->isChecked() && (ui.Ctabase_exenta_lineEdit->text().isEmpty() ||
                                            !esauxiliar(ui.Ctabase_exenta_lineEdit->text())))
      {
        ui.AceptarButton->setEnabled(false);
        return;
      }

    if (!esauxiliar(ui.CtaivalineEdit->text()) || !escuentadeivasoportado(ui.CtaivalineEdit->text()))
        {
         if (!ui.exento_checkBox->isChecked())
           {
            ui.AceptarButton->setEnabled(false);
            return;
           }
        }

    if ((ui.groupBox2->isChecked()) &&
       (!esauxiliar(ui.CtaivalineEdit2->text()) || !escuentadeivasoportado(ui.CtaivalineEdit2->text())))
        { ui.AceptarButton->setEnabled(false); return; }

    if ((ui.groupBox3->isChecked()) &&
       (!esauxiliar(ui.CtaivalineEdit3->text()) || !escuentadeivasoportado(ui.CtaivalineEdit3->text())))
        { ui.AceptarButton->setEnabled(false); return; }

    if (!esauxiliar(ui.CtafralineEdit->text()))
        { ui.AceptarButton->setEnabled(false); return; }

    ui.AceptarButton->setEnabled(true);

}


// ****************************************************************************************
void ivasop_aut::buscactabase()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtabaselineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.CtabaselineEdit->setText(cadena2);
       else ui.CtabaselineEdit->setText("");
    delete(labusqueda);
}

void ivasop_aut::buscactabase2()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtabaselineEdit_2->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.CtabaselineEdit_2->setText(cadena2);
       else ui.CtabaselineEdit_2->setText("");
    delete(labusqueda);
}

void ivasop_aut::buscactabase3()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtabaselineEdit_3->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.CtabaselineEdit_3->setText(cadena2);
       else ui.CtabaselineEdit_3->setText("");
    delete(labusqueda);
}

void ivasop_aut::buscactabase_exenta()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.Ctabase_exenta_lineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.Ctabase_exenta_lineEdit->setText(cadena2);
       else ui.Ctabase_exenta_lineEdit->setText("");
    delete(labusqueda);
}


void ivasop_aut::buscactaiva()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtaivalineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.CtaivalineEdit->setText(cadena2);
       else ui.CtaivalineEdit->setText("");
    delete(labusqueda);
}

void ivasop_aut::buscactaiva2()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtaivalineEdit2->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.CtaivalineEdit2->setText(cadena2);
       else ui.CtaivalineEdit2->setText("");
    delete(labusqueda);
}

void ivasop_aut::buscactaiva3()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtaivalineEdit3->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.CtaivalineEdit3->setText(cadena2);
       else ui.CtaivalineEdit3->setText("");
    delete(labusqueda);
}


void ivasop_aut::buscactafra()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtafralineEdit->text());
    if (labusqueda->exec()==QDialog::Accepted) {
       cadena2=labusqueda->seleccioncuenta();
       if (esauxiliar(cadena2)) ui.CtafralineEdit->setText(cadena2);
          else ui.CtafralineEdit->setText("");
    } else ui.CtafralineEdit->setText("");
    delete(labusqueda);
}

void ivasop_aut::buscactaret()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.cuentaretlineEdit->text());
    if (labusqueda->exec()==QDialog::Accepted)
      {
       cadena2=labusqueda->seleccioncuenta();
       ui.cuentaretlineEdit->setText(cadena2);
      }
      else
         { ui.cuentaretlineEdit->clear();}
    delete(labusqueda);
    if (!ui.cuentaretlineEdit->text().isEmpty())
      {
        ctaretfinedic();
      }
    //bool basedatos::hayretasociada(QString cuenta, QString *cuentaret, QString *ret, bool *arrend,
    // QString *tipo_oper_ret)
}

void ivasop_aut::ctaretfinedic()
{
    QMessageBox msgBox;
    QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
    msgBox.addButton(tr("No"), QMessageBox::ActionRole);
    msgBox.setWindowTitle(tr("ADVERTENCIA"));
    msgBox.setIcon(QMessageBox::Warning);
    QString cadena;
    if (ui.cuentaretlineEdit->text().isEmpty()) return;
    ui.cuentaretlineEdit->setText(expandepunto(ui.cuentaretlineEdit->text(),anchocuentas()));
    if ((ui.cuentaretlineEdit->text().length()>3 && cod_longitud_variable())
        || (esauxiliar(ui.cuentaretlineEdit->text()) )){
       if (!existecodigoplan(ui.cuentaretlineEdit->text(),&cadena) && !ui.cuentaretlineEdit->text().isEmpty())
         {
        // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
           QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
               msgBox.setText(TextoAviso);
               msgBox.exec();
           if (msgBox.clickedButton() == aceptarButton)
                {
                  aux_express *lasubcuenta2=new aux_express();
                  lasubcuenta2->pasacodigo(ui.cuentaretlineEdit->text());
                  lasubcuenta2->exec();
                  delete(lasubcuenta2);
                  if (!esauxiliar(ui.cuentaretlineEdit->text()))
                      ui.cuentaretlineEdit->setText("");
                }
                else
                   ui.cuentaretlineEdit->setText("");
           chequeabotonaceptar();
          }
       }
    QString qcuentaretencion=ui.cuentaretlineEdit->text();
    if (qcuentaretencion.isEmpty())
       {
        ui.cta_rettextLabel->clear();
        ui.tiporetlineEdit->clear();
        ui.retencionlineEdit->clear();
        ui.arrendamientocheckBox->setChecked(false);
        return;
       }

    ui.cta_rettextLabel->setText(descripcioncuenta(qcuentaretencion));
    double tipo_ret_cta;
    tipo_ret_cta=basedatos::instancia()->tipo_ret_cta(qcuentaretencion);
    QString cadtiporet;
    if (tipo_ret_cta>0.001)
      {
       cadtiporet.setNum(tipo_ret_cta,'f',2);
       ui.tiporetlineEdit->setText(comadecimal ? convacoma(cadtiporet) : cadtiporet);
      }
      else
           {
            ui.tiporetlineEdit->clear();
            ui.retencionlineEdit->clear();
           }
    actutotalfra();
    QString tipo_operacion_ret=basedatos::instancia()->tipo_operacion_ret(qcuentaretencion);
    int indice=0;
    while (indice<ui.clavecomboBox->count())
      {
       if (ui.clavecomboBox->itemText(indice).left(1)==tipo_operacion_ret.left(1)) break;
       indice++;
      }
    if (indice>=ui.clavecomboBox->count()) indice=0;
    ui.clavecomboBox->setCurrentIndex(indice);

    if (basedatos::instancia()->ret_arrendamiento(qcuentaretencion))
        ui.arrendamientocheckBox->setChecked(true);
     else
         ui.arrendamientocheckBox->setChecked(false);

}

void ivasop_aut::tiporetfinedic()
{
  if (comadecimal) ui.tiporetlineEdit->setText(convacoma(ui.tiporetlineEdit->text()));
     else ui.tiporetlineEdit->setText(convapunto(ui.tiporetlineEdit->text()));
  actutotalfra();
}

void ivasop_aut::grboxcambiado1()
{
  if (!ui.groupBox2->isChecked())
     {
      ui.CtabaselineEdit_2->clear();
      ui.CtaivalineEdit2->clear();
      ui.baselineEdit2->clear();
      // ui.TipolineEdit2->clear();
      ui.CuotalineEdit2->clear();
     }
  chequeabotonaceptar();
}

void ivasop_aut::grboxcambiado2()
{
  if (!ui.groupBox3->isChecked())
     {
      ui.CtabaselineEdit_3->clear();
      ui.CtaivalineEdit3->clear();
      ui.baselineEdit3->clear();
      // ui.TipolineEdit3->clear();
      ui.CuotalineEdit3->clear();
     }
  chequeabotonaceptar();
}

void ivasop_aut::fechafracambiada()
{
 ui.fechacontdateEdit->setDate(ui.FechafradateEdit->date());
 ui.fechaoperaciondateEdit->setDate(ui.FechafradateEdit->date());
 formaconcepto();
}

void ivasop_aut::cargacombooperaciones()
{
  QStringList lista=basedatos::instancia()->listaoperaciones_recibidas();
  ui.claveoperacioncomboBox->addItems (lista);
}

QString ivasop_aut::fichdocumento()
{
  return ui.fichdoclineEdit->text();
}

void ivasop_aut::pasa_fecha(QDate fecha)
{
    ui.FechafradateEdit->setDate(fecha);
    ui.fechacontdateEdit->setDate(fecha);
    ui.fechaoperaciondateEdit->setDate(fecha);
}

void ivasop_aut::pasa_datos_ini(QString externo, QString cuenta_proveedor, QString cuenta_gasto, QString cuenta_iva_soportado, QString cuenta_ret_irpf,
                                QDate fecha, QJsonObject importes_info, QString factura, QString fichero)
{
    ui.externo_lineEdit->setText(externo);
    ui.CtafralineEdit->setText(cuenta_proveedor);
    if (cuenta_iva_soportado.isEmpty()) { ui.groupBox->setChecked(false); ui.Ctabase_exenta_lineEdit->setText(cuenta_gasto); ui.exento_checkBox->setChecked(true);}
      else {ui.CtabaselineEdit->setText(cuenta_gasto); ui.CtaivalineEdit->setText(cuenta_iva_soportado); }
    ui.cuentaretlineEdit->setText(cuenta_ret_irpf);
    if (!cuenta_ret_irpf.isEmpty()) ctaretfinedic();
    ui.FechafradateEdit->setDate(fecha);
    ui.documentolineEdit->setText(factura);
    ui.fechacontdateEdit->setDate(fecha);
    ui.fechaoperaciondateEdit->setDate(fecha);
    ui.fichdoclineEdit->setText(fichero);
    if (importes_info["base21"].toDouble()>0.001 || importes_info["base21"].toDouble()<-0.001) {
        // buscamos en el combo de tipos de IVA el correspondiente
        double base21=importes_info["base21"].toDouble();
        QString clave_iva=basedatos::instancia()->cod_iva(21);
        ui.ClaveivacomboBox->setCurrentText(clave_iva);
        ui.baselineEdit->setText(formateanumero(base21,comadecimal,decimales));
        comboivacambiado();
    }

    if (importes_info["base10"].toDouble()>0.001 || importes_info["base10"].toDouble()<-0.001) {
        // buscamos en el combo de tipos de IVA el correspondiente
        double base10=importes_info["base10"].toDouble();
        QString clave_iva=basedatos::instancia()->cod_iva(10);
        if (ui.baselineEdit->text().isEmpty()) {
           ui.ClaveivacomboBox->setCurrentText(clave_iva);
           ui.baselineEdit->setText(formateanumero(base10,comadecimal,decimales));
           comboivacambiado();
        }
        else {
            // Activamos groupbox segundo
            ui.groupBox2->setChecked(true);
            ui.ClaveivacomboBox2->setCurrentText(clave_iva);
            ui.baselineEdit2->setText(formateanumero(base10,comadecimal,decimales));
            comboivacambiado2();
        }
    }

    if (importes_info["base4"].toDouble()>0.001 || importes_info["base4"].toDouble()<-0.001) {
        // buscamos en el combo de tipos de IVA el correspondiente
        double base4=importes_info["base4"].toDouble();
        QString clave_iva=basedatos::instancia()->cod_iva(4);
        if (ui.baselineEdit->text().isEmpty()) {
            ui.ClaveivacomboBox->setCurrentText(clave_iva);
            ui.baselineEdit->setText(formateanumero(base4,comadecimal,decimales));
            comboivacambiado();
        }
        else {
            if (ui.baselineEdit2->text().isEmpty()) {
            // Activamos groupbox segundo
            ui.groupBox2->setChecked(true);
            ui.ClaveivacomboBox2->setCurrentText(clave_iva);
            ui.baselineEdit2->setText(formateanumero(base4,comadecimal,decimales));
            comboivacambiado2();
            } else {
                ui.groupBox3->setChecked(true);
                ui.ClaveivacomboBox3->setCurrentText(clave_iva);
                ui.baselineEdit3->setText(formateanumero(base4,comadecimal,decimales));
                comboivacambiado3();
            }

        }
    }
    if (ui.exento_checkBox->isChecked()) {
        double baseret=importes_info["baseret"].toDouble();
        ui.base_exentalineEdit->setText(formateanumero(baseret,comadecimal, decimales));
    }
}


void ivasop_aut::buscacopiadoc()
{
#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(rutacargadocs().isEmpty() ? adapta(dirtrabajo()):adapta(rutacargadocs()));
  if (dir->exec() == QDialog::Accepted)
    {
      // QMessageBox::information( this, tr("selección"),
      //                         dir->selec_actual() );
      // QMessageBox::information( this, tr("selección"),
      //                         dir->ruta_actual() );
      ui.fichdoclineEdit->setText(dir->ruta_actual());
    }
  delete(dir);
#else

  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::ExistingFile);
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

  QStringList filtros;
  filtros << tr("Todos los archivos (*)");
  filtros << tr("Archivos jpg (*.jpg)");
  filtros << tr("Archivos jpg (*.jpeg)");
  filtros << tr("Archivos png (*.png)");
  filtros << tr("Archivos pdf (*.pdf)");
  dialogofich.setNameFilters(filtros);
  if (rutacargadocs().isEmpty())
     dialogofich.setDirectory(adapta(dirtrabajo()));
    else
      dialogofich.setDirectory(adapta(rutacargadocs()));
  dialogofich.setWindowTitle(tr("SELECCIÓN DE COPIA DE DOCUMENTO"));
  // dialogofich.exec();
  //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
  //                                              dirtrabajo,
  //                                              tr("Ficheros de texto (*.txt)"));
  QStringList fileNames;
  if (dialogofich.exec())
     {
      fileNames = dialogofich.selectedFiles();
      if (fileNames.at(0).length()>0)
          {
           // QString cadfich=cadfich.fromLocal8Bit(fileNames.at(0));
          ui.fichdoclineEdit->setText(fileNames.at(0));
          }
     }
#endif
}

void ivasop_aut::visdoc()
{
   if (ui.fichdoclineEdit->text().isEmpty()) return;
   if (!ejecuta(aplicacionabrirfich(extensionfich(ui.fichdoclineEdit->text())),ui.fichdoclineEdit->text()))
         QMessageBox::warning( this, tr("TABLA DE ASIENTOS"),
                             tr("No se puede abrir ")+ui.fichdoclineEdit->text()+tr(" con ")+
                             aplicacionabrirfich(extensionfich(ui.fichdoclineEdit->text())));

}

void ivasop_aut::borraasignaciondoc()
{
    ui.fichdoclineEdit->clear();
}


void ivasop_aut::afectacionfinedicion()
{
    bool bien;
    QString cadena=ui.afectacionlineEdit->text();
    double porcentaje = convapunto(ui.afectacionlineEdit->text()).toDouble(& bien);
     if (!bien || porcentaje>100 || porcentaje<0)
       {
         ui.afectacionlineEdit->setText("100");
       }
       else
           {
             if (comadecimal)
                 ui.afectacionlineEdit->setText(convacoma(cadena));
             else
                 ui.afectacionlineEdit->setText(convapunto(cadena));
           }

}


void ivasop_aut::formaconcepto()
{
   QString concepto=ui.fratextLabel->text();
   if (!ui.externo_lineEdit->text().isEmpty())
       concepto=ui.descrip_externo_label->text();
   concepto+=" - ";
   concepto+=ui.documentolineEdit->text();
   concepto+=" - ";
   concepto+=ui.FechafradateEdit->date().toString("dd-MM-yyyy");
   ui.conceptolineEdit->setText(concepto);
}


QString ivasop_aut::concepto()
{
  return ui.conceptolineEdit->text();
}

void ivasop_aut::arrendamiento_cambiado()
{
  if (!ui.arrendamientocheckBox->isChecked()) ui.clavecomboBox->setEnabled(true);
     else ui.clavecomboBox->setEnabled(false);
}

void ivasop_aut::iva_exento_cambiado()
{
    if (ui.exento_checkBox->isChecked()) ui.exento_groupBox->show();
    else {
        ui.exento_groupBox->hide();
        ui.base_exentalineEdit->clear();
        ui.Ctabase_exenta_lineEdit->clear();
    }
}


void ivasop_aut::buscar_externo()
{
    busca_externo *b = new busca_externo();
    if (b->exec()==QDialog::Accepted)
      {
        ui.externo_lineEdit->setText(b->codigo_elec());
      }
      else ui.externo_lineEdit->clear();
    delete(b);
}


void ivasop_aut::externo_cambiado()
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


void ivasop_aut::fichero_soltado() {
    ui.fichdoclineEdit->setText(etiqueta->nombre_fich());
}
