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

#include "ed_cta_lote.h"
#include "basedatos.h"
#include "funciones.h"
#include "buscasubcuenta.h"
#include "busca_externo.h"
#include "externos.h"

ed_cta_lote::ed_cta_lote(bool qcomadecimal) : QDialog() {
    ui.setupUi(this);

// cargamos datos de la tabla configuración
comadecimal=qcomadecimal;
QString cuenta;
// double tipo;
ui.cuentalineEdit->setText(cuenta);
// ui.tipolineEdit->setText(formateanumero(tipo,comadecimal,true));

connect(ui.ctapushButton,SIGNAL(clicked()),this,SLOT(buscacta()));
connect(ui.cuentalineEdit,SIGNAL(editingFinished()),this,SLOT(ctaexpandepunto()));
connect(ui.tipolineEdit,SIGNAL(editingFinished()),SLOT(fin_edic_cuota()));
connect(ui.importelineEdit,SIGNAL(editingFinished()),SLOT(fin_edic_importe()));

connect(ui.busca_externo_pushButton,SIGNAL(clicked(bool)),SLOT(buscar_externo()));
connect(ui.externo_lineEdit,SIGNAL(textChanged(QString)),SLOT(campo_externo_cambiado()));
connect(ui.quita_externo_pushButton,SIGNAL(clicked(bool)),SLOT(borra_externo()));
connect(ui.datos_externo_pushButton,SIGNAL(clicked(bool)),SLOT(ver_externo()));

if (!basedatos::instancia()->hay_externos())
   {
    ui.externo_label->hide();
    ui.externo_lineEdit->hide();
    ui.descrip_externo_lineEdit->hide();
    ui.busca_externo_pushButton->hide();
    ui.datos_externo_pushButton->hide();
    ui.quita_externo_pushButton->hide();
   }

}



void ed_cta_lote::buscacta()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.cuentalineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2))
      ui.cuentalineEdit->setText(cadena2);
       else ui.cuentalineEdit->setText("");
    delete labusqueda;
    ctaexpandepunto();
}


void ed_cta_lote::ctaexpandepunto()
{
  ui.cuentalineEdit->setText(expandepunto(ui.cuentalineEdit->text(),anchocuentas()));
  QString descrip;
  if (existecodigoplan(ui.cuentalineEdit->text(),&descrip) &&
      esauxiliar(ui.cuentalineEdit->text()))
     {
      ui.descrip_label->setText(descrip);
     }
     else
        {
         ui.descrip_label->setText("- -");

        }

  // int existecodigoplan(QString cadena,QString *qdescrip);
  // esauxiliar(codigo)
}


void ed_cta_lote::fin_edic_cuota()
{
    if (comadecimal)
    ui.tipolineEdit->setText(convacoma(ui.tipolineEdit->text()));
    else
        ui.tipolineEdit->setText(convapunto(ui.tipolineEdit->text()));
}

void ed_cta_lote::fin_edic_importe()
{
    if (comadecimal)
    ui.importelineEdit->setText(convacoma(ui.importelineEdit->text()));
    else
        ui.importelineEdit->setText(convapunto(ui.importelineEdit->text()));
}


QString ed_cta_lote::cuenta()
{
  return ui.cuentalineEdit->text();
}

QString ed_cta_lote::cuota()
{
  return ui.tipolineEdit->text();
}

QString ed_cta_lote::importe()
{
  return ui.importelineEdit->text();
}

QString ed_cta_lote::descrip()
{
  return ui.descrip_label->text();
}

QString ed_cta_lote::externo()
{
  return ui.externo_lineEdit->text();
}

void ed_cta_lote::con_cuota(bool cuota)
{
    if (cuota)
       {
        ui.importelineEdit->hide();
        ui.importe_label->hide();
        ui.cuota_label->show();
        ui.tipolineEdit->show();
       }
      else
          {
            ui.importelineEdit->show();
            ui.importe_label->show();
            ui.cuota_label->hide();
            ui.tipolineEdit->hide();
          }
}


void ed_cta_lote::pasa_edicion(QString cuenta, QString importe, QString cuota, QString externo)
{
  ui.externo_lineEdit->setReadOnly(true);
  ui.busca_externo_pushButton->hide();
  ui.quita_externo_pushButton->hide();
  ui.externo_lineEdit->setText(externo);
  ui.ctapushButton->hide();
  ui.cuentalineEdit->setReadOnly(true);
  if (!ui.tipolineEdit->isHidden()) ui.tipolineEdit->setFocus();
    else ui.importelineEdit->setFocus();
  ui.cuentalineEdit->setText(cuenta);
  ui.importelineEdit->setText(importe);
  ui.tipolineEdit->setText(cuota);
  ctaexpandepunto();
}


void ed_cta_lote::buscar_externo()
{
   busca_externo *b = new busca_externo();
   int rdo=b->exec();
   if (rdo==QDialog::Accepted)
     {
       ui.externo_lineEdit->setText(b->codigo_elec());
     }
     else ui.externo_lineEdit->clear();
   delete(b);
}

void ed_cta_lote::campo_externo_cambiado()
{
 QString codigo=ui.externo_lineEdit->text();
 if (basedatos::instancia()->existe_externo(codigo))
   {
     ui.descrip_externo_lineEdit->setText(basedatos::instancia()->razon_externo(codigo));
     ui.cuentalineEdit->setText(basedatos::instancia()->cuenta_externo(codigo));
   }
   else ui.descrip_externo_lineEdit->clear();
  ctaexpandepunto();
}

void ed_cta_lote::borra_externo()
{
  ui.externo_lineEdit->clear();
}

void ed_cta_lote::ver_externo()
{
    externos *e = new externos();
    e->pasa_codigo(ui.externo_lineEdit->text());
    e->exec();
    delete(e);
}
