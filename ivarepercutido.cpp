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

#include "ivarepercutido.h"
#include "funciones.h"
#include "basedatos.h"
#include "aux_express.h"
#include <QMessageBox>

ivarepercutido::ivarepercutido(bool concomadecimal, bool condecimales) : QDialog() {
    ui.setupUi(this);

    ui.tipo_rectificativa_comboBox->addItems(tipos_rectificativas);

    if (conigic())
       {
        setWindowTitle(tr("IGIC repercutido"));
        ui.TextLabel3->setText(tr("Clave IGIC"));
        ui.textLabel4->setText(tr("Cuenta IGIC"));
        ui.textLabel7->setText(tr("Cuota IGIC"));
       }
    comadecimal=concomadecimal;
    decimales=condecimales;

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

	if (comadecimal) ui.RElineEdit->setText(convacoma(convert));
            else ui.RElineEdit->setText(convert);

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

 cargacombooperaciones();
 if (basedatos::instancia()->hay_criterio_caja_config())
     ui.cajacheckBox->setChecked(true);

 connect(ui.ClaveivacomboBox,SIGNAL(activated(int)),this,SLOT(comboivacambiado()));
 connect(ui.CtabaselineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctabasecambiada()));
 connect(ui.CtabaselineEdit,SIGNAL(editingFinished()),this,SLOT(ctabasefinedicion()));
 connect(ui.CtaivalineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctaivacambiada()));
 connect(ui.CtaivalineEdit,SIGNAL(editingFinished()),this,SLOT(ctaivafinedicion()));
 connect(ui.CtafralineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctafracambiada()));
 connect(ui.CtafralineEdit,SIGNAL(editingFinished()),this,SLOT(ctafrafinedicion()));
 connect(ui.baselineEdit,SIGNAL(textChanged(QString)),this,SLOT(basecambiada()));
 connect(ui.baselineEdit,SIGNAL(editingFinished()),this,SLOT(basefinedicion()));
 connect(ui.AceptarButton,SIGNAL(clicked()),this,SLOT(botonaceptar()));
 connect(ui.CuotalineEdit,SIGNAL(textChanged(QString)),this,SLOT(cuotacambiada()));
 connect(ui.buscabasepushButton,SIGNAL(clicked()),this,SLOT(buscactabase()));
 connect(ui.buscaivapushButton,SIGNAL(clicked()),this,SLOT(buscactaiva()));
 connect(ui.buscafrapushButton,SIGNAL(clicked()),this,SLOT(buscactafra()));
 connect(ui.CtabaselineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));
 connect(ui.CtaivalineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));
 connect(ui.CtafralineEdit,SIGNAL(textChanged(QString)),this,SLOT(chequeabotonaceptar()));


 connect(ui.FechafradateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(fechafracambiada()));

 connect(ui.bicostelineEdit,SIGNAL(textChanged(QString)),this,SLOT(bicostecambiada()));

}

void ivarepercutido::comboivacambiado()
{
QString cadenaiva=ui.ClaveivacomboBox->currentText();
QString extracto=cadenaiva.section(':',1,1);
ui.TipolineEdit->setText(extracto.remove(tr(" RE")));

extracto=cadenaiva.section(':',2,2);
ui.RElineEdit->setText(convacoma(extracto.remove(' ')));

    bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     double re = convapunto(ui.RElineEdit->text()).toDouble( & bien)/100;
     if (!bien) re=0;
     QString cadena;
     int ndecimales= decimales ? 2 : 0;

     if (ui.CtabaselineEdit->text().isEmpty())
        {
         double totalfra=bi+convapunto(ui.CuotaTOTlineEdit->text()).toDouble();
         if ((totalfra >0.001 || totalfra<-0.001))
          {
           bi=CutAndRoundNumberToNDecimals (totalfra/(1+tipo+re), ndecimales);
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

     double bixre=CutAndRoundNumberToNDecimals (bi*re, ndecimales);
     if (decimales)
         cadena.setNum(bixre,'f',2);
       else cadena.setNum(bixre,'f',0);
     if (comadecimal) ui.CuotaRElineEdit->setText(convacoma(cadena));
         else ui.CuotaRElineEdit->setText(cadena);

     if (decimales) cadena.setNum(cuota+bixre,'f',2);
         else cadena.setNum(cuota+bixre,'f',0);
     if (comadecimal) ui.CuotaTOTlineEdit->setText(convacoma(cadena));
         else ui.CuotaTOTlineEdit->setText(cadena);

   chequeabotonaceptar();
}

void ivarepercutido::ctabasecambiada()
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

void ivarepercutido::ctabasefinedicion()
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


void ivarepercutido::ctaivacambiada()
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
		   lasubcuenta2->pasacodigo(ui.CtabaselineEdit->text());
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
  if (esauxiliar(ui.CtaivalineEdit->text()))
       ui.ivatextLabel->setText(descripcioncuenta(ui.CtaivalineEdit->text()));
    else ui.ivatextLabel->setText("");
}

void ivarepercutido::ctaivafinedicion()
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


void ivarepercutido::ctafracambiada()
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
	  }
        }
    chequeabotonaceptar();
    if (esauxiliar(ui.CtafralineEdit->text()))
       {
	ui.fratextLabel->setText(descripcioncuenta(ui.CtafralineEdit->text()));
        if (basedatos::instancia()->hay_criterio_caja_aux(ui.CtafralineEdit->text()))
            ui.cajacheckBox->setChecked(true);
       }
    else ui.fratextLabel->clear();
}

void ivarepercutido::ctafrafinedicion()
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


void ivarepercutido::basecambiada()
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
     
     ui.CuotaTOTlineEdit->setText(formateanumero(cuota+bixre,comadecimal,decimales));

  chequeabotonaceptar();
}

void ivarepercutido::basefinedicion()
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


void ivarepercutido::botonaceptar()
{
    bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;
     // double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
     // if (!bien) tipo=0;
     double cuota = convapunto(ui.CuotalineEdit->text()).toDouble(& bien);
     if (!bien) cuota=0;
     
     double cuotare = convapunto(ui.CuotaRElineEdit->text()).toDouble(& bien);
     if (!bien) cuotare=0;

     double cuotatot = convapunto(ui.CuotaTOTlineEdit->text()).toDouble(& bien);
     if (!bien) cuotatot=0;

     if (cuota+cuotare-cuotatot>=0.005 || cuota+cuotare-cuotatot<=-0.005)
        {
	 QMessageBox::warning( this, tr("ERROR"),
               tr("La cuota es incorrecta"));
	 return;
        }
	 
     if (bi<0.0001 && bi>-0.0001) 
            {
              //QMessageBox::warning( this, tr("ERROR"),
              // tr("La base imponible no puede ser nula"));
              //return;
            }
     
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
    
    if (!escuentadeivarepercutido(ui.CtaivalineEdit->text()))
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

void ivarepercutido::recuperadatos(QString *cuentabase, QString *baseimponible, QString *qclaveiva,
                                    QString *qtipoiva, QString *qtipore, QString *cuentaiva,
                                    QString *cuotaiva, QString *ctafra, QDate *qfechafra,
                                    QString *soportado, QString *qrectif,
                                  QDate *fechaop, QString *claveop,
                                  QString *bicoste,
                                    QString *frectif, QString *numfact,
                                    QString *facini, QString *facfinal, QString *nombre,
                                    QString *cif, QString *cajaiva, QString *arrto_ret, QString *arrto_sin_ret)
{

*cuentabase=ui.CtabaselineEdit->text();
*baseimponible=ui.baselineEdit->text();
if (baseimponible->isEmpty()) *baseimponible="0";
QString cadenaiva=ui.ClaveivacomboBox->currentText();
QString extracto=cadenaiva.section("//",0,0);
*qclaveiva=extracto.remove(' ');
*qtipoiva=ui.TipolineEdit->text();
*qtipore=ui.RElineEdit->text();
*cuentaiva=ui.CtaivalineEdit->text();
*cuotaiva=ui.CuotaTOTlineEdit->text();
*ctafra=ui.CtafralineEdit->text();
*qfechafra=ui.FechafradateEdit->date();
*soportado="0";
if (ui.rectificativagroupBox->isChecked()) *qrectif="1"; else *qrectif="0";
*fechaop=ui.fechaoperaciondateEdit->date();
QString cadenaclave=ui.clavecomboBox->currentText();
extracto=cadenaclave.section(' ',0,0);
*claveop=extracto.remove(' ');
*bicoste=ui.bicostelineEdit->text();
if (ui.rectificativagroupBox->isChecked()) {
    *frectif=ui.tipo_rectificativa_comboBox->currentText().left(2);
    if (!ui.rectificada_lineEdit->text().isEmpty())
       {*frectif+=":"; *frectif+=ui.rectificada_lineEdit->text();}
}
   else *frectif="";
*numfact=ui.numfacturaslineEdit->text();
*facini=ui.iniciallineEdit->text();
*facfinal=ui.finallineEdit->text();
*nombre=ui.nombrelineEdit->text();
*cif=ui.ciflineEdit->text();
*cajaiva=ui.cajacheckBox->isChecked() ? "1" : "";
*arrto_ret=ui.arr_ret_checkBox->isChecked() ? "1" : "";
*arrto_sin_ret=ui.arr_sin_ret_checkBox->isChecked() ? "1" : "";

}


void ivarepercutido::chequeabotonaceptar()
{
    bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;
     // double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
     // if (!bien) tipo=0;
     double cuota = convapunto(ui.CuotalineEdit->text()).toDouble(& bien);
     if (!bien) cuota=0;

     double cuotare = convapunto(ui.CuotaRElineEdit->text()).toDouble(& bien);
     if (!bien) cuotare=0;

     double cuotatot = convapunto(ui.CuotaTOTlineEdit->text()).toDouble(& bien);
     if (!bien) cuotatot=0;
     if (!escuentadeivarepercutido(ui.CtaivalineEdit->text()))
        { ui.AceptarButton->setEnabled(false); return; }

     if (cuota+cuotare-cuotatot>=0.005 || cuota+cuotare-cuotatot<=-0.005)
        { ui.AceptarButton->setEnabled(false); return; }

     if (bi<0.0001 && bi>-0.0001)
        { /*ui.AceptarButton->setEnabled(false); return;*/ }

     if (!esauxiliar(ui.CtabaselineEdit->text())) 
     { 
        if (ui.CtabaselineEdit->text().length()!=0)
       { ui.AceptarButton->setEnabled(false); return; }
        if (baseoblig && ui.CtabaselineEdit->text().length()==0)
       { ui.AceptarButton->setEnabled(false); return; }
     }

    if (!esauxiliar(ui.CtaivalineEdit->text())) 
        { ui.AceptarButton->setEnabled(false); return; }
    
    if (!escuentadeivarepercutido(ui.CtaivalineEdit->text()))
        { ui.AceptarButton->setEnabled(false); return; }
    
    if (!esauxiliar(ui.CtafralineEdit->text())) 
        { ui.AceptarButton->setEnabled(false); return; }
    
    ui.AceptarButton->setEnabled(true);

}


void ivarepercutido::cuotacambiada()
{
  bool bien=false;
  double cuota = convapunto(ui.CuotaTOTlineEdit->text()).toDouble(& bien);
  ui.CuotaTOTlineEdit->setText(formateanumero(cuota,comadecimal,decimales));
  chequeabotonaceptar();

}

void ivarepercutido::pasadatos( QString cuentabase, QString baseimponible, QString qcuentaiva,
                                QString cuotaiva, QString ctafra, QDate qfechafra,
                                QString qrectificativa )
{
 ui.CtabaselineEdit->setText(cuentabase);
 if (cuentabase.length()>0) ui.basetextLabel->setText(descripcioncuenta(cuentabase));
 ui.CtaivalineEdit->setText(qcuentaiva);
 if (qcuentaiva.length()>0) ui.ivatextLabel->setText(descripcioncuenta(qcuentaiva));
 ui.CuotaTOTlineEdit->setText(cuotaiva);
 ui.CtafralineEdit->setText(ctafra);
 if (ctafra.length()>0) ui.fratextLabel->setText(descripcioncuenta(ctafra));
 ui.FechafradateEdit->setDate(qfechafra);
 ui.baselineEdit->setText(baseimponible);
 comboivacambiado();
 if (qrectificativa=="1") ui.rectificativagroupBox->setChecked(true);
}

void ivarepercutido::pasadatos_fin( QString ctafra, QString baseimponible, QString qcuentaiva,
                                    QDate qfechafra, QString cta_base_iva)
{
 ui.CtafralineEdit->setText(ctafra);
 if (ctafra.length()>0) ui.fratextLabel->setText(descripcioncuenta(ctafra));
 ui.CtaivalineEdit->setText(qcuentaiva);
 if (qcuentaiva.length()>0) ui.ivatextLabel->setText(descripcioncuenta(qcuentaiva));
 ui.FechafradateEdit->setDate(qfechafra);
 ui.baselineEdit->setText(baseimponible);
 ui.CtabaselineEdit->setText(cta_base_iva);
 comboivacambiado();
}



void ivarepercutido::pasadatos2(QString cuentabase, QString baseimponible, QString qclaveiva,
                                 QString qtipoiva, QString qtipore, QString cuentaiva,
                                 QString cuotaiva, QString ctafra, QDate qfechafra, QString qrectif,
                               QDate fechaop, QString claveop,
                               QString bicoste,
                               QString frectif, QString numfact,
                               QString facini, QString facfinal,
                               QString nombre, QString cif, QString cajaiva, QString arrto_ret, QString arrto_sin_ret)
{
ui.CtabaselineEdit->setText(cuentabase);
 if (cuentabase.length()>0) ui.basetextLabel->setText(descripcioncuenta(cuentabase));
ui.baselineEdit->setText(convacoma(baseimponible));

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

ui.TipolineEdit->setText(convacoma(qtipoiva));
ui.RElineEdit->setText(convacoma(qtipore));

// comboivacambiado();
int ndecimales= decimales ? 2 : 0;
     bool bien;
    double bi = convapunto(ui.baselineEdit->text()).toDouble(& bien);
     if (!bien) bi=0;
     double tipo = convapunto(ui.TipolineEdit->text()).toDouble(& bien)/100;
     if (!bien) tipo=0;
     ui.CuotalineEdit->setText( formateanumero(redond(bi*tipo,ndecimales),comadecimal,decimales));
     double re = convapunto(ui.RElineEdit->text()).toDouble( & bien)/100;
     if (!bien) re=0;
     ui.CuotaRElineEdit->setText(formateanumero(redond(bi*re,ndecimales),comadecimal,decimales));

ui.CtaivalineEdit->setText(cuentaiva);

if (cuentaiva.length()>0) ui.ivatextLabel->setText(descripcioncuenta(cuentaiva));
double cuotatot=convapunto(cuotaiva).toDouble(&bien);
if (!bien) cuotatot=0;
ui.CuotaTOTlineEdit->setText(formateanumero(cuotatot,comadecimal,decimales));

ui.CtafralineEdit->setText(ctafra);
if (ctafra.length()>0) ui.fratextLabel->setText(descripcioncuenta(ctafra));
ui.FechafradateEdit->setDate(qfechafra);
if (qrectif=="1") ui.rectificativagroupBox->setChecked(true);
   else ui.rectificativagroupBox->setChecked(false);

ui.fechaoperaciondateEdit->setDate(fechaop);
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

  if (qrectif=="1") {
      ui.rectificativagroupBox->setChecked(true);
      for (int veces=0; veces<ui.tipo_rectificativa_comboBox->count(); veces++)
        {
         if (ui.tipo_rectificativa_comboBox->itemText(veces).left(2)==frectif.left(2))
           ui.tipo_rectificativa_comboBox->setCurrentIndex(veces);
        }
      if (frectif.length()>3)
        ui.rectificada_lineEdit->setText(frectif.mid(3));
    }
    else ui.rectificativagroupBox->setChecked(false);

 ui.numfacturaslineEdit->setText(numfact);
 ui.iniciallineEdit->setText(facini);
 ui.finallineEdit->setText(facfinal);
 ui.nombrelineEdit->setText(nombre);
 ui.ciflineEdit->setText(cif);
 ui.cajacheckBox->setChecked(cajaiva=="1");

 ui.arr_ret_checkBox->setChecked(arrto_ret=="1");
 ui.arr_sin_ret_checkBox->setChecked(arrto_sin_ret=="1");

}


void ivarepercutido::buscactabase()
{
    QString cadena2;    
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtabaselineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cadena2.length()>0) ui.CtabaselineEdit->setText(cadena2);
       else ui.CtabaselineEdit->clear();
    delete(labusqueda);
}


void ivarepercutido::buscactaiva()
{
    QString cadena2;    
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtaivalineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cadena2.length()>0) ui.CtaivalineEdit->setText(cadena2);
       else ui.CtaivalineEdit->setText("");
    delete(labusqueda);
}


void ivarepercutido::buscactafra()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.CtafralineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cadena2.length()>0) ui.CtafralineEdit->setText(cadena2);
       else ui.CtafralineEdit->setText("");
    delete(labusqueda);
}




void ivarepercutido::hazbaseoblig()
{
  baseoblig=true;
  QDate fecha;
  ui.FechafradateEdit->setDate(fecha.currentDate());
  comboivacambiado();
}

void ivarepercutido::fococtafra()
{
    ui.CtafralineEdit->setFocus();
}

void ivarepercutido::fococtabi()
{
    ui.CtabaselineEdit->setFocus();
}

void ivarepercutido::cargacombooperaciones()
{
  QStringList lista=basedatos::instancia()->listaoperaciones_expedidas();
  ui.clavecomboBox->addItems (lista);
}

void ivarepercutido::fechafracambiada()
{
 ui.fechaoperaciondateEdit->setDate(ui.FechafradateEdit->date());
}


void ivarepercutido::bicostecambiada()
{
  if (comadecimal)
          ui.bicostelineEdit->setText( convacoma(ui.bicostelineEdit->text()));
}

void ivarepercutido::pasatipoiva(QString tipo)
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

void ivarepercutido::modoconsulta()
{
    ui.baselineEdit->setReadOnly(true);
    ui.bicostelineEdit->setReadOnly(true);
    ui.buscabasepushButton->setEnabled(false);
    ui.buscafrapushButton->setEnabled(false);
    ui.buscaivapushButton->setEnabled(false);
    ui.clavecomboBox->setEnabled(false);
    ui.ClaveivacomboBox->setEnabled(false);
    ui.CtabaselineEdit->setReadOnly(true);
    ui.CtafralineEdit->setReadOnly(true);
    ui.CtaivalineEdit->setReadOnly(true);
    ui.CtafralineEdit->setReadOnly(true);
    ui.CtaivalineEdit->setReadOnly(true);
    ui.CuotalineEdit->setReadOnly(true);
    ui.CuotaRElineEdit->setReadOnly(true);
    ui.CuotaTOTlineEdit->setReadOnly(true);
    ui.FechafradateEdit->setReadOnly(true);
    ui.fechaoperaciondateEdit->setReadOnly(true);
    ui.finallineEdit->setReadOnly(true);
    ui.iniciallineEdit->setReadOnly(true);
    ui.numfacturaslineEdit->setReadOnly(true);
    ui.tipo_rectificativa_comboBox->setEnabled(false);
    ui.RElineEdit->setReadOnly(true);
    ui.TipolineEdit->setReadOnly(true);
    ui.nombrelineEdit->setReadOnly(true);
    ui.ciflineEdit->setReadOnly(true);
    ui.cajacheckBox->setEnabled(false);
    ui.arr_ret_checkBox->setEnabled(false);
    ui.arr_sin_ret_checkBox->setEnabled(false);
}
