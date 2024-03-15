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

#include "ivarep_aut.h"
#include "funciones.h"
#include "basedatos.h"
#include "aux_express.h"
#include "directorio.h"
#include "busca_externo.h"
#include <QMessageBox>
#include <QFileDialog>

ivarep_aut::ivarep_aut(bool concomadecimal, bool condecimales) : QDialog() {
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
        setWindowTitle(tr("IGIC repercutido"));
        ui.TextLabel3->setText(tr("Clave IGIC"));
        ui.textLabel4->setText(tr("Cuenta IGIC"));
        ui.textLabel7->setText(tr("Cuota IGIC"));

        ui.TextLabel3_2->setText(tr("Clave IGIC"));
        ui.textLabel4_2->setText(tr("Cuenta IGIC"));
        ui.textLabel7_4->setText(tr("Cuota IGIC"));

        ui.TextLabel3_6->setText(tr("Clave IGIC"));
        ui.textLabel4_6->setText(tr("Cuenta IGIC"));
        ui.textLabel7_11->setText(tr("Cuota IGIC"));

        ui.groupBox->setTitle("IGIC 1");
        ui.groupBox_2->setTitle("IGIC 2");
        ui.groupBox_3->setTitle("IGIC 3");
       }
    comadecimal=concomadecimal;
    decimales=condecimales;

    ui.tipo_rectificativa_comboBox->addItems(tipos_rectificativas);

    baseoblig=false;
  // Tenemos que cargar clave IVA por defecto y resto de IVA'S en el combo box
    QString pasalinea;
    QString Clavedefecto, laclave;
    double tipo;
    double re;
    QString qdescrip;

    Clavedefecto=clave_iva_defecto();
    if (Clavedefecto.length()>0 && existeclavetipoiva(Clavedefecto,&tipo,&re,&qdescrip))
        {
	pasalinea=Clavedefecto;
	ui.ClaveivacomboBox->insertItem ( 0,pasalinea) ;
	ui.ClaveivacomboBox_2->insertItem ( 0,pasalinea) ;
	ui.ClaveivacomboBox_3->insertItem ( 0,pasalinea) ;
        }
    // ahora cargamos el resto de los tipos de IVA
    
 QSqlQuery query = basedatos::instancia()->selectClavetiposivanoclave(Clavedefecto);
 
 if ( query.isActive() )
    {
      while (query.next())
         {
	  laclave=query.value(0).toString();
	  ui.ClaveivacomboBox->insertItem ( -1,laclave) ;
	  ui.ClaveivacomboBox_2->insertItem ( -1,laclave) ;
	  ui.ClaveivacomboBox_3->insertItem ( -1,laclave) ;
         }
      }

 comboivacambiado();
 comboivacambiado2();
 comboivacambiado3();

 cargacombooperaciones();

 if (basedatos::instancia()->hay_criterio_caja_config())
     ui.cajacheckBox->setChecked(true);

 ui.FechafradateEdit->setDate(QDate::currentDate());
  ui.fechacontdateEdit->setDate(QDate::currentDate());
 ui.fechaoperaciondateEdit->setDate(QDate::currentDate());

 connect(ui.ClaveivacomboBox,SIGNAL(activated(int)),this,SLOT(comboivacambiado()));
 connect(ui.ClaveivacomboBox_2,SIGNAL(activated(int)),this,SLOT(comboivacambiado2()));
 connect(ui.ClaveivacomboBox_3,SIGNAL(activated(int)),this,SLOT(comboivacambiado3()));

 connect(ui.CtabaselineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctabasecambiada()));
 connect(ui.CtabaselineEdit,SIGNAL(editingFinished()),this,SLOT(ctabasefinedicion()));

 connect(ui.CtabaselineEdit_2,SIGNAL(textChanged(QString)),this,SLOT(ctabasecambiada2()));
 connect(ui.CtabaselineEdit_2,SIGNAL(editingFinished()),this,SLOT(ctabasefinedicion2()));

 connect(ui.CtabaselineEdit_3,SIGNAL(textChanged(QString)),this,SLOT(ctabasecambiada3()));
 connect(ui.CtabaselineEdit_3,SIGNAL(editingFinished()),this,SLOT(ctabasefinedicion3()));

 connect(ui.CtaivalineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctaivacambiada()));
 connect(ui.CtaivalineEdit,SIGNAL(editingFinished()),this,SLOT(ctaivafinedicion()));

 connect(ui.CtaivalineEdit_2,SIGNAL(textChanged(QString)),this,SLOT(ctaivacambiada2()));
 connect(ui.CtaivalineEdit_2,SIGNAL(editingFinished()),this,SLOT(ctaivafinedicion2()));

 connect(ui.CtaivalineEdit_3,SIGNAL(textChanged(QString)),this,SLOT(ctaivacambiada3()));
 connect(ui.CtaivalineEdit_3,SIGNAL(editingFinished()),this,SLOT(ctaivafinedicion3()));

 connect(ui.CtafralineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctafracambiada()));
 connect(ui.CtafralineEdit,SIGNAL(editingFinished()),this,SLOT(ctafrafinedicion()));

 connect(ui.baselineEdit,SIGNAL(textChanged(QString)),this,SLOT(basecambiada()));
 connect(ui.baselineEdit_2,SIGNAL(textChanged(QString)),this,SLOT(basecambiada2()));
 connect(ui.baselineEdit_3,SIGNAL(textChanged(QString)),this,SLOT(basecambiada3()));

 connect(ui.baselineEdit,SIGNAL(editingFinished()),this,SLOT(basefinedicion()));
 connect(ui.baselineEdit_2,SIGNAL(editingFinished()),this,SLOT(basefinedicion2()));
 connect(ui.baselineEdit_3,SIGNAL(editingFinished()),this,SLOT(basefinedicion3()));

 connect(ui.AceptarButton,SIGNAL(clicked()),this,SLOT(botonaceptar()));

 connect(ui.buscabasepushButton,SIGNAL(clicked()),this,SLOT(buscactabase()));
 connect(ui.buscaivapushButton,SIGNAL(clicked()),this,SLOT(buscactaiva()));

 connect(ui.buscabasepushButton_2,SIGNAL(clicked()),this,SLOT(buscactabase2()));
 connect(ui.buscaivapushButton_2,SIGNAL(clicked()),this,SLOT(buscactaiva2()));

 connect(ui.buscabasepushButton_3,SIGNAL(clicked()),this,SLOT(buscactabase3()));
 connect(ui.buscaivapushButton_3,SIGNAL(clicked()),this,SLOT(buscactaiva3()));

 connect(ui.buscafrapushButton,SIGNAL(clicked()),this,SLOT(buscactafra()));

 connect(ui.CtabaselineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));
 connect(ui.CtaivalineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));

 connect(ui.CtabaselineEdit_2,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));
 connect(ui.CtaivalineEdit_2,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));

 connect(ui.CtabaselineEdit_3,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));
 connect(ui.CtaivalineEdit_3,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));

 connect(ui.CtafralineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));

 connect(ui.groupBox_2,SIGNAL(toggled(bool)),this,SLOT(grboxcambiado2()));
 connect(ui.groupBox_3,SIGNAL(toggled(bool)),this,SLOT(grboxcambiado3()));

 connect(ui.FechafradateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(fechafracambiada()));

connect(ui.fichdocpushButton ,SIGNAL(clicked()),SLOT(buscacopiadoc()));
connect(ui.visdocpushButton ,SIGNAL(clicked()),SLOT(visdoc()));
connect(ui.borraasdocpushButton ,SIGNAL(clicked()),SLOT(borraasignaciondoc()));
connect(ui.documentolineEdit, SIGNAL(textChanged(QString)),SLOT(documento_cambiado()));

connect(ui.externo_pushButton,SIGNAL(clicked(bool)),this,SLOT(buscar_externo()));

connect(ui.externo_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(externo_cambiado()));

connect(etiqueta, SIGNAL(filedroped()),SLOT(fichero_soltado()));

if (!basedatos::instancia()->hay_externos())
{
 ui.externo_label->hide();
 ui.externo_lineEdit->hide();
 ui.externo_pushButton->hide();
 ui.descrip_externo_label->hide();
}

} 


void ivarep_aut::actutotalfra()
{
     QString cadena;
     double total=convapunto(ui.baselineEdit->text()).toDouble()+
                  convapunto(ui.CuotalineEdit->text()).toDouble()+
                  convapunto(ui.CuotaRElineEdit->text()).toDouble()+
                  convapunto(ui.baselineEdit_2->text()).toDouble()+
                  convapunto(ui.CuotalineEdit_2->text()).toDouble()+
                  convapunto(ui.CuotaRElineEdit_2->text()).toDouble()+
                  convapunto(ui.baselineEdit_3->text()).toDouble()+
                  convapunto(ui.CuotalineEdit_3->text()).toDouble()+
                  convapunto(ui.CuotaRElineEdit_3->text()).toDouble();
     if (decimales)
         cadena.setNum(total,'f',2);
       else cadena.setNum(total,'f',0);
     if (comadecimal) ui.totalfralineEdit->setText( convacoma(cadena));
         else ui.totalfralineEdit->setText(cadena);
}


void ivarep_aut::comboivacambiado()
{
QString codiva=ui.ClaveivacomboBox->currentText();

QString tipoc,rec;
cadvalorestipoiva(codiva,&tipoc,&rec);
if (comadecimal) tipoc=convacoma(tipoc);
ui.TipolineEdit->setText(tipoc);
if (comadecimal) rec=convacoma(rec);
ui.RElineEdit->setText(rec);

ui.CtaivalineEdit->setText(basedatos::instancia()->cuenta_tipo_iva(codiva));

    bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     double tipore = convapunto(ui.RElineEdit->text()).toDouble(& bien)/100;
     if (!bien) tipore=0;
     int ndecimales= decimales ? 2 : 0;
     double cuota=CutAndRoundNumberToNDecimals (bi*tipo, ndecimales);
     QString cadena;
     if (decimales)
         cadena.setNum(cuota,'f',2);
       else cadena.setNum(cuota,'f',0);
     if (comadecimal) ui.CuotalineEdit->setText( convacoma(cadena));
         else ui.CuotalineEdit->setText(cadena);

     double bixtipore=CutAndRoundNumberToNDecimals (bi*tipore, ndecimales);
     if (decimales)
         cadena.setNum(bixtipore,'f',2);
       else cadena.setNum(bixtipore,'f',0);
     if (comadecimal) ui.CuotaRElineEdit->setText( convacoma(cadena));
         else ui.CuotaRElineEdit->setText(cadena);

   actutotalfra();

   chequeabotonaceptar();
}


void ivarep_aut::comboivacambiado2()
{
QString codiva=ui.ClaveivacomboBox_2->currentText();

QString tipoc,rec;
cadvalorestipoiva(codiva,&tipoc,&rec);
if (comadecimal) tipoc=convacoma(tipoc);
ui.TipolineEdit_2->setText(tipoc);
if (comadecimal) rec=convacoma(rec);
ui.RElineEdit_2->setText(rec);

ui.CtaivalineEdit_2->setText(basedatos::instancia()->cuenta_tipo_iva(codiva));

    bool bien;
    double bi = convapunto(ui.baselineEdit_2->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit_2->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     double tipore = convapunto(ui.RElineEdit_2->text()).toDouble(& bien)/100;
     if (!bien) tipore=0;
     int ndecimales= decimales ? 2 : 0;
     double cuota=CutAndRoundNumberToNDecimals (bi*tipo, ndecimales);
     QString cadena;
     if (decimales)
         cadena.setNum(cuota,'f',2);
       else cadena.setNum(cuota,'f',0);
     if (comadecimal) ui.CuotalineEdit_2->setText( convacoma(cadena));
         else ui.CuotalineEdit_2->setText(cadena);

     double bixtipore=CutAndRoundNumberToNDecimals (bi*tipore, ndecimales);
     if (decimales)
         cadena.setNum(bixtipore,'f',2);
       else cadena.setNum(bixtipore,'f',0);
     if (comadecimal) ui.CuotaRElineEdit_2->setText( convacoma(cadena));
         else ui.CuotaRElineEdit_2->setText(cadena);

   actutotalfra();

   chequeabotonaceptar();
}


void ivarep_aut::comboivacambiado3()
{
QString codiva=ui.ClaveivacomboBox_3->currentText();

QString tipoc,rec;
cadvalorestipoiva(codiva,&tipoc,&rec);
if (comadecimal) tipoc=convacoma(tipoc);
ui.TipolineEdit_3->setText(tipoc);
if (comadecimal) rec=convacoma(rec);
ui.RElineEdit_3->setText(rec);

ui.CtaivalineEdit_3->setText(basedatos::instancia()->cuenta_tipo_iva(codiva));

    bool bien;
    double bi = convapunto(ui.baselineEdit_3->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit_3->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     double tipore = convapunto(ui.RElineEdit_3->text()).toDouble(& bien)/100;
     if (!bien) tipore=0;
     int ndecimales= decimales ? 2 : 0;
     double cuota=CutAndRoundNumberToNDecimals (bi*tipo, ndecimales);
     QString cadena;
     if (decimales)
         cadena.setNum(cuota,'f',2);
       else cadena.setNum(cuota,'f',0);
     if (comadecimal) ui.CuotalineEdit_3->setText( convacoma(cadena));
         else ui.CuotalineEdit_3->setText(cadena);

     double bixtipore=CutAndRoundNumberToNDecimals (bi*tipore, ndecimales);
     if (decimales)
         cadena.setNum(bixtipore,'f',2);
       else cadena.setNum(bixtipore,'f',0);
     if (comadecimal) ui.CuotaRElineEdit_3->setText( convacoma(cadena));
         else ui.CuotaRElineEdit_3->setText(cadena);

   actutotalfra();

   chequeabotonaceptar();
}



void ivarep_aut::ctabasecambiada()
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

void ivarep_aut::ctabasefinedicion()
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



void ivarep_aut::ctabasecambiada2()
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
	           if (!existecodigoplan(ui.CtabaselineEdit_2->text(),&cadena))
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


void ivarep_aut::ctabasefinedicion2()
{
  ui.CtabaselineEdit_2->setText(expandepunto(ui.CtabaselineEdit_2->text(),anchocuentas()));
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
 QString cadena;
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


void ivarep_aut::ctabasecambiada3()
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
	           if (!existecodigoplan(ui.CtabaselineEdit_3->text(),&cadena))
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


void ivarep_aut::ctabasefinedicion3()
{
  ui.CtabaselineEdit_3->setText(expandepunto(ui.CtabaselineEdit_3->text(),anchocuentas()));
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
 QString cadena;
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
			ui.CtabaselineEdit_2->setText("");
		 }
		  else
	            ui.CtabaselineEdit_3->setText("");
            chequeabotonaceptar();
	   }
        if (esauxiliar(ui.CtabaselineEdit_3->text()))
          ui.basetextLabel->setText(descripcioncuenta(ui.CtabaselineEdit_3->text()));
         else ui.basetextLabel_3->setText("");
        }
}



void ivarep_aut::ctaivacambiada()
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
	  }
         chequeabotonaceptar();
        }
}

void ivarep_aut::ctaivafinedicion()
{
   ui.CtaivalineEdit->setText(expandepunto(ui.CtaivalineEdit->text(),anchocuentas()));

 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
 QString cadena;

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


void ivarep_aut::ctaivacambiada2()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);

   QString cadena;
    if (ui.CtaivalineEdit_2->text().length()==anchocuentas() && !cod_longitud_variable()) {
	if (!existecodigoplan(ui.CtaivalineEdit_2->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.CtaivalineEdit_2->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2); 
	           if (!existecodigoplan(ui.CtaivalineEdit_2->text(),&cadena))
			ui.CtaivalineEdit_2->clear();
		 }
  		else
	            ui.CtaivalineEdit_2->clear();
	  }
         chequeabotonaceptar();
        }
}

void ivarep_aut::ctaivafinedicion2()
{
   ui.CtaivalineEdit_2->setText(expandepunto(ui.CtaivalineEdit_2->text(),anchocuentas()));

 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
 QString cadena;

       if (ui.CtaivalineEdit_2->text().length()>3 && cod_longitud_variable()) {
	if (!existecodigoplan(ui.CtaivalineEdit_2->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.CtaivalineEdit_2->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2);
	           if (!esauxiliar(ui.CtaivalineEdit_2->text()))
			ui.CtaivalineEdit_2->clear();
		 }
  		else
	            ui.CtaivalineEdit_2->clear();
            chequeabotonaceptar();
	  }
        }

}


void ivarep_aut::ctaivacambiada3()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);

   QString cadena;
    if (ui.CtaivalineEdit_3->text().length()==anchocuentas() && !cod_longitud_variable()) {
	if (!existecodigoplan(ui.CtaivalineEdit_3->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.CtaivalineEdit_3->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2);
	           if (!existecodigoplan(ui.CtaivalineEdit_3->text(),&cadena))
			ui.CtaivalineEdit_3->clear();
		 }
  		else
	            ui.CtaivalineEdit_3->clear();
	  }
         chequeabotonaceptar();
        }
}

void ivarep_aut::ctaivafinedicion3()
{
   ui.CtaivalineEdit_3->setText(expandepunto(ui.CtaivalineEdit_3->text(),anchocuentas()));

 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
 QString cadena;

       if (ui.CtaivalineEdit_3->text().length()>3 && cod_longitud_variable()) {
	if (!existecodigoplan(ui.CtaivalineEdit_3->text(),&cadena)) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
	    if (msgBox.clickedButton() == aceptarButton) 
	         {
                   aux_express *lasubcuenta2=new aux_express();
		   lasubcuenta2->pasacodigo(ui.CtaivalineEdit_3->text());
		   lasubcuenta2->exec();
	           delete(lasubcuenta2);
	           if (!esauxiliar(ui.CtaivalineEdit_3->text()))
			ui.CtaivalineEdit_3->clear();
		 }
  		else
	            ui.CtaivalineEdit_3->clear();
            chequeabotonaceptar();
	  }
        }

}



void ivarep_aut::ctafracambiada()
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
	            ui.CtafralineEdit->clear();
            chequeabotonaceptar();
	   }
        }
    if (esauxiliar(ui.CtafralineEdit->text())) 
	ui.fratextLabel->setText(descripcioncuenta(ui.CtafralineEdit->text()));
    else ui.fratextLabel->clear();
    formaconcepto();
}

void ivarep_aut::ctafrafinedicion()
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


void ivarep_aut::basecambiada()
{
    bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     QString cadena;
     int ndecimales=2;
     if (!decimales) ndecimales=0;
     double cuota=CutAndRoundNumberToNDecimals (bi*tipo, ndecimales);
     cadena.setNum(cuota,'f',ndecimales);
     if (comadecimal)
          ui.CuotalineEdit->setText( convacoma(cadena));
        else
          ui.CuotalineEdit->setText( cadena);
     if (comadecimal) ui.baselineEdit->setText(ui.baselineEdit->text().replace('.',','));

     double re = convapunto(ui.RElineEdit->text()).toDouble( & bien)/100;
     if (!bien) re=0;
     double bixre=CutAndRoundNumberToNDecimals (bi*re, ndecimales);
     ui.CuotaRElineEdit->setText(formateanumero(bixre,comadecimal,decimales));
  actutotalfra();
  chequeabotonaceptar();
}


void ivarep_aut::basecambiada2()
{
    bool bien;
    double bi = convapunto(ui.baselineEdit_2->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit_2->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     QString cadena;
     int ndecimales=2;
     if (!decimales) ndecimales=0;
     double cuota=CutAndRoundNumberToNDecimals (bi*tipo, ndecimales);
     cadena.setNum(cuota,'f',ndecimales);
     if (comadecimal)
          ui.CuotalineEdit_2->setText( convacoma(cadena));
        else
          ui.CuotalineEdit_2->setText( cadena);
     if (comadecimal) ui.baselineEdit_2->setText(ui.baselineEdit_2->text().replace('.',','));

     double re = convapunto(ui.RElineEdit_2->text()).toDouble( & bien)/100;
     if (!bien) re=0;
     double bixre=CutAndRoundNumberToNDecimals (bi*re, ndecimales);
     ui.CuotaRElineEdit_2->setText(formateanumero(bixre,comadecimal,decimales));
  actutotalfra();
  chequeabotonaceptar();
}

void ivarep_aut::basecambiada3()
{
    bool bien;
    double bi = convapunto(ui.baselineEdit_3->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit_3->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     QString cadena;
     int ndecimales=2;
     if (!decimales) ndecimales=0;
     double cuota=CutAndRoundNumberToNDecimals (bi*tipo, ndecimales);
     cadena.setNum(cuota,'f',ndecimales);
     if (comadecimal)
          ui.CuotalineEdit_3->setText( convacoma(cadena));
        else
          ui.CuotalineEdit_3->setText( cadena);
     if (comadecimal) ui.baselineEdit_3->setText(ui.baselineEdit_3->text().replace('.',','));

     double re = convapunto(ui.RElineEdit_3->text()).toDouble( & bien)/100;
     if (!bien) re=0;
     double bixre=CutAndRoundNumberToNDecimals (bi*re, ndecimales);
     ui.CuotaRElineEdit_3->setText(formateanumero(bixre,comadecimal,decimales));
  actutotalfra();
  chequeabotonaceptar();
}

void ivarep_aut::basefinedicion()
{
 double valor=convapunto(ui.baselineEdit->text()).toDouble();
 ui.baselineEdit->setText(formateanumero(valor,comadecimal,decimales));
}

void ivarep_aut::basefinedicion2()
{
 double valor=convapunto(ui.baselineEdit_2->text()).toDouble();
 ui.baselineEdit_2->setText(formateanumero(valor,comadecimal,decimales));
}

void ivarep_aut::basefinedicion3()
{
 double valor=convapunto(ui.baselineEdit_3->text()).toDouble();
 ui.baselineEdit_3->setText(formateanumero(valor,comadecimal,decimales));
}

void ivarep_aut::botonaceptar()
{
    bool bien;
    double bi=0;
    bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;
	 
     if (bi<0.0001 && bi>-0.0001) 
            {
              //QMessageBox::warning( this, tr("ERROR"),
              // tr("La base imponible no puede ser nula"));
              //return;
            }

   if (ui.groupBox_2->isChecked())
    {
     bi = convapunto(ui.baselineEdit_2->text()).toDouble(& bien);
     if (!bien) bi=0;
	 
     if (bi<0.0001 && bi>-0.0001) 
            {
	     QMessageBox::warning( this, tr("ERROR"),
               tr("La segunda base imponible no puede ser nula"));
	     return;
            }
    }

   if (ui.groupBox_3->isChecked())
    {
     bi = convapunto(ui.baselineEdit_3->text()).toDouble(& bien);
     if (!bien) bi=0;
	 
     if (bi<0.0001 && bi>-0.0001) 
            {
	     QMessageBox::warning( this, tr("ERROR"),
               tr("La tercera base imponible no puede ser nula"));
	     return;
            }
    }

    if (!esauxiliar(ui.CtabaselineEdit->text()))
            {
	     QMessageBox::warning( this, tr("ERROR"),
                    tr("La cuenta base no es correcta"));
            }

    if (ui.groupBox_2->isChecked() && !esauxiliar(ui.CtabaselineEdit_2->text()))
            {
	     QMessageBox::warning( this, tr("ERROR"),
                    tr("La segunda cuenta base no es correcta"));
            }

    if (ui.groupBox_3->isChecked() && !esauxiliar(ui.CtabaselineEdit_3->text()))
            {
	     QMessageBox::warning( this, tr("ERROR"),
                    tr("La tercera cuenta base no es correcta"));
            }

    if (!esauxiliar(ui.CtaivalineEdit->text())) 
            {
 	     QMessageBox::warning( this, tr("ERROR"),
               tr("La cuenta de iva no es correcta"));
	     return;
            }

    if (ui.groupBox_2->isChecked() && !esauxiliar(ui.CtaivalineEdit_2->text())) 
            {
 	     QMessageBox::warning( this, tr("ERROR"),
               tr("La segunda cuenta de iva no es correcta"));
	     return;
            }

    if (ui.groupBox_3->isChecked() && !esauxiliar(ui.CtaivalineEdit_3->text())) 
            {
 	     QMessageBox::warning( this, tr("ERROR"),
               tr("La tercera cuenta de iva no es correcta"));
	     return;
            }
    
    if (!escuentadeivarepercutido(ui.CtaivalineEdit->text()))
            {
	     QMessageBox::warning( this, tr("ERROR"),
               tr("La cuenta de iva no es de repercutido"));
	     return;
            }
    
    if (ui.groupBox_2->isChecked() && !escuentadeivarepercutido(ui.CtaivalineEdit_2->text()))
            {
	     QMessageBox::warning( this, tr("ERROR"),
               tr("La cuenta de iva segunda no es de repercutido"));
	     return;
            }

    if (ui.groupBox_3->isChecked() && !escuentadeivarepercutido(ui.CtaivalineEdit_3->text()))
            {
	     QMessageBox::warning( this, tr("ERROR"),
               tr("La cuenta de iva tercera no es de repercutido"));
	     return;
            }

    if (!esauxiliar(ui.CtafralineEdit->text()))
            {
	     QMessageBox::warning( this, tr("ERROR"),
               tr("La cuenta de factura no es correcta"));
	     return;
            }


    QString cad_lista=basedatos::instancia()->selectCuentasacobrarconfiguracion().remove(' ');
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
          tr("¿ Desea borrar el contenido del campo ?"),
          tr("&Sí"), tr("&No"),
          QString(), 0, 1 ) ==1 )
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


void ivarep_aut::recuperadatos(QString *cuentabase, QString *baseimponible, QString *qclaveiva, QString *qtipoiva,
  QString *qtipore, QString *cuentaiva, QString *cuotaiva,

  QString *cuentabase2, QString *baseimponible2, QString *qclaveiva2, QString *qtipoiva2,
  QString *qtipore2, QString *cuentaiva2, QString *cuotaiva2,

  QString *cuentabase3, QString *baseimponible3, QString *qclaveiva3, QString *qtipoiva3,
  QString *qtipore3, QString *cuentaiva3, QString *cuotaiva3,

  QString *ctafra, QDate *qfechafra, QDate *qfechacont, QString *qdoc,
  QDate *qfechaop, QString *qclaveop, bool *rectificativa,
  QString *rectificada, QString *nfacturas, QString *finicial, QString *ffinal,
  bool *verifica, QString *nombre, QString *cif, bool *cajaiva , QString *externo)
{

*cuentabase=ui.CtabaselineEdit->text();
*baseimponible=ui.baselineEdit->text();
 if (baseimponible->isEmpty()) *baseimponible="0";
*qclaveiva=ui.ClaveivacomboBox->currentText();
*qtipoiva=ui.TipolineEdit->text();
*qtipore=ui.RElineEdit->text();
*cuentaiva=ui.CtaivalineEdit->text();
double cuotatot=convapunto(ui.CuotalineEdit->text()).toDouble() + convapunto(ui.CuotaRElineEdit->text()).toDouble();
*cuotaiva=formateanumero(cuotatot,comadecimal,decimales);
if (ui.rectificativacheckBox->isChecked())
   {
    *rectificada=ui.tipo_rectificativa_comboBox->currentText().left(2);
    if (!ui.rectificada_lineEdit->text().isEmpty())
        *rectificada+=":"+ui.rectificada_lineEdit->text();
   }
*nfacturas=ui.nfacturaslineEdit->text();
*finicial=ui.iniciallineEdit->text();
*ffinal=ui.finallineEdit->text();

*cuentabase2=ui.CtabaselineEdit_2->text();
*baseimponible2=ui.baselineEdit_2->text();
if (baseimponible2->isEmpty()) *baseimponible2="0";
*qclaveiva2=ui.ClaveivacomboBox_2->currentText();
*qtipoiva2=ui.TipolineEdit_2->text();
*qtipore2=ui.RElineEdit_2->text();
*cuentaiva2=ui.CtaivalineEdit_2->text();
cuotatot=convapunto(ui.CuotalineEdit_2->text()).toDouble() + convapunto(ui.CuotaRElineEdit_2->text()).toDouble();
*cuotaiva2=formateanumero(cuotatot,comadecimal,decimales);

*cuentabase3=ui.CtabaselineEdit_3->text();
*baseimponible3=ui.baselineEdit_3->text();
if (baseimponible3->isEmpty()) *baseimponible3="0";
*qclaveiva3=ui.ClaveivacomboBox_3->currentText();
*qtipoiva3=ui.TipolineEdit_3->text();
*qtipore3=ui.RElineEdit_3->text();
*cuentaiva3=ui.CtaivalineEdit_3->text();
cuotatot=convapunto(ui.CuotalineEdit_3->text()).toDouble() + convapunto(ui.CuotaRElineEdit_3->text()).toDouble();
*cuotaiva3=formateanumero(cuotatot,comadecimal,decimales);

*ctafra=ui.CtafralineEdit->text();
*qfechafra=ui.FechafradateEdit->date();
*qfechacont=ui.fechacontdateEdit->date();
*qdoc=ui.documentolineEdit->text();
*qfechaop=ui.fechaoperaciondateEdit->date();

QString cadenaclave=ui.claveoperacioncomboBox->currentText();
QString extracto=cadenaclave.section(' ',0,0);
*qclaveop=extracto.remove(' ');

*rectificativa=ui.rectificativacheckBox->isChecked();

*verifica=ui.verificarcheckBox->isChecked();

*nombre=ui.nombrelineEdit->text();
*cif=ui.ciflineEdit->text();
*cajaiva=ui.cajacheckBox->isChecked();
*externo=ui.externo_lineEdit->text();
}


void ivarep_aut::chequeabotonaceptar()
{
    bool bien;
    double bi;
    bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;

//     if (bi<0.0001 && bi>-0.0001)
//        { ui.AceptarButton->setEnabled(false); return; }
 
     if (!escuentadeivarepercutido(ui.CtaivalineEdit->text()))
        { ui.AceptarButton->setEnabled(false); return; }

     if (!esauxiliar(ui.CtabaselineEdit->text()))
        { 
       ui.AceptarButton->setEnabled(false);
           return;
        }

    if (!esauxiliar(ui.CtaivalineEdit->text())) 
        { ui.AceptarButton->setEnabled(false); return; }
    
    if (!escuentadeivarepercutido(ui.CtaivalineEdit->text()))
        { ui.AceptarButton->setEnabled(false); return; }

    if (ui.groupBox_2->isChecked())
       {
        bi = convapunto(ui.baselineEdit_2->text()).toDouble(& bien);
        if (!bien) bi=0;

        //if (bi<0.0001 && bi>-0.0001)
        //  { ui.AceptarButton->setEnabled(false); return; }
 
        if (!escuentadeivarepercutido(ui.CtaivalineEdit_2->text()))
          { ui.AceptarButton->setEnabled(false); return; }

        if (!esauxiliar(ui.CtabaselineEdit_2->text()))
          {
       ui.AceptarButton->setEnabled(false);
           return;
          }

        if (!esauxiliar(ui.CtaivalineEdit_2->text())) 
          { ui.AceptarButton->setEnabled(false); return; }
    
        if (!escuentadeivarepercutido(ui.CtaivalineEdit_2->text()))
          { ui.AceptarButton->setEnabled(false); return; }
       }

    if (ui.groupBox_3->isChecked())
       {
        bi = convapunto(ui.baselineEdit_3->text()).toDouble(& bien);
        if (!bien) bi=0;

        //if (bi<0.0001 && bi>-0.0001)
        //  { ui.AceptarButton->setEnabled(false); return; }
 
        if (!escuentadeivarepercutido(ui.CtaivalineEdit_3->text()))
          { ui.AceptarButton->setEnabled(false); return; }

        if (!esauxiliar(ui.CtabaselineEdit_3->text()))
          {
       ui.AceptarButton->setEnabled(false);
           return;
          }

        if (!esauxiliar(ui.CtaivalineEdit_3->text())) 
          { ui.AceptarButton->setEnabled(false); return; }
    
        if (!escuentadeivarepercutido(ui.CtaivalineEdit_3->text()))
          { ui.AceptarButton->setEnabled(false); return; }
       }

    if (!esauxiliar(ui.CtafralineEdit->text()))
        { ui.AceptarButton->setEnabled(false); return; }
    
    ui.AceptarButton->setEnabled(true);

}



void ivarep_aut::buscactabase()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtabaselineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.CtabaselineEdit->setText(cadena2);
       else ui.CtabaselineEdit->clear();
    delete(labusqueda);
}

void ivarep_aut::buscactabase2()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtabaselineEdit_2->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.CtabaselineEdit_2->setText(cadena2);
       else ui.CtabaselineEdit_2->clear();
    delete(labusqueda);
}

void ivarep_aut::buscactabase3()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtabaselineEdit_3->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.CtabaselineEdit_3->setText(cadena2);
       else ui.CtabaselineEdit_3->clear();
    delete(labusqueda);
}


void ivarep_aut::buscactaiva()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtaivalineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.CtaivalineEdit->setText(cadena2);
       else ui.CtaivalineEdit->clear();
    delete(labusqueda);
}

void ivarep_aut::buscactaiva2()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtaivalineEdit_2->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.CtaivalineEdit_2->setText(cadena2);
       else ui.CtaivalineEdit_2->clear();
    delete(labusqueda);
}

void ivarep_aut::buscactaiva3()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtaivalineEdit_3->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.CtaivalineEdit_3->setText(cadena2);
       else ui.CtaivalineEdit_3->clear();
    delete(labusqueda);
}

void ivarep_aut::buscactafra()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtafralineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.CtafralineEdit->setText(cadena2);
       else ui.CtafralineEdit->clear();
    delete(labusqueda);
}


void ivarep_aut::grboxcambiado2()
{
  if (!ui.groupBox_2->isChecked())
     {
      ui.CtabaselineEdit_2->clear();
      ui.CtaivalineEdit_2->clear();
      ui.baselineEdit_2->clear();
      // ui.TipolineEdit_2->clear();
      // ui.RElineEdit_2->clear();
      ui.CuotalineEdit_2->clear();
      ui.CuotaRElineEdit_2->clear();
     }
  chequeabotonaceptar();
}

void ivarep_aut::grboxcambiado3()
{
  if (!ui.groupBox_3->isChecked())
     {
      ui.CtabaselineEdit_3->clear();
      ui.CtaivalineEdit_3->clear();
      ui.baselineEdit_3->clear();
      // ui.TipolineEdit_3->clear();
      // ui.RElineEdit_3->clear();
      ui.CuotalineEdit_3->clear();
      ui.CuotaRElineEdit_3->clear();
     }
  chequeabotonaceptar();
}


void ivarep_aut::fechafracambiada()
{
 ui.fechacontdateEdit->setDate(ui.FechafradateEdit->date());
 ui.fechaoperaciondateEdit->setDate(ui.FechafradateEdit->date());
 formaconcepto();
}

void ivarep_aut::documento_cambiado()
{
 formaconcepto();
}

void ivarep_aut::cargacombooperaciones()
{
  QStringList lista=basedatos::instancia()->listaoperaciones_expedidas();
  ui.claveoperacioncomboBox->addItems (lista);
}

QString ivarep_aut::fichdocumento()
{
  return ui.fichdoclineEdit->text();
}

void ivarep_aut::pasa_fecha(QDate fecha)
{
    ui.FechafradateEdit->setDate(fecha);
    ui.fechacontdateEdit->setDate(fecha);
    ui.fechaoperaciondateEdit->setDate(fecha);
}


void ivarep_aut::buscacopiadoc()
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

void ivarep_aut::visdoc()
{
   if (ui.fichdoclineEdit->text().isEmpty()) return;
   if (!ejecuta(aplicacionabrirfich(extensionfich(ui.fichdoclineEdit->text())),ui.fichdoclineEdit->text()))
         QMessageBox::warning( this, tr("TABLA DE ASIENTOS"),
                             tr("No se puede abrir ")+ui.fichdoclineEdit->text()+tr(" con ")+
                             aplicacionabrirfich(extensionfich(ui.fichdoclineEdit->text())));

}

void ivarep_aut::borraasignaciondoc()
{
    ui.fichdoclineEdit->clear();
}


void ivarep_aut::formaconcepto()
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


QString ivarep_aut::concepto()
{
  return ui.conceptolineEdit->text();
}


void ivarep_aut::buscar_externo()
{
    busca_externo *b = new busca_externo();
    if (b->exec()==QDialog::Accepted)
      {
        ui.externo_lineEdit->setText(b->codigo_elec());
      }
      else ui.externo_lineEdit->clear();
    delete(b);
}


void ivarep_aut::externo_cambiado()
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

void ivarep_aut::fichero_soltado() {
    ui.fichdoclineEdit->setText(etiqueta->nombre_fich());
}
