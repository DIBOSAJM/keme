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

#include "retencion.h"
#include <QSqlQuery>
#include "funciones.h"
#include "buscasubcuenta.h"
#include "aux_express.h"
#include "basedatos.h"
#include <QMessageBox>

retencion::retencion(bool concomadecimal, bool condecimales) : QDialog() {
    ui.setupUi(this);

comadecimal=concomadecimal;
decimales=condecimales;

// QStringList basedatos::listacodprovincias()
ui.provinciacomboBox->addItems(basedatos::instancia()->listacodprovincias());
QString pr_defecto=basedatos::instancia()->provincia_def();
for (int veces=0; veces<ui.provinciacomboBox->count(); veces++)
    {
     if (ui.provinciacomboBox->itemText(veces).left(2)==pr_defecto.left(2))
        {
         ui.provinciacomboBox->setCurrentIndex(veces);
         break;
        }
    }

ui.clavecomboBox->addItems(basedatos::instancia()->listaoperaciones_ret());
connect(ui.buscaretenidopushButton,SIGNAL(clicked()),this,SLOT(buscactaretenido()));

connect(ui.retenidolineEdit,SIGNAL(textChanged(QString)),this,SLOT(ctaretenidocambiada()));

connect(ui.retenidolineEdit,SIGNAL(editingFinished()),this,SLOT(ctaretenidofinedicion()));

connect(ui.baselineEdit,SIGNAL(textChanged(QString)),this,SLOT(base_o_tipo_cambiado()));
connect(ui.tiporetlineEdit,SIGNAL(textChanged(QString)),this,SLOT(base_o_tipo_cambiado()));

connect(ui.dinerariosgroupBox,SIGNAL(toggled(bool)),this,SLOT(dinerario_cambiado()));
connect(ui.especiegroupBox,SIGNAL(toggled(bool)),this,SLOT(especie_cambiado()));

connect(ui.ing_a_cta_replineEdit,SIGNAL(textChanged(QString)),this,
        SLOT(ing_a_cta_replineEdit_cambiado()));

connect(ui.aceptarpushButton,SIGNAL(clicked()),this,SLOT(aceptar_pulsado()));


ui.retenidolineEdit->setFocus();
}

void retencion::aceptar_pulsado()
{
  //if (!cod_longitud_variable())
  //    ui.retenidolineEdit->setText(expandepunto(ui.retenidolineEdit->text(),anchocuentas()));
  accept();
}

void retencion::pasadatos(QString qcuentaretencion,
               QString qbase_percepciones,
               QString retencion)
{
  qbase_percepciones=convapunto(qbase_percepciones);
  retencion=convapunto(retencion);
  ui.cta_retlineEdit->setText(qcuentaretencion);
  ui.cta_retlabel->setText(descripcioncuenta(qcuentaretencion));
  ui.baselineEdit->setText(comadecimal ? convacoma(qbase_percepciones) : qbase_percepciones);
  double tipo_ret_cta;
  tipo_ret_cta=basedatos::instancia()->tipo_ret_cta(qcuentaretencion);
  if (tipo_ret_cta<0.001 && qbase_percepciones.toDouble()>0.001)
      tipo_ret_cta=CutAndRoundNumberToNDecimals (
              retencion.toDouble()/qbase_percepciones.toDouble()*100,2);
  QString cadtiporet; cadtiporet.setNum(tipo_ret_cta,'f',2);
  ui.tiporetlineEdit->setText(comadecimal ? convacoma(cadtiporet) : cadtiporet);
  ui.retlineEdit->setText(comadecimal ? convacoma(retencion) : retencion);

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


void retencion::recuperadatos(QString *cta_retenido, bool *ret_arrendamiento,
                              QString *clave, QString *base_percepciones,
                              QString *tipo_ret, QString *retencion,
                              QString *ing_cta, QString *ing_cta_repercutido,
                              QString *nombre_ret, QString *cif_ret, QString *provincia)
{
 *cta_retenido=ui.retenidolineEdit->text();
 *ret_arrendamiento=ui.arrendamientocheckBox->isChecked();
 *clave=ui.clavecomboBox->currentText().left(1);
 *base_percepciones=ui.baselineEdit->text();
 *tipo_ret=ui.tiporetlineEdit->text();
 *retencion=ui.retlineEdit->text();
 *ing_cta=ui.ing_a_ctalineEdit->text();
 *ing_cta_repercutido=ui.ing_a_cta_replineEdit->text();
 *nombre_ret=ui.nombrelineEdit->text();
 *cif_ret=ui.ciflineEdit->text();
 *provincia=ui.provinciacomboBox->currentText().left(2);
}



void retencion::pasadatos_all(QString qcuentaretencion,
                              QString cta_retenido, bool ret_arrendamiento,
                              QString clave, QString base_percepciones,
                              QString tipo_ret, QString retencion,
                              QString ing_cta, QString ing_cta_repercutido,
                              QString nombre_ret, QString cif_ret, QString provincia)
{
    ui.cta_retlineEdit->setText(qcuentaretencion);
    ui.cta_retlabel->setText(descripcioncuenta(qcuentaretencion));
    ui.retenidolineEdit->setText(cta_retenido);
    if (ret_arrendamiento) ui.arrendamientocheckBox->setChecked(true);

    int indice=0;
    while (indice<ui.clavecomboBox->count())
      {
       if (ui.clavecomboBox->itemText(indice).left(1)==clave.left(1)) break;
       indice++;
      }
    if (indice>=ui.clavecomboBox->count()) indice=0;
    ui.clavecomboBox->setCurrentIndex(indice);
    ui.baselineEdit->setText(comadecimal ? convacoma(base_percepciones):
                             convapunto(base_percepciones));
    ui.tiporetlineEdit->setText(comadecimal ? convacoma(tipo_ret):
                                convapunto(tipo_ret));
    ui.retlineEdit->setText(comadecimal ? convacoma(retencion):
                            convapunto(retencion));
    if (!ing_cta.isEmpty() && (convapunto(ing_cta).toDouble()>0.001 || convapunto(ing_cta).toDouble()<-0.001))
      {
        ui.especiegroupBox->setChecked(true);
        ui.ing_a_ctalineEdit->setText(comadecimal ? convacoma(ing_cta):
                                  convapunto(ing_cta));
         ui.ing_a_cta_replineEdit->setText(comadecimal ? convacoma(ing_cta_repercutido):
                                  convapunto(ing_cta_repercutido));
       }
    ui.nombrelineEdit->setText(nombre_ret);
    ui.ciflineEdit->setText(cif_ret);

    indice=0;
    while (indice<ui.provinciacomboBox->count())
      {
        if (ui.provinciacomboBox->itemText(indice).left(2)==provincia.left(2)) break;
        indice++;
      }
    if (indice>=ui.provinciacomboBox->count()) indice=0;
    ui.provinciacomboBox->setCurrentIndex(indice);
}

void retencion::pasadatos2(QString qcuentaretencion,
                              bool ret_arrendamiento,
                              QString clave, QString base_percepciones,
                              QString tipo_ret)
{
    ui.cta_retlineEdit->setText(qcuentaretencion);
    ui.cta_retlabel->setText(descripcioncuenta(qcuentaretencion));
    if (ret_arrendamiento) ui.arrendamientocheckBox->setChecked(true);

    int indice=0;
    while (indice<ui.clavecomboBox->count())
      {
       if (ui.clavecomboBox->itemText(indice).left(1)==clave.left(1)) break;
       indice++;
      }
    if (indice>=ui.clavecomboBox->count()) indice=0;
    ui.clavecomboBox->setCurrentIndex(indice);
    ui.baselineEdit->setText(comadecimal ? convacoma(base_percepciones):
                             convapunto(base_percepciones));
    ui.tiporetlineEdit->setText(comadecimal ? convacoma(tipo_ret):
                                convapunto(tipo_ret));
    base_o_tipo_cambiado();

}


void retencion::buscactaretenido()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.retenidolineEdit->text());
    labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cadena2.length()>0) ui.retenidolineEdit->setText(cadena2);
       else ui.retenidolineEdit->clear();
    delete(labusqueda);
}



void retencion::ctaretenidocambiada()
{
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);

   QString cadena;
    if (ui.retenidolineEdit->text().length()==anchocuentas() && !cod_longitud_variable()) {
        if (!existecodigoplan(ui.retenidolineEdit->text(),&cadena))
          {
         // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
            QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
            if (msgBox.clickedButton() == aceptarButton)
                 {
                   aux_express *lasubcuenta2=new aux_express();
                   lasubcuenta2->pasacodigo(ui.retenidolineEdit->text());
                   lasubcuenta2->exec();
                   delete(lasubcuenta2);
                   if (!existecodigoplan(ui.retenidolineEdit->text(),&cadena))
                        ui.retenidolineEdit->setText("");
                 }
                else
                    ui.retenidolineEdit->setText("");
          }
         // chequeabotonaceptar();
        }
  if (esauxiliar(ui.retenidolineEdit->text()))
     {
       ui.ctaretenidolabel->setText(descripcioncuenta(ui.retenidolineEdit->text()));
     }
    else ui.ctaretenidolabel->setText("");

  cheq_aceptar();
}


void retencion::ctaretenidofinedicion()
{
 if (!cod_longitud_variable()) ui.retenidolineEdit->setText(expandepunto(ui.retenidolineEdit->text(),anchocuentas()));
 QMessageBox msgBox;
 QPushButton *aceptarButton = msgBox.addButton(tr("Sí"), QMessageBox::ActionRole);
 msgBox.addButton(tr("No"), QMessageBox::ActionRole);
 msgBox.setWindowTitle(tr("ADVERTENCIA"));
 msgBox.setIcon(QMessageBox::Warning);
 QString cadena;

       if (ui.retenidolineEdit->text().length()>3 && cod_longitud_variable()) {
        if (!existecodigoplan(ui.retenidolineEdit->text(),&cadena))
          {
         // preguntar si se desea añadir la cuenta, si no o si se cancela lo anterior dejar en blanco
            QString TextoAviso = tr("Esa cuenta no existe, ¿ Desea añadirla ?");
            msgBox.setText(TextoAviso);
            msgBox.exec();
            if (msgBox.clickedButton() == aceptarButton)
                 {
                   aux_express *lasubcuenta2=new aux_express();
                   lasubcuenta2->pasacodigo(ui.retenidolineEdit->text());
                   lasubcuenta2->exec();
                   delete(lasubcuenta2);
                   if (!esauxiliar(ui.retenidolineEdit->text()))
                        ui.retenidolineEdit->setText("");
                 }
                else
                    ui.retenidolineEdit->setText("");
            // chequeabotonaceptar();
          }
        }
    cheq_aceptar();

}


void retencion::base_o_tipo_cambiado()
{
   double retencion;
   retencion=CutAndRoundNumberToNDecimals (convapunto(ui.baselineEdit->text()).toDouble() *
             convapunto(ui.tiporetlineEdit->text()).toDouble()/100,2);
   QString cadret; cadret.setNum(retencion,'f',2);
   if (comadecimal) cadret=convacoma(cadret);
   if (ui.dinerariosgroupBox->isChecked())
    {
     ui.retlineEdit->setText(cadret);
    }
     else
         ui.ing_a_ctalineEdit->setText(cadret);
   if (comadecimal) ui.baselineEdit->setText(convacoma(ui.baselineEdit->text()));
   if (comadecimal) ui.tiporetlineEdit->setText(convacoma(ui.tiporetlineEdit->text()));
   cheq_aceptar();

}

void retencion::dinerario_cambiado()
{
    if (ui.dinerariosgroupBox->isChecked())
    {
      ui.especiegroupBox->setChecked(false);
      ui.ing_a_ctalineEdit->clear();
      ui.ing_a_cta_replineEdit->clear();
      base_o_tipo_cambiado();
    }
    else if (!ui.especiegroupBox->isChecked()) ui.especiegroupBox->setChecked(true);
    cheq_aceptar();
}


void retencion::especie_cambiado()
{
    if (ui.especiegroupBox->isChecked())
    {
      ui.dinerariosgroupBox->setChecked(false);
      ui.retlineEdit->clear();
      base_o_tipo_cambiado();
    }
    else if (!ui.dinerariosgroupBox->isChecked()) ui.dinerariosgroupBox->setChecked(true);
    cheq_aceptar();
}


void retencion::ing_a_cta_replineEdit_cambiado()
{
 ui.ing_a_cta_replineEdit->setText(convacoma(ui.ing_a_cta_replineEdit->text()));
}

void retencion::cheq_aceptar()
{
  bool aceptar=true;
  if (!esauxiliar(ui.retenidolineEdit->text())) aceptar=false;
  if (convapunto(ui.baselineEdit->text()).toDouble()>-0.001 && convapunto(ui.baselineEdit->text()).toDouble()<0.001)
      aceptar=false;
  if (ui.dinerariosgroupBox->isChecked())
     {
      double ret=convapunto(ui.retlineEdit->text()).toDouble();
      double producto=convapunto(ui.baselineEdit->text()).toDouble()*convapunto(ui.tiporetlineEdit->text()).toDouble()/100;
      if (ret-producto<-0.01 || ret-producto>0.01) aceptar=false;
     }
  if (ui.especiegroupBox->isChecked())
     {
      double ret=convapunto(ui.ing_a_ctalineEdit->text()).toDouble();
      double producto=convapunto(ui.baselineEdit->text()).toDouble()*convapunto(ui.tiporetlineEdit->text()).toDouble()/100;
      if (ret-producto<-0.01 || ret-producto>0.01) aceptar=false;
     }
  ui.aceptarpushButton->setEnabled(aceptar);
}


void retencion::modoconsulta()
{
   ui.arrendamientocheckBox->setEnabled(false);
   ui.baselineEdit->setReadOnly(true);
   ui.ciflineEdit->setReadOnly(true);
   ui.clavecomboBox->setEnabled(false);
   ui.cta_retlineEdit->setReadOnly(true);
   ui.dinerariosgroupBox->setEnabled(false);
   ui.especiegroupBox->setEnabled(false);
   ui.ing_a_ctalineEdit->setReadOnly(true);
   ui.ing_a_cta_replineEdit->setReadOnly(true);
   ui.nombrelineEdit->setReadOnly(true);
   ui.provinciacomboBox->setEnabled(false);
   ui.retenidolineEdit->setReadOnly(true);
   ui.retlineEdit->setReadOnly(true);
   ui.tiporetlineEdit->setReadOnly(true);

}
