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

#include "asignainputdiario.h"
#include "funciones.h"
#include "basedatos.h"
#include "introci.h"
#include <QTableWidget>
#include <QSqlRecord>
#include <QMessageBox>


asignainputdiario::asignainputdiario(bool qcomadecimal, bool qdecimales) : QDialog() {
    ui.setupUi(this);

comadecimal=qcomadecimal;
decimales=qdecimales;
modoconsulta=false;
edicion_ext=false;
ui.activa_edicion_pushButton->hide();
    // connect(ui.aceptarpushButton,SIGNAL( clicked()),this,SLOT(botonaceptar()));
    ui.tableWidget->setColumnWidth(1,50);  // porcentaje


    connect(ui.cilineEdit,SIGNAL(textChanged(QString)),this,SLOT(fijadescripciones()));
    connect(ui.buscapushButton,SIGNAL(clicked()),this,SLOT(buscapulsado()));

    connect(ui.tableWidget ,SIGNAL(cellClicked ( int , int)),this,SLOT(tablacipulsada()));

     connect(ui.cilineEdit,SIGNAL(editingFinished()),this,SLOT(compruebaci()));
     connect(ui.cilineEdit,SIGNAL(textChanged(QString)),this,SLOT(cicambiado()));
     connect(ui.asignacionlineEdit,SIGNAL(textChanged(QString)),this,SLOT(asignacioncambiada()));
     connect(ui.cancelarcipushButton,SIGNAL(clicked()),this,SLOT(botoncancelarcipulsado()));

     connect(ui.aceptarcipushButton,SIGNAL(clicked()),this,SLOT(aceptarcipulsado()));
     connect(ui.borrarcipushButton,SIGNAL(clicked()),this,SLOT(botonborrarcipulsado()));

     connect(ui.predefpushButton,SIGNAL(clicked()),this, SLOT(carga_input_predef()));
     connect(ui.activa_edicion_pushButton,SIGNAL(clicked()),this, SLOT(activa_edicion()));

}



void asignainputdiario::pasacta( QString codcta )
{
  ctaactivo=codcta;
  QString cadena=codcta;
  cadena+=": ";
  cadena+=descripcioncuenta(codcta);
  ui.ctaactivolabel->setText(cadena);
}


void asignainputdiario::buscapulsado()
{
 introci *c = new introci();
 c->pasaci(ui.cilineEdit->text());
 c->exec();
 ui.cilineEdit->setText(c->cadenaci());
 delete(c);
}


void asignainputdiario::fijadescripciones()
{

  QString codigo=ui.cilineEdit->text();
  QString cadena,descripcion;
  QString qnivel=0;
  ui.nivel1lineEdit->setText("");
  ui.nivel2lineEdit->setText("");
  ui.nivel3lineEdit->setText("");

 if (codigo.length()==0) return;

 bool encontrada=buscaci(codigo.left(3),&descripcion,&qnivel);
 if (encontrada && qnivel.toInt()==1)
    ui.nivel1lineEdit->setText(descripcion);

 if (codigo.length()<=3) return;

 encontrada=buscaci(codigo.mid(3,3),&descripcion,&qnivel);
 int elnivel=qnivel.toInt();
 if (encontrada && elnivel==2)
     ui.nivel2lineEdit->setText(descripcion);

 if (codigo.length()<=6) return;

 encontrada=buscaci(codigo.right(codigo.length()-6),&descripcion,&qnivel);
 if (encontrada && qnivel.toInt()==3)
     ui.nivel3lineEdit->setText(descripcion);

}



void asignainputdiario::compruebaci()
{
 // fin edición ci
 if (ui.cilineEdit->text().isEmpty()) return;
 if (!ciok(ui.cilineEdit->text()))
    {
         QMessageBox::warning( this, tr("Plan de amortizaciones"),
		       tr("ERROR, código de imputación incorrecto"));
         ui.cilineEdit->clear();
         return;
    }
}

void asignainputdiario::cicambiado()
{
 if (ciok(ui.cilineEdit->text()))
    {
     ui.asignacionlineEdit->setEnabled(true);
    }
    else
        {
         ui.asignacionlineEdit->clear();
         ui.asignacionlineEdit->setEnabled(false);
        }

}

void asignainputdiario::asignacioncambiada()
{
 if (comadecimal) ui.asignacionlineEdit->setText(convacoma(ui.asignacionlineEdit->text()));
 double valor=convapunto(ui.asignacionlineEdit->text()).toDouble();
 if (valor>0.00001)
   {
    ui.aceptarcipushButton->setEnabled(true);
    ui.aceptarcipushButton->setDefault(true);
   }
   else ui.aceptarcipushButton->setEnabled(false);
}



void asignainputdiario::botoncancelarcipulsado()
{
  ui.cilineEdit->clear();
  ui.asignacionlineEdit->clear();

}

void asignainputdiario::carga_input_predef()
{
     // borramos contenido primero
    QString total=ui.importelineEdit->text();
     vaciatabla();

     QSqlQuery query = basedatos::instancia()->ci_input(ctaactivo);
     if (query.isActive())
     while (query.next())
        {
          pasainputacion(query.value(0).toString(),
                         total,
                         query.value(1).toDouble());
        }
     ui.importelineEdit->setText(total);

     actualiza_total();
}

void asignainputdiario::pasainputacion(QString CI, QString total, double porcentaje)
{
  total.remove('-');

  int fila=ui.tableWidget->rowCount();
  ui.tableWidget->insertRow(fila);
  QTableWidgetItem *newItem = new QTableWidgetItem(CI);
  ui.tableWidget->setItem(fila,0,newItem);

  QString cadporcentaje;
  cadporcentaje.setNum(porcentaje*100,'f',2);
  if (comadecimal) convacoma(cadporcentaje);
  QTableWidgetItem *newItem2 = new QTableWidgetItem(cadporcentaje);
  newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
  ui.tableWidget->setItem(fila,1,newItem2);

  QString cadinput;
  cadinput.setNum(convapunto(total).toDouble()*porcentaje,'f',4);
  QTableWidgetItem *newItem3 = new QTableWidgetItem(cadinput);
  newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
  ui.tableWidget->setItem(fila,2,newItem3);

  ui.importelineEdit->setText(total);
}

void asignainputdiario::pasatotal(QString total)
{
    total.remove('-');
    ui.importelineEdit->setText(total);
}

bool asignainputdiario::tablavacia()
{
   return ui.tableWidget->rowCount()==0;
}

void asignainputdiario::vaciatabla()
{
   ui.tableWidget->setRowCount(0);
}



void asignainputdiario::aceptarcipulsado()
{
 // insertamos o modificamos registro
 for (int veces=0; veces<ui.tableWidget->rowCount(); veces++)
    {
     if (ui.tableWidget->item(veces,0)->text()==ui.cilineEdit->text())
       {
         ui.tableWidget->item(veces,2)->setText(ui.asignacionlineEdit->text());
         actualiza_porcentajes();
         actualiza_total();
         ui.cilineEdit->setFocus();
         return;
       }
    }
 // si no hemos salido de la función es porque no existe el código de imputación

 int fila=ui.tableWidget->rowCount();
 ui.tableWidget->insertRow(fila);
 QTableWidgetItem *newItem = new QTableWidgetItem(ui.cilineEdit->text());
 ui.tableWidget->setItem(fila,0,newItem);

 QTableWidgetItem *newItem2 = new QTableWidgetItem(""); // se va a autocalcular
 newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
 ui.tableWidget->setItem(fila,1,newItem2);

 QTableWidgetItem *newItem3 = new QTableWidgetItem(ui.asignacionlineEdit->text());
 newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
 ui.tableWidget->setItem(fila,2,newItem3);

 actualiza_porcentajes();
 actualiza_total();
}


void asignainputdiario::botonborrarcipulsado()
{
  if (ui.cilineEdit->text().length()==0) return;

  int x=QMessageBox::question(this,tr("Borrar elemento CI"),
                                           tr("¿ Desea borrar el CI '%1' ?").arg(ui.cilineEdit->text()),
                                           tr("&Sí"),
                                           tr("&No"),
                                QString(),
                                           0,1);
  if (x==0)
     {
      for (int veces=0; veces<ui.tableWidget->rowCount(); veces++)
         {
          if (ui.tableWidget->item(veces,0)->text()==ui.cilineEdit->text())
            {
              ui.tableWidget->removeRow(veces);
              actualiza_porcentajes();
              actualiza_total();
              return;
            }
         }
     }
}


void asignainputdiario::actualiza_porcentajes()
{
    for (int veces=0; veces<ui.tableWidget->rowCount(); veces++)
      {
        double porcentaje=convapunto(ui.tableWidget->item(veces,2)->text()).toDouble()/
                          convapunto(ui.importelineEdit->text()).toDouble()*100;
        QString cadporcentaje=formateanumero(porcentaje, comadecimal, decimales);
        ui.tableWidget->item(veces,1)->setText(cadporcentaje);
      }
}


void asignainputdiario::actualiza_total()
{
    if (ui.tableWidget->rowCount()==0)
       { ui.sumalineEdit->clear(); return; }
    double total=0;
    double asignacion=convapunto(ui.importelineEdit->text()).toDouble();
    for (int veces=0; veces<ui.tableWidget->rowCount(); veces++)
      {
        total+=convapunto(ui.tableWidget->item(veces,2)->text()).toDouble();
      }
    QString cadnum=formateanumero(total/asignacion*100, comadecimal, decimales);;
    ui.sumalineEdit->setText(cadnum);
    cadnum=formateanumero(asignacion-total,comadecimal,decimales);
    ui.pendientelineEdit->setText(cadnum);
}


void asignainputdiario::tablacipulsada()
{
     if (ui.tableWidget->currentRow()>=0 &&
         ui.tableWidget->currentRow()<ui.tableWidget->rowCount())
       {
        int fila=ui.tableWidget->currentRow();
        ui.cilineEdit->setText(ui.tableWidget->item(fila,0)->text());
        ui.asignacionlineEdit->setText(ui.tableWidget->item(fila,2)->text());

        ui.asignacionlineEdit->setFocus();
        ui.aceptarcipushButton->setEnabled(false);
       }
}


bool asignainputdiario::input_OK()
{
 double sumalineas=convapunto(ui.sumalineEdit->text()).toDouble();
 if (sumalineas-100 > -0.009 && sumalineas-100<0.009) return true;
 return false;
}


QStringList asignainputdiario::lista_ci()
{
  QStringList lista;
  for (int veces=0; veces<ui.tableWidget->rowCount(); veces++)
      lista << ui.tableWidget->item(veces,0)->text();
  return lista;
}

QStringList asignainputdiario::lista_total()
{
    QStringList lista;
    for (int veces=0; veces<ui.tableWidget->rowCount(); veces++)
        lista << ui.tableWidget->item(veces,2)->text();
    return lista;

}


void asignainputdiario::activa_modoconsulta()
{
  modoconsulta=true;
  ui.asignacionlineEdit->setReadOnly(true);
  ui.borrarcipushButton->setEnabled(false);
  ui.predefpushButton->setEnabled(false);
  ui.activa_edicion_pushButton->show();
}


void asignainputdiario::activa_edicion()
{
  ui.asignacionlineEdit->setReadOnly(false);
  ui.borrarcipushButton->setEnabled(true);
  ui.predefpushButton->setEnabled(true);
  edicion_ext=true;
}

bool asignainputdiario::en_edicion_ext()
{
    return edicion_ext;
}
