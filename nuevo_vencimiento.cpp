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

#include "nuevo_vencimiento.h"
#include "buscasubcuenta.h"
#include "aux_express.h"
#include "basedatos.h"
#include <QMessageBox>
#include "busca_externo.h"

nuevo_vencimiento::nuevo_vencimiento() : QDialog() {
    ui.setupUi(this);

    comadecimal=haycomadecimal(); decimales=haydecimales();

   connect(ui.ordenantepushButton,SIGNAL(clicked()),this,SLOT(buscactaordenante()));
   connect(ui.OrdenantelineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctaordenantecambiada()));
   connect(ui.OrdenantelineEdit,SIGNAL(editingFinished ()),this,SLOT(expandepuntoordenante()));
   connect(ui.tesoreriapushButton,SIGNAL(clicked()),this,SLOT(buscactatesoreria()));
   connect(ui.TesorerialineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctatesoreriacambiada()));
   connect(ui.TesorerialineEdit,SIGNAL(editingFinished ()),this,SLOT(expandepuntotesoreria()));
   connect(ui.importelineEdit,SIGNAL(editingFinished ()),this,SLOT(importecambiado()));
   connect(ui.externo_pushButton,SIGNAL(clicked(bool)),this,SLOT(buscaexterno()));
   connect(ui.externo_lineEdit,SIGNAL(textChanged(QString)),this,SLOT(externo_cambiado()));

 }

void nuevo_vencimiento::pasaInfo(QString cuentaordenante, QDate fechaoperacion, QString importe, QString externo)
{
   // QMessageBox::warning( this, tr("Tabla de apuntes"),importe);

   double elimporte=convapunto(importe).toDouble();
   QString importe2; importe2=formateanumero(elimporte,comadecimal,decimales);
   //QString descrip;
   QString importedef;
   // QMessageBox::warning( this, tr("Tabla de apuntes"),importe2);
   if (importe2.contains('-')) {
       ui.ObligacionradioButton->setChecked(true);
       importedef=importe2.remove('-');
   } else {
       ui.DerechoradioButton->setChecked(true);
       importedef=importe2;
   }
   
   QString tesor_defecto=basedatos::instancia()->select_cuenta_tesoreria_configuracion();
  //existecodigoplan(cuentaordenante,&descrip);
  ui.OrdenantelineEdit->setText(cuentaordenante);
  //ui.OrdenanteNombrelineEdit->setText(descrip);
  ui.operaciondateEdit->setDate(fechaoperacion);
  ui.importelineEdit->setText(importedef);
  ui.vencimientodateEdit->setDate(fechaoperacion);
  ui.TesorerialineEdit->setText(tesor_defecto);
  ui.externo_lineEdit->setText(externo);
  if (tesor_defecto.isEmpty())
     ui.TesorerialineEdit->setFocus();
    else
        ui.importelineEdit->setFocus();
}


void nuevo_vencimiento::buscactaordenante()
{
    QString cadena2;
    QString inicial;
    // QString descrip;
    inicial=ui.OrdenantelineEdit->text();

    buscasubcuenta *labusqueda=new buscasubcuenta(inicial);
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
       {
	ui.OrdenantelineEdit->setText(cadena2);
        // if (cod_longitud_variable()) ui.OrdenanteNombrelineEdit->setText(descripcioncuenta(cadena2));
       }
    else { ui.OrdenantelineEdit->setText("");  ui.OrdenanteNombrelineEdit->setText(""); }
    delete labusqueda;
}


void nuevo_vencimiento::ctaordenantecambiada()
{
  QString descrip;
  if (ui.OrdenantelineEdit->text().length()==anchocuentas() && !cod_longitud_variable())
    {
       existecodigoplan(ui.OrdenantelineEdit->text(),&descrip);
       ui.OrdenanteNombrelineEdit->setText(descrip);
    } else ui.OrdenanteNombrelineEdit->setText("");
  if (cod_longitud_variable() && existesubcuenta(ui.OrdenantelineEdit->text()))
     ui.OrdenanteNombrelineEdit->setText(descripcioncuenta(ui.OrdenantelineEdit->text()));
  ui.domiciliablecheckBox->setChecked(basedatos::instancia()->cuenta_domiciliable(ui.OrdenantelineEdit->text()));
  chequeaintro();
}


void nuevo_vencimiento::expandepuntoordenante()
{
 QString resultante;
 QString cadena;
 
 resultante=expandepunto(ui.OrdenantelineEdit->text(),anchocuentas());
 if (resultante!=ui.OrdenantelineEdit->text()) ui.OrdenantelineEdit->setText(resultante);
 if ((ui.OrdenantelineEdit->text().length()==anchocuentas() &&
     !existecodigoplan(ui.OrdenantelineEdit->text(),&cadena) && !cod_longitud_variable())
     || (cod_longitud_variable() && ui.OrdenantelineEdit->text().length()>3 &&
        !existesubcuenta(ui.OrdenantelineEdit->text()))) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    int x=QMessageBox::question(this,tr("Vencimientos"),
					   tr("Esa cuenta no existe, ¿ Desea añadirla ?"),
					   tr("&Sí"),
					   tr("&No"),
                       QString(),
					   0,1);
	    if (x==0) {
                       aux_express *lasubcuenta2=new aux_express();
		       lasubcuenta2->pasacodigo(ui.OrdenantelineEdit->text());
		       lasubcuenta2->exec();
	               delete lasubcuenta2;
	               if (!existesubcuenta(ui.OrdenantelineEdit->text()))
			   ui.OrdenantelineEdit->setText("");
		       else  ui.OrdenanteNombrelineEdit->setText(cadena);

		     }
	    else ui.OrdenantelineEdit->setText("");
	}
 ui.domiciliablecheckBox->setChecked(basedatos::instancia()->cuenta_domiciliable(ui.OrdenantelineEdit->text()));
}


void nuevo_vencimiento::buscactatesoreria()
{
    QString cadena2;
    QString inicial;
    // QString descrip;
    inicial=ui.TesorerialineEdit->text();

    buscasubcuenta *labusqueda=new buscasubcuenta(inicial);
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
       {
	ui.TesorerialineEdit->setText(cadena2);
       }
    else { ui.TesorerialineEdit->setText("");  ui.TesoreriaNombrelineEdit->setText(""); }
    delete labusqueda;
}


void nuevo_vencimiento::ctatesoreriacambiada()
{
  QString descrip;
  if (ui.TesorerialineEdit->text().length()==anchocuentas() && !cod_longitud_variable())
    {
       existecodigoplan(ui.TesorerialineEdit->text(),&descrip);
       ui.TesoreriaNombrelineEdit->setText(descrip);
  } else ui.TesoreriaNombrelineEdit->setText("");
  if (cod_longitud_variable() && existesubcuenta(ui.TesorerialineEdit->text()))
     ui.TesoreriaNombrelineEdit->setText(descripcioncuenta(ui.TesorerialineEdit->text()));
  chequeaintro();
}


void nuevo_vencimiento::expandepuntotesoreria()
{
 QString resultante;
 QString cadena;
 
 resultante=expandepunto(ui.TesorerialineEdit->text(),anchocuentas());
 if (resultante!=ui.TesorerialineEdit->text()) ui.TesorerialineEdit->setText(resultante);
 
 if ((ui.TesorerialineEdit->text().length()==anchocuentas() && 
     !existecodigoplan(ui.TesorerialineEdit->text(),&cadena) && !cod_longitud_variable())
     || (cod_longitud_variable() && ui.TesorerialineEdit->text().length()>3 &&
        !existesubcuenta(ui.TesorerialineEdit->text()))) 
	  {
	 // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
	    int x=QMessageBox::question(this,tr("Vencimientos"),
					   tr("Esa cuenta no existe, ¿ Desea añadirla ?"),
					   tr("&Sí"),
					   tr("&No"),
                       QString(),
					   0,1);
	    if (x==0) {
                        aux_express *lasubcuenta2=new aux_express();
		        lasubcuenta2->pasacodigo(ui.TesorerialineEdit->text());
		        lasubcuenta2->exec();
	                delete lasubcuenta2;
                        if (!existesubcuenta(ui.TesorerialineEdit->text()))
			   ui.TesorerialineEdit->setText("");                        
                         else
                             {
                              if (existecodigoplan(ui.TesorerialineEdit->text(),&cadena))
                                 ui.TesoreriaNombrelineEdit->setText(cadena);
                             }
		      }
	    else ui.TesorerialineEdit->setText("");
	}
}


void nuevo_vencimiento::chequeaintro()
{
    if (!existesubcuenta(ui.OrdenantelineEdit->text()))
        { ui.AceptarpushButton->setEnabled(false); return; }
    else 
         if (!existesubcuenta(ui.TesorerialineEdit->text())) 
          {ui.AceptarpushButton->setEnabled(false); return; }
	     else 
	       {
		 if (convapunto(ui.importelineEdit->text()).toDouble()<0.00001)
            {ui.AceptarpushButton->setEnabled(false); return; }
	       }
    ui.AceptarpushButton->setEnabled(true);
}


void nuevo_vencimiento::importecambiado()
{
 ui.importelineEdit->setText(formateanumero(convapunto(ui.importelineEdit->text()).toDouble(),
                  comadecimal,decimales));
 chequeaintro();
}



void nuevo_vencimiento::guardanuevovencimiento( QString pase )
{
    QSqlDatabase::database().transaction();

    basedatos::instancia()->bloquea_para_nuevo_vencimiento();

    qlonglong vnum = basedatos::instancia()->proximovencimiento();

    QString cadnum; cadnum.setNum(vnum+1);
    // ***********************************************************************
    QString cadnumvto; cadnumvto.setNum(vnum);

    basedatos::instancia()->updateConfiguracionprox_vencimiento(cadnum);

    basedatos::instancia()->insertVencimientos_plantilla ( cadnumvto ,
                                                 ui.OrdenantelineEdit->text().left(-1) ,
                                                 ui.operaciondateEdit->date() ,
                                                 convapunto(ui.importelineEdit->text()) ,
                                                 ui.TesorerialineEdit->text() ,
                                                 ui.vencimientodateEdit->date() ,
                                                 ui.DerechoradioButton->isChecked() ,
                                                 true ,
                                                 false ,
                                                 pase,
                                                 "0",
                                                 ui.conceptolineEdit->text(),
                                                 ui.domiciliablecheckBox->isChecked(),
                                                 ui.externo_lineEdit->text());

    basedatos::instancia()->update_cuenta_tesoreria_config(ui.TesorerialineEdit->text());
   basedatos::instancia()->desbloquea_y_commit();
}




void nuevo_vencimiento::pasaInfo2( QString ctatesoreria, QDate qvencimiento,
                                   QString qconcepto, bool domiciliable, QString externo )
{
  ui.TesorerialineEdit->setText(ctatesoreria);
  ui.vencimientodateEdit->setDate(qvencimiento);
  ui.conceptolineEdit->setText(qconcepto);
  ui.domiciliablecheckBox->setChecked(domiciliable);
  ui.externo_lineEdit->setText(externo);
  setWindowTitle("VENCIMIENTO");
}


void nuevo_vencimiento::cambiavencimiento( QString cadnumvto )
{
    basedatos::instancia()->updateVencimientos7num( ui.OrdenantelineEdit->text() ,
                                                    ui.operaciondateEdit->date() ,
                                                    convapunto(ui.importelineEdit->text()) ,
                                                    ui.TesorerialineEdit->text() ,
                                                    ui.vencimientodateEdit->date() ,
                                                    ui.conceptolineEdit->text() ,
                                                    cadnumvto,
                                                    ui.domiciliablecheckBox->isChecked(),
                                                    ui.externo_lineEdit->text());

    basedatos::instancia()->update_cuenta_tesoreria_config(ui.TesorerialineEdit->text());


}

void nuevo_vencimiento::buscaexterno() {
    busca_externo *b = new busca_externo();
    int resultado=b->exec();
    if (resultado==QDialog::Accepted)
       {
        ui.externo_lineEdit->setText(b->codigo_elec());
        //ui.descrip_externo_lineEdit->setText(b->nombre_elec());

       }
    delete(b);

}

void nuevo_vencimiento::externo_cambiado() {
    ui.ExternoNombre_lineEdit->setText(basedatos::instancia()->razon_externo(ui.externo_lineEdit->text()));
}
