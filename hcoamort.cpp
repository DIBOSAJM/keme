/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2; aplicación para llevar contabilidades

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

#include "hcoamort.h"
#include "funciones.h"
#include "basedatos.h"
#include "buscasubcuenta.h"


hcoamort::hcoamort() : QDialog() {
    ui.setupUi(this);

  ui.tablahcoamort->setRowCount(50);
  ui.tablahcoamort->setColumnCount(4);

 QStringList columnas;
 columnas << tr("Ejercicio") << tr("Gasto contable") << tr("Gasto fiscal");
 columnas << tr("Diferencia");

 ui.tablahcoamort->setHorizontalHeaderLabels(columnas);

 comadecimal=haycomadecimal(); 
 decimales=haydecimales();

 connect(ui.buscapushButton,SIGNAL(clicked()),SLOT(botonsubcuentapulsado()));
 connect(ui.subcuentalineEdit,SIGNAL(textChanged(QString)),SLOT(subctacambiada()));
 connect(ui.subcuentalineEdit,SIGNAL(editingFinished()),SLOT(finedicsubcuenta()));


}



void hcoamort::cargadatos()
{
  if (!existesubcuenta(ui.subcuentalineEdit->text()))
    {
      ui.titulotextLabel->setText("- -");
      return;
    }
 
    ui.titulotextLabel->setText(descripcioncuenta(ui.subcuentalineEdit->text()));

    QSqlQuery consulta =
      basedatos::instancia()->select4AmortfiscalycontableEjercicioscuentaejercicioorderapertura(
              ui.subcuentalineEdit->text() );

    int fila=0;
    double totcontable,totfiscal,totdiferencia;
    totcontable=0;
    totfiscal=0;
    totdiferencia=0;
  while (consulta.next())
      {
         QTableWidgetItem *newItem = new QTableWidgetItem(consulta.value(0).toString());
         ui.tablahcoamort->setItem(fila,0,newItem);
         QTableWidgetItem *newItem1 = new QTableWidgetItem(
                    formateanumerosep(consulta.value(1).toDouble(),comadecimal,decimales));
         newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.tablahcoamort->setItem(fila,1,newItem1);
         totcontable+=consulta.value(1).toDouble();

         QTableWidgetItem *newItem2 = new QTableWidgetItem(
                    formateanumerosep(consulta.value(2).toDouble(),comadecimal,decimales));
         newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.tablahcoamort->setItem(fila,2,newItem2);

         totfiscal+=consulta.value(2).toDouble();

         QTableWidgetItem *newItem3 = new QTableWidgetItem(
                    formateanumerosep(consulta.value(3).toDouble(),comadecimal,decimales));
         newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.tablahcoamort->setItem(fila,3,newItem3);
         totdiferencia+=consulta.value(3).toDouble();
         fila++;
      }
   ui.contablelineEdit->setText(formateanumerosep(totcontable,comadecimal,decimales));
   ui.fiscallineEdit->setText(formateanumerosep(totfiscal,comadecimal,decimales));
   ui.diferencialineEdit->setText(formateanumerosep(totdiferencia,comadecimal,decimales));

}


void hcoamort::botonsubcuentapulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.subcuentalineEdit->text());
    int rdo=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (rdo==QDialog::Accepted && esauxiliar(cadena2)) ui.subcuentalineEdit->setText(cadena2);
       else ui.subcuentalineEdit->setText("");
    delete labusqueda;
}


void hcoamort::subctacambiada()
{
  borratabla();
  if (esauxiliar(ui.subcuentalineEdit->text()))
    {
     cargadatos();
    }
  else ui.titulotextLabel->setText("- -");
}


void hcoamort::ctaexpandepunto()
{
     ui.subcuentalineEdit->setText(expandepunto(ui.subcuentalineEdit->text(),anchocuentas()));
}



void hcoamort::finedicsubcuenta()
{
 ctaexpandepunto();
 if (!esauxiliar(ui.subcuentalineEdit->text())) botonsubcuentapulsado();
}



void hcoamort::borratabla()
{
    ui.tablahcoamort->clearContents();
    ui.contablelineEdit->setText("");
    ui.fiscallineEdit->setText("");
    ui.diferencialineEdit->setText("");
}
