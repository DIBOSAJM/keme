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

#include "ivasoportado.h"
#include "funciones.h"
#include "basedatos.h"
#include "aux_express.h"
#include <QMessageBox>

ivasoportado::ivasoportado(bool concomadecimal, bool condecimales) : QDialog() {
    ui.setupUi(this);

    ui.tipo_rectificativa_comboBox->addItems(tipos_rectificativas);
    if (conigic())
       {
        setWindowTitle(tr("IGIC soportado"));
        ui.TextLabel3->setText(tr("Clave IGIC"));
        ui.textLabel4->setText(tr("Cuenta IGIC"));
        ui.textLabel7->setText(tr("Cuota IGIC"));
       }
    comadecimal=concomadecimal;
    decimales=condecimales;
    cambiando_cuota=false;
    baseoblig=false;
  // Tenemos que cargar clave IVA por defecto y resto de IVA'S en el combo box
    QString pasalinea;
    QString Clavedefecto, laclave;
    double tipo;
    double re;
    QString qdescrip;
    QString convert;
    Clavedefecto=clave_iva_defecto();
    if (Clavedefecto.length()>0 && existeclavetipoiva(Clavedefecto,&tipo,&re,&qdescrip))
        {
	pasalinea=Clavedefecto;
	if (!conigic()) pasalinea+=tr(" // IVA:");
            else pasalinea+=tr(" // IGIC:");
	convert.setNum(tipo,'f',2);
	if (comadecimal) ui.TipolineEdit->setText(convacoma(convert));
            else ui.TipolineEdit->setText(convert);
	if (comadecimal) pasalinea+=convacoma(convert);
            else pasalinea+=convert;
	pasalinea+=tr(" RE:");
	convert.setNum(re,'f',2);
	if (comadecimal) pasalinea+=convacoma(convert);
            else pasalinea+=convert;
	pasalinea += " : ";
	pasalinea += qdescrip;
	ui.ClaveivacomboBox->insertItem ( 0,pasalinea) ;
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
	  pasalinea += " : ";
	  pasalinea += qdescrip;
	  ui.ClaveivacomboBox->insertItem ( -1,pasalinea) ;
         }
      }

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

 cargacombooperaciones();

 if (basedatos::instancia()->hay_criterio_caja_config())
     ui.cajacheckBox->setChecked(true);

 connect(ui.ClaveivacomboBox,SIGNAL(activated(int)),this,SLOT(comboivacambiado()));
 connect(ui.CtabaselineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctabasecambiada()));
 connect(ui.CtabaselineEdit,SIGNAL(editingFinished()),this,SLOT(ctabasefinedicion()));
 connect(ui.CuotalineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuotacambiada()));

 connect(ui.CtaivalineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctaivacambiada()));
 connect(ui.CtaivalineEdit,SIGNAL(editingFinished()),this,SLOT(ctaivafinedicion()));
 connect(ui.CtafralineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctafracambiada()));
 connect(ui.CtafralineEdit,SIGNAL(editingFinished()),this,SLOT(ctafrafinedicion()));
 connect(ui.baselineEdit,SIGNAL(textChanged(QString)),this,SLOT(basecambiada()));
 connect(ui.baselineEdit,SIGNAL(editingFinished()),this,SLOT(basefinedicion()));
 connect(ui.totalfralineEdit,SIGNAL(textChanged(QString)),this,SLOT(totalfracambiado()));
 connect(ui.totalfralineEdit,SIGNAL(editingFinished()),this,SLOT(totalfrafinedicion()));
 connect(ui.AceptarButton,SIGNAL(clicked()),this,SLOT(botonaceptar()));
 connect(ui.CuotalineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuotacambiada()));
 connect(ui.ctabasepushButton,SIGNAL(clicked()),this,SLOT(buscactabase()));
 connect(ui.ctaivapushButton,SIGNAL(clicked()),this,SLOT(buscactaiva()));
 connect(ui.ctafrapushButton,SIGNAL(clicked()),this,SLOT(buscactafra()));

 connect(ui.CtabaselineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));
 connect(ui.CtaivalineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));
 connect(ui.CtafralineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));

 connect(ui.FechafradateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(fechafracambiada()));

 connect(ui.bicostelineEdit,SIGNAL(textChanged(QString)),this,SLOT(bicostecambiada()));

 connect(ui.afectacionlineEdit,SIGNAL(editingFinished()),this,SLOT(afectacionfinedicion()));

}


void ivasoportado::comboivacambiado()
{
 ui.CuotalineEdit->disconnect(SIGNAL(textChanged(QString)));

QString cadenaiva=ui.ClaveivacomboBox->currentText();
QString extracto=cadenaiva.section(':',1,1);
ui.TipolineEdit->setText(extracto.remove(tr(" RE")));

    bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;

     double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;

     double tiporet=convapunto(ui.tiporetlineEdit->text()).toDouble()/100;

     int ndecimales= decimales ? 2 : 0;

     QString cadena;

     if (ui.CtabaselineEdit->text().isEmpty())
        {
         double totalfra=convapunto(ui.totalfralineEdit->text()).toDouble();
         if ((totalfra >0.001 || totalfra<-0.001) && tiporet<0.0001)
          {
           bi=CutAndRoundNumberToNDecimals (totalfra/(1+tipo), ndecimales);
           cadena.setNum(bi,'f',2); if (comadecimal) cadena=convacoma(cadena);
           ui.baselineEdit->setText(cadena);
          }
        }
     double cuota=CutAndRoundNumberToNDecimals (bi*tipo, ndecimales);
     if (decimales)
         cadena.setNum(cuota,'f',2);
       else cadena.setNum(cuota,'f',0);
     if (comadecimal) ui.CuotalineEdit->setText( convacoma(cadena));
         else ui.CuotalineEdit->setText(cadena);

     double retencion=CutAndRoundNumberToNDecimals (bi*tiporet, ndecimales);
     if (decimales)
         cadena.setNum(retencion,'f',2);
       else cadena.setNum(retencion,'f',0);
     if (comadecimal) ui.retencionlineEdit->setText( convacoma(cadena));
         else ui.retencionlineEdit->setText(cadena);

     double total=convapunto(ui.baselineEdit->text()).toDouble(&bien)+
                  convapunto(ui.CuotalineEdit->text()).toDouble(&bien);
     if (decimales)
         cadena.setNum(total,'f',2);
       else cadena.setNum(total,'f',0);
     if (comadecimal) ui.totalfralineEdit->setText( convacoma(cadena));
         else ui.totalfralineEdit->setText(cadena);

     double total_tras_ret=total-retencion;
     if (decimales)
         cadena.setNum(total_tras_ret,'f',2);
       else cadena.setNum(total_tras_ret,'f',0);
     if (comadecimal) ui.total_tras_ret_lineEdit->setText( convacoma(cadena));
         else ui.total_tras_ret_lineEdit->setText(cadena);

   chequeabotonaceptar();
   connect(ui.CuotalineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuotacambiada()));

}


void ivasoportado::ctabasecambiada()
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

void ivasoportado::ctabasefinedicion()
{
  QString cadena;
  ui.CtabaselineEdit->setText(expandepunto(ui.CtabaselineEdit->text(),anchocuentas()));
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
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


void ivasoportado::ctaivacambiada()
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
            chequeabotonaceptar();
	  }
        }
  if (esauxiliar(ui.CtaivalineEdit->text()))
       ui.ivatextLabel->setText(descripcioncuenta(ui.CtaivalineEdit->text()));
    else ui.ivatextLabel->setText("");
}

void ivasoportado::ctaivafinedicion()
{
   ui.CtaivalineEdit->setText(expandepunto(ui.CtaivalineEdit->text(),anchocuentas()));
 QString cadena;
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
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
            chequeabotonaceptar();
	  }
        if (esauxiliar(ui.CtaivalineEdit->text()))
           ui.ivatextLabel->setText(descripcioncuenta(ui.CtaivalineEdit->text()));
          else ui.ivatextLabel->setText("");
        }

}


void ivasoportado::ctafracambiada()
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
       {
	ui.fratextLabel->setText(descripcioncuenta(ui.CtafralineEdit->text()));
        if (basedatos::instancia()->hay_criterio_caja_aux(ui.CtafralineEdit->text()))
            ui.cajacheckBox->setChecked(true);
       }
    else ui.fratextLabel->setText("");
}

void ivasoportado::ctafrafinedicion()
{
   ui.CtafralineEdit->setText(expandepunto(ui.CtafralineEdit->text(),anchocuentas()));
 QString cadena;
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
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

void ivasoportado::basecambiada()
{
    ui.CuotalineEdit->disconnect(SIGNAL(textChanged(QString)));

    bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     QString cadena;
     int ndecimales=2;
     if (!decimales) ndecimales=0;
     double cuota=CutAndRoundNumberToNDecimals (bi*tipo, ndecimales);
     double tiporet=convapunto(ui.tiporetlineEdit->text()).toDouble()/100;
     double retencion=CutAndRoundNumberToNDecimals (bi*tiporet, ndecimales);

     cadena.setNum(cuota,'f',ndecimales);
     if (!cambiando_cuota)
       {
        if (comadecimal)
           ui.CuotalineEdit->setText( convacoma(cadena));
         else
            ui.CuotalineEdit->setText( cadena);
       }

     cadena.setNum(retencion,'f',ndecimales);
     if (comadecimal)
         ui.retencionlineEdit->setText( convacoma(cadena));
       else
         ui.retencionlineEdit->setText( cadena);

     if (comadecimal) ui.baselineEdit->setText(ui.baselineEdit->text().replace('.',','));
     double total=bi+cuota;
     double total_tras_ret=bi+cuota-retencion;

     cadena.setNum(total,'f',ndecimales);
     if (comadecimal)
         ui.totalfralineEdit->setText(convacoma(cadena));
        else
         ui.totalfralineEdit->setText(cadena);

     cadena.setNum(total_tras_ret,'f',ndecimales);
     if (comadecimal)
         ui.total_tras_ret_lineEdit->setText(convacoma(cadena));
       else
         ui.total_tras_ret_lineEdit->setText(cadena);

  connect(ui.CuotalineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuotacambiada()));

  chequeabotonaceptar();
}

void ivasoportado::basefinedicion()
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


// -----------------------------------------------------------------------------------
void ivasoportado::cuotacambiada()
{
    ui.CuotalineEdit->disconnect(SIGNAL(textChanged(QString)));
    cambiando_cuota=true;
    /*
    double cuota=convapunto(ui.CuotalineEdit->text()).toDouble();
    double bi=0;
    double tipo = convapunto(ui.TipolineEdit->text()).toDouble()/100;
    int ndecimales=2;
    if (!decimales) ndecimales=0;
    double cuota_antes_cambio=CutAndRoundNumberToNDecimals (bi*tipo,ndecimales);
    if (cuota-cuota_antes_cambio>0.01 || cuota-cuota_antes_cambio<-0.01)
     {
      if (tipo>0)
        bi=CutAndRoundNumberToNDecimals (cuota/tipo, ndecimales);
     }
    QString cadena;
    cadena.setNum(bi,'f',ndecimales);
    if (comadecimal)
         ui.baselineEdit->setText( convacoma(cadena));
       else
         ui.baselineEdit->setText( cadena);

    if (comadecimal) ui.CuotalineEdit->setText(ui.CuotalineEdit->text().replace('.',','));
    */
    //------------------------------------------------------------------------------------
    double cuota=convapunto(ui.CuotalineEdit->text()).toDouble();
    // double bi=0;
    // double tipo = convapunto(ui.TipolineEdit->text()).toDouble()/100;
    int ndecimales=2;
    if (!decimales) ndecimales=0;
    // double cuota_correcta=CutAndRoundNumberToNDecimals (bi*tipo,ndecimales);
    // if (cuota-cuota_correcta<=0.01 && cuota-cuota_correcta>=-0.01)
       {
        // calculamos total factura
        double total_fac=0;
        double retencion=0;
        if (!ui.retencionlineEdit->text().isEmpty())
            retencion=convapunto(ui.retencionlineEdit->text()).toDouble();
        double total_fac_antes_ret=convapunto(ui.baselineEdit->text()).toDouble()+cuota;
        total_fac_antes_ret=CutAndRoundNumberToNDecimals (total_fac_antes_ret,ndecimales);
        total_fac=convapunto(ui.baselineEdit->text()).toDouble() +
                  cuota - retencion;
        total_fac=CutAndRoundNumberToNDecimals (total_fac,ndecimales);
        ui.totalfralineEdit->setText(formateanumero(total_fac_antes_ret,comadecimal,decimales));
        ui.total_tras_ret_lineEdit->setText(formateanumero(total_fac,comadecimal,decimales));
       }

    //------------------------------------------------------------------------------------
    connect(ui.CuotalineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuotacambiada()));
    cambiando_cuota=false;
  chequeabotonaceptar();
}


// -----------------------------------------------------------------------------------

void ivasoportado::totalfracambiado()
{
 // si bi+cuota=totalfra no cambiar nada
 /*bool bien;
 double totalfra;
 QString cadena;
 QString guardacadena=ui.totalfralineEdit->text();
 if (comadecimal) guardacadena=convacoma(guardacadena);
   else guardacadena=convapunto(guardacadena);
 totalfra=convapunto(ui.totalfralineEdit->text()).toDouble(&bien);
 if (!bien) totalfra=0;
 // cadena=formateanumero(totalfra,comadecimal,decimales);
 */

 if (comadecimal && ui.totalfralineEdit->text().contains('.'))
      ui.totalfralineEdit->setText(ui.totalfralineEdit->text().replace('.',','));

 /*
 double bi = convapunto(ui.baselineEdit->text()).toDouble(&bien);
 if (!bien) bi=0;
 double cuota = convapunto(ui.CuotalineEdit->text()).toDouble(& bien);
 if (!bien) cuota=0;
 if (bi+cuota-totalfra>-0.005 && bi+cuota-totalfra<0.005) return;
 // if (!decimales && bi+cuota-totalfra>-0.1 && bi+cuota-totalfra<0.1) return;
 // recalculamos bi y cuota
     double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
 if (tipo>0.0001) bi=totalfra/(1+tipo); else bi=0;
 ui.baselineEdit->setText(formateanumero(bi,comadecimal,decimales));
 cuota=bi*tipo;
 ui.CuotalineEdit->setText(formateanumero(cuota,comadecimal,decimales));

 ui.totalfralineEdit->setText(guardacadena);
*/
}

void ivasoportado::totalfrafinedicion()
{
    bool bien;
    double total = convapunto(ui.totalfralineEdit->text()).toDouble(& bien);
    if (!bien) total=0;
    ui.totalfralineEdit->setText(formateanumero(total,comadecimal,decimales));
}

void ivasoportado::botonaceptar()
{
    // bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble();
     // if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit->text()).toDouble()/100;
     // if (!bien) tipo=0;
     double cuota = convapunto(ui.CuotalineEdit->text()).toDouble();
     // if (!bien) cuota=0;
     int ndecimales= decimales ? 2 : 0;
     double cuota_calc=CutAndRoundNumberToNDecimals (bi*tipo, ndecimales);
     if (cuota_calc-cuota>=0.011 || cuota_calc-cuota<=-0.011)
            {
	 QMessageBox::warning( this, tr("ERROR"),
               tr("La cuota es incorrecta"));
	 return;
            }
	 
     if (bi<0.0001 && bi>-0.0001) 
            {
             //QMessageBox::warning( this, tr("ERROR"),
             //  tr("La base imponible no puede ser nula"));
             //return;
            }
     /*
     if (cuota<0.0001 && cuota >-0.0001) 
            {
	 QMessageBox::warning( this, tr("ERROR"),
               tr("La cuota no puede ser nula"));
	 return;
            }
     */
    if (!esauxiliar(ui.CtabaselineEdit->text())) 
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
     
    if (!esauxiliar(ui.CtaivalineEdit->text())) 
            {
	 QMessageBox::warning( this, tr("ERROR"),
               tr("La cuenta de iva no es correcta"));
	 return;
            }
    
    if (!escuentadeivasoportado(ui.CtaivalineEdit->text()))
            {
	     QMessageBox::warning( this, tr("ERROR"),
               tr("La cuenta de iva no es correcta"));
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

QString ivasoportado::valor_retencion()
{
  return ui.retencionlineEdit->text();
}

void ivasoportado::recuperadatos(QString *cuentabase, QString *baseimponible,
                                  QString *qclaveiva, QString *qtipoiva,
                                  QString *cuentaiva, QString *cuotaiva,
                                  QString *ctafra, QDate *qfechafra,
                                  QString *soportado, QString *prorrata,
                                  QString *rectificativa,
                                  QDate *fechaop, QString *claveop,
                                  QString *bicoste,
                                  QString *frectif, QString *numfact,
                                  QString *facini, QString *facfinal, QString *bieninversion,
                                  QString *afectacion, QString *qagrario, QString *qnombre,
                                  QString *qcif, QString *import, QString *cajaiva, bool *no347,
                                  bool *arrto_ret, bool *arrto_sin_ret, QString *dua)

{
*cuentabase=ui.CtabaselineEdit->text();
*baseimponible=ui.baselineEdit->text();
QString cadenaiva=ui.ClaveivacomboBox->currentText();
QString extracto=cadenaiva.section("//",0,0);
*qclaveiva=extracto.remove(' ');
*qtipoiva=ui.TipolineEdit->text();
*cuentaiva=ui.CtaivalineEdit->text();
*cuotaiva=ui.CuotalineEdit->text();
*ctafra=ui.CtafralineEdit->text();
*qfechafra=ui.FechafradateEdit->date();
if (ui.prorratacheckBox->isChecked()) *prorrata=ui.prorratalineEdit->text();
   else *prorrata="";
*soportado="1";

if (ui.rectificativagroupBox->isChecked()) *rectificativa="1";
   else *rectificativa="0";

*fechaop=ui.FechaopdateEdit->date();
QString cadenaclave=ui.clavecomboBox->currentText();
extracto=cadenaclave.section(' ',0,0);
*claveop=extracto.remove(' ');
*bicoste=ui.bicostelineEdit->text();

if (ui.rectificativagroupBox->isChecked()) {
    *frectif=ui.tipo_rectificativa_comboBox->currentText().left(2);
    if (ui.rectificada_lineEdit->text().length()>0)
        *frectif+=":";
    *frectif+=ui.rectificada_lineEdit->text();
}
   else *frectif="";
*numfact=ui.numfacturaslineEdit->text();
*facini=ui.iniciallineEdit->text();
*facfinal=ui.finallineEdit->text();
*bieninversion=ui.bieninversioncheckBox->isChecked() ? "1" : "0";
*afectacion=ui.afectacionlineEdit->text();
*qagrario=ui.agrariocheckBox->isChecked() ? "1" : "0";
*qnombre=ui.nombrelineEdit->text();
*qcif=ui.ciflineEdit->text();
*import=ui.importcheckBox->isChecked() ? "1" : "";
*cajaiva=ui.cajacheckBox->isChecked() ? "1" : "";
*no347=ui.NO347checkBox->isChecked();
*arrto_ret=ui.arr_ret_checkBox->isChecked();
*arrto_sin_ret=ui.arr_sin_ret_checkBox->isChecked();
*dua=ui.dua_lineEdit->text();
}


void ivasoportado::chequeabotonaceptar()
{
    bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     double cuota = convapunto(ui.CuotalineEdit->text()).toDouble(& bien);
     int ndecimales= decimales ? 2 : 0;
     double bixtipo=CutAndRoundNumberToNDecimals (bi*tipo, ndecimales);
     if (!bien) cuota=0;
     
     if (!escuentadeivasoportado(ui.CtaivalineEdit->text()))
        { ui.AceptarButton->setEnabled(false); return; }

     if (bixtipo-cuota>=0.011 || bixtipo-cuota<=-0.011) {}
     //   { ui.AceptarButton->setEnabled(false); return; }


     // permitimos cuotas cero
     /* if (bi<0.0001 && bi>-0.0001)
        { ui.AceptarButton->setEnabled(false); return; }
	 
     if (cuota<0.0001 && cuota >-0.0001) 
        { ui.AceptarButton->setEnabled(false); return; } */
     
     // QString cadena="";
    if (!esauxiliar(ui.CtabaselineEdit->text()))
        { 
	if (ui.CtabaselineEdit->text().length()!=0)
       { ui.AceptarButton->setEnabled(false); return; }
	if (baseoblig && ui.CtabaselineEdit->text().length()==0)
       { ui.AceptarButton->setEnabled(false); return; }
        }
     
    if (!esauxiliar(ui.CtaivalineEdit->text()))
        { ui.AceptarButton->setEnabled(false); return; }
    
    if (!escuentadeivasoportado(ui.CtaivalineEdit->text()))
        { ui.AceptarButton->setEnabled(false); return; }
    
    if (!esauxiliar(ui.CtafralineEdit->text()))
        { ui.AceptarButton->setEnabled(false); return; }
    
    ui.AceptarButton->setEnabled(true);

}

void ivasoportado::pasadatos_base( QString ctabase, QString baseimponible, QString qcuentaiva,
                                  QDate qfechafra, QString porc_ret )
{
 // ui.CtabaselineEdit->setText(cuentabase);
 // if (cuentabase.length()>0) ui.basetextLabel->setText(descripcioncuenta(cuentabase));
 // ui.CuotalineEdit->setText(cuotaiva);
 ui.baselineEdit->setText(baseimponible);
 ui.CtaivalineEdit->setText(qcuentaiva);
 if (qcuentaiva.length()>0) ui.ivatextLabel->setText(descripcioncuenta(qcuentaiva));
 ui.CtabaselineEdit->setText(ctabase);
 if (ctabase.length()>0) ui.basetextLabel->setText(descripcioncuenta(ctabase));
 ui.FechafradateEdit->setDate(qfechafra);
 ui.tiporetlineEdit->setText(comadecimal ? convacoma(porc_ret) : porc_ret);
 comboivacambiado();
}

void ivasoportado::pasadatos( QString cuentabase, QString baseimponible, QString qcuentaiva,
                              QString cuotaiva, QString ctafra, QDate qfechafra )
{
 ui.CuotalineEdit->disconnect(SIGNAL(textChanged(QString)));
 ui.baselineEdit->disconnect(SIGNAL(textChanged(QString)));

 ui.CtabaselineEdit->setText(cuentabase);
 if (cuentabase.length()>0) ui.basetextLabel->setText(descripcioncuenta(cuentabase));
 ui.CuotalineEdit->setText(cuotaiva);
 ui.baselineEdit->setText(baseimponible);
 ui.CtaivalineEdit->setText(qcuentaiva);
 if (qcuentaiva.length()>0) ui.ivatextLabel->setText(descripcioncuenta(qcuentaiva));
 ui.CtafralineEdit->setText(ctafra);
 if (ctafra.length()>0) ui.fratextLabel->setText(descripcioncuenta(ctafra));
 ui.FechafradateEdit->setDate(qfechafra);
 // solo comboivacambiado si cuota no correcta (error +/- 0.01)
 double bi=0;
 bi = convapunto(ui.baselineEdit->text()).toDouble();
 double tipo = 0;
 tipo=convapunto(ui.TipolineEdit->text()).toDouble()/100;
 int ndecimales= decimales ? 2 : 0;
 double cuota_calc=CutAndRoundNumberToNDecimals (bi*tipo, ndecimales);
 double cuota_campo=convapunto(ui.CuotalineEdit->text()).toDouble();
 if (cuota_calc-cuota_campo>=0.01 || cuota_calc-cuota_campo<=-0.01)
     comboivacambiado();
   else
       {
       bool bien;
       double total=convapunto(ui.baselineEdit->text()).toDouble(&bien)+
                    convapunto(ui.CuotalineEdit->text()).toDouble(&bien);
       QString cadena;
       if (decimales)
           cadena.setNum(total,'f',2);
         else cadena.setNum(total,'f',0);
       if (comadecimal) ui.totalfralineEdit->setText( convacoma(cadena));
           else ui.totalfralineEdit->setText(cadena);
       double tiporet=convapunto(ui.tiporetlineEdit->text()).toDouble()/100;
       double retencion=CutAndRoundNumberToNDecimals (bi*tiporet, ndecimales);
       if (decimales)
          cadena.setNum(retencion,'f',2);
         else cadena.setNum(retencion,'f',0);
          if (comadecimal) ui.retencionlineEdit->setText( convacoma(cadena));
            else ui.retencionlineEdit->setText(cadena);
       double total_tras_ret=total-retencion;
       if (decimales)
           cadena.setNum(total_tras_ret,'f',2);
         else cadena.setNum(total_tras_ret,'f',0);
       if (comadecimal) ui.total_tras_ret_lineEdit->setText( convacoma(cadena));
           else ui.total_tras_ret_lineEdit->setText(cadena);

       }
 // -----------------------------------------------------------
 connect(ui.baselineEdit,SIGNAL(textChanged(QString)),this,SLOT(basecambiada()));
 connect(ui.CuotalineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuotacambiada()));

}

void ivasoportado::pasadatos_fin( QString ctafra, QString baseimponible, QString qcuentaiva,
                                  QDate qfechafra, QString porc_ret, QString cta_base_iva )
{
 // ui.CtabaselineEdit->setText(cuentabase);
 // if (cuentabase.length()>0) ui.basetextLabel->setText(descripcioncuenta(cuentabase));
 // ui.CuotalineEdit->setText(cuotaiva);
 ui.baselineEdit->setText(baseimponible);
 ui.CtaivalineEdit->setText(qcuentaiva);
 if (qcuentaiva.length()>0) ui.ivatextLabel->setText(descripcioncuenta(qcuentaiva));
 ui.CtafralineEdit->setText(ctafra);
 if (ctafra.length()>0) ui.fratextLabel->setText(descripcioncuenta(ctafra));
 ui.FechafradateEdit->setDate(qfechafra);
 ui.tiporetlineEdit->setText(comadecimal ? convacoma(porc_ret) : porc_ret);
 ui.CtabaselineEdit->setText(cta_base_iva);
 comboivacambiado();
}


void ivasoportado::pasadatos2( QString cuentabase, QString baseimponible, QString qclaveiva,
                               QString qtipoiva, QString cuentaiva, QString cuotaiva,
                               QString ctafra, QDate qfechafra, QString qprorrata,
                               QString rectificativa,
                               QDate fechaop, QString claveop,
                               QString bicoste,
                               QString frectif, QString numfact,
                               QString facini, QString facfinal, QString binversion,
                               QString afectacion, QString agrario,
                               QString nombre, QString cif, QString import, QString cajaiva, bool no347,
                                bool arrto_ret, bool arrto_sin_ret, QString dua)
{
ui.CtabaselineEdit->setText(cuentabase);
 if (cuentabase.length()>0) ui.basetextLabel->setText(descripcioncuenta(cuentabase));

QString cadenaiva;
QString extracto;

for (int veces=0;veces<ui.ClaveivacomboBox->count();veces++)
     {
        cadenaiva=ui.ClaveivacomboBox->itemText(veces);
        extracto=cadenaiva.section("//",0,0);
        extracto=extracto.remove(' ');
        if (extracto==qclaveiva)
	{
	    ui.ClaveivacomboBox->setCurrentIndex(veces);
	    break;
	}
     }

ui.TipolineEdit->setText(qtipoiva);
ui.CtaivalineEdit->setText(cuentaiva);
if (cuentaiva.length()>0) ui.ivatextLabel->setText(descripcioncuenta(cuentaiva));
ui.baselineEdit->setText(baseimponible);
ui.CuotalineEdit->setText(cuotaiva);
ui.CtafralineEdit->setText(ctafra);
if (ctafra.length()>0) ui.fratextLabel->setText(descripcioncuenta(ctafra));
ui.FechafradateEdit->setDate(qfechafra);
if (!qprorrata.isEmpty())
  {
   ui.prorratalineEdit->setText(qprorrata);
  }
if (prorrata_especial()) 
   {
    if (qprorrata.length()>0)
       {
        ui.prorratalineEdit->setText(qprorrata);
        ui.prorratacheckBox->setChecked(true);
       }
   }

 ui.FechaopdateEdit->setDate(fechaop);
 // buscar claveop en el combo
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

 ui.bicostelineEdit->setText(bicoste);
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

 ui.numfacturaslineEdit->setText(numfact);
 ui.iniciallineEdit->setText(facini);
 ui.finallineEdit->setText(facfinal);
 ui.bieninversioncheckBox->setChecked(binversion=="1");
 ui.afectacionlineEdit->setText(afectacion);
 ui.agrariocheckBox->setChecked(agrario=="1");
 ui.nombrelineEdit->setText(nombre);
 ui.ciflineEdit->setText(cif);
 ui.importcheckBox->setChecked(import=="1");
 ui.cajacheckBox->setChecked(cajaiva=="1");
 ui.NO347checkBox->setChecked(no347);
 //bool arrto_ret, bool arrto_sin_ret, QString dua
 ui.arr_ret_checkBox->setChecked(arrto_ret);
 ui.arr_sin_ret_checkBox->setChecked(arrto_sin_ret);
 ui.dua_lineEdit->setText(dua);
}

void ivasoportado::buscactabase()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtabaselineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.CtabaselineEdit->setText(cadena2);
       else ui.CtabaselineEdit->setText("");
    delete(labusqueda);
}


void ivasoportado::buscactaiva()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtaivalineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.CtaivalineEdit->setText(cadena2);
       else ui.CtaivalineEdit->setText("");
    delete(labusqueda);
}


void ivasoportado::buscactafra()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtafralineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (esauxiliar(cadena2)) ui.CtafralineEdit->setText(cadena2);
       else ui.CtafralineEdit->setText("");
    delete(labusqueda);
}




void ivasoportado::hazbaseoblig()
{
  baseoblig=true;
  QDate fecha;
  ui.FechafradateEdit->setDate(fecha.currentDate());
  comboivacambiado();
}

void ivasoportado::pasatipoiva(QString tipo)
{
  // buscamos tipoiva en ClaveivacomboBox
  int indice=0;
  while (indice<ui.ClaveivacomboBox->count())
     {
      QString texto=ui.ClaveivacomboBox->itemText(indice);
      QString extracto=texto.section("//",0,0).trimmed();
      if (extracto==tipo) break;
      indice++;
     }
  if (indice>ui.ClaveivacomboBox->count()) indice--;
  ui.ClaveivacomboBox->setCurrentIndex(indice);
  comboivacambiado();
}

void ivasoportado::fococtafra()
{
    ui.CtafralineEdit->setFocus();
}

void ivasoportado::fococtabi()
{
    ui.CtabaselineEdit->setFocus();
}



void ivasoportado::cargacombooperaciones()
{
  QStringList lista=basedatos::instancia()->listaoperaciones_recibidas();
  ui.clavecomboBox->addItems (lista);
}

void ivasoportado::fechafracambiada()
{
 ui.FechaopdateEdit->setDate(ui.FechafradateEdit->date());
}


void ivasoportado::bicostecambiada()
{
  if (comadecimal)
          ui.bicostelineEdit->setText( convacoma(ui.bicostelineEdit->text()));
}

void ivasoportado::afectacionfinedicion()
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


void ivasoportado::modoconsulta()
{
  ui.afectacionlineEdit->setReadOnly(true);
  ui.baselineEdit->setReadOnly(true);
  ui.bicostelineEdit->setReadOnly(true);
  ui.bieninversioncheckBox->setEnabled(false);
  ui.importcheckBox->setEnabled(false);
  ui.clavecomboBox->setEnabled(false);
  ui.ClaveivacomboBox->setEnabled(false);
  ui.CtabaselineEdit->setReadOnly(true);
  ui.ctabasepushButton->setEnabled(false);
  ui.ctabasepushButton->setEnabled(false);
  ui.CtafralineEdit->setReadOnly(true);
  ui.ctafrapushButton->setEnabled(false);
  ui.CtaivalineEdit->setReadOnly(true);
  ui.ctaivapushButton->setEnabled(false);
  ui.CuotalineEdit->setReadOnly(true);
  ui.FechafradateEdit->setReadOnly(true);
  ui.FechaopdateEdit->setReadOnly(true);
  ui.finallineEdit->setReadOnly(true);
  ui.iniciallineEdit->setReadOnly(true);
  ui.numfacturaslineEdit->setReadOnly(true);
  ui.prorratacheckBox->setEnabled(false);
  ui.rectificativagroupBox->setEnabled(false);
  ui.TipolineEdit->setReadOnly(true);
  ui.totalfralineEdit->setReadOnly(true);
  ui.agrariocheckBox->setEnabled(false);
  ui.nombrelineEdit->setReadOnly(true);
  ui.ciflineEdit->setReadOnly(true);
  ui.cajacheckBox->setEnabled(false);
  ui.arr_ret_checkBox->setEnabled(false);
  ui.arr_sin_ret_checkBox->setEnabled(false);
  ui.dua_lineEdit->setReadOnly(true);
}

void ivasoportado::desactiva_base_tipo() {
    ui.baselineEdit->setReadOnly(true);
    ui.ClaveivacomboBox->setEnabled(false);
    ui.CuotalineEdit->setEnabled(false);

}
