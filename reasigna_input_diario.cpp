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

#include "reasigna_input_diario.h"
#include "funciones.h"
#include "basedatos.h"
#include "introci.h"
#include "tabla_asientos.h"
#include <QTableWidget>
#include <QSqlRecord>
#include <QMessageBox>


reasigna_input_diario::reasigna_input_diario(bool qcomadecimal, bool qdecimales,
                                             QString qusuario) : QDialog() {
    ui.setupUi(this);


comadecimal=qcomadecimal;
decimales=qdecimales;
usuario=qusuario;
modoconsulta=false;
sin_imputaciones=false;

debe=false;

QStringList diarios;

diarios << diario_no_asignado();

  QSqlQuery query = basedatos::instancia()->selectCodigoDiariosordercodigo();
  if (query.isActive())
  while (query.next())
     {
         diarios << query.value(0).toString();
     }

  ui.diario_comboBox->addItems(diarios);

  ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  ui.diariotableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);


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

     connect(ui.aceptarpushButton,SIGNAL(clicked()),this,SLOT(aceptar()));

}



void reasigna_input_diario::pasacta( QString codcta )
{
  ctaactivo=codcta;
  QString cadena=codcta;
  cadena+=": ";
  cadena+=descripcioncuenta(codcta);
  ui.ctalabel->setText(cadena);
}


void reasigna_input_diario::buscapulsado()
{
 introci *c = new introci();
 c->pasaci(ui.cilineEdit->text());
 c->exec();
 ui.cilineEdit->setText(c->cadenaci());
 delete(c);
}


void reasigna_input_diario::fijadescripciones()
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



void reasigna_input_diario::compruebaci()
{
 // fin edición ci
 if (!ciok(ui.cilineEdit->text()))
    {
         QMessageBox::warning( this, tr("Imputaciones"),
		       tr("ERROR, código de imputación incorrecto"));
         ui.cilineEdit->clear();
         return;
    }
}

void reasigna_input_diario::cicambiado()
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

void reasigna_input_diario::asignacioncambiada()
{
 if (comadecimal) ui.asignacionlineEdit->setText(convacoma(ui.asignacionlineEdit->text()));
 double valor=convapunto(ui.asignacionlineEdit->text()).toDouble();
 if (valor>0.00001 && !modoconsulta)
   {
    ui.aceptarcipushButton->setEnabled(true);
    ui.aceptarcipushButton->setDefault(true);
   }
   else ui.aceptarcipushButton->setEnabled(false);
}



void reasigna_input_diario::botoncancelarcipulsado()
{
  ui.cilineEdit->clear();
  ui.asignacionlineEdit->clear();

}

void reasigna_input_diario::pasainputacion(QString CI, QString total, double porcentaje)
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

void reasigna_input_diario::pasatotal(QString total)
{
    total.remove('-');
    ui.importelineEdit->setText(total);
}

bool reasigna_input_diario::tablavacia()
{
   return ui.tableWidget->rowCount()==0;
}

void reasigna_input_diario::vaciatabla()
{
   ui.tableWidget->setRowCount(0);
}



void reasigna_input_diario::aceptarcipulsado()
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


void reasigna_input_diario::botonborrarcipulsado()
{
  if (ui.cilineEdit->text().length()==0) return;

  if (QMessageBox::question(this,tr("Borrar elemento CI"),
                                tr("¿ Desea borrar el CI '%1' ?").arg(ui.cilineEdit->text()))==QMessageBox::Yes)
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


void reasigna_input_diario::actualiza_porcentajes()
{
    for (int veces=0; veces<ui.tableWidget->rowCount(); veces++)
      {
        double porcentaje=convapunto(ui.tableWidget->item(veces,2)->text()).toDouble()/
                          convapunto(ui.importelineEdit->text()).toDouble()*100;
        QString cadporcentaje=formateanumero(porcentaje, comadecimal, decimales);
        ui.tableWidget->item(veces,1)->setText(cadporcentaje);
      }
}


void reasigna_input_diario::actualiza_total()
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


void reasigna_input_diario::tablacipulsada()
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


bool reasigna_input_diario::input_OK()
{
 double sumalineas=convapunto(ui.sumalineEdit->text()).toDouble();
 if (sumalineas-100>-0.0001 && sumalineas-100<0.0001) return true;
 return false;
}


QStringList reasigna_input_diario::lista_ci()
{
  QStringList lista;
  for (int veces=0; veces<ui.tableWidget->rowCount(); veces++)
      lista << ui.tableWidget->item(veces,0)->text();
  return lista;
}

QStringList reasigna_input_diario::lista_total()
{
    QStringList lista;
    for (int veces=0; veces<ui.tableWidget->rowCount(); veces++)
        lista << ui.tableWidget->item(veces,2)->text();
    return lista;

}


void reasigna_input_diario::activa_modoconsulta()
{
  modoconsulta=true;
  ui.asignacionlineEdit->setReadOnly(true);
  ui.borrarcipushButton->setEnabled(false);
}


void reasigna_input_diario::pasa_datos(QDate fechaini, QDate fechafinal,
                bool qesdebe, QString qcuenta)
{
  ui.inicialdateEdit->setDate(fechaini);
  ui.finaldateEdit->setDate(fechafinal);
  if (!qesdebe) ui.debelabel->setText("HABER");
  debe=qesdebe;
  cuenta=qcuenta;
  QString cadena;
  if (existecodigoplan(cuenta,&cadena))
      ui.ctalabel->setText(cuenta+": "+cadena);
   else ui.ctalabel->setText("- -");

  // cargamos datos de imputaciones
  // lista_inputs_cuenta(QString cuenta, QDate fecha_ini,
  //                     QDate fecha_fin, bool esdebe)
   QSqlQuery q=basedatos::instancia()->lista_inputs_cuenta(cuenta,fechaini,fechafinal,qesdebe);
   double total=0;
   if (q.isActive())
       while (q.next())
        {
         // select c.ci, sum(c.importe) from diario d, diario_ci c
         int fila=ui.diariotableWidget->rowCount();
         QString ci=q.value(0).toString();
         ui.diariotableWidget->insertRow(fila);
         QTableWidgetItem *newItem = new QTableWidgetItem(ci);
         ui.diariotableWidget->setItem(fila,0,newItem);

         /*QString cadporcentaje;
         cadporcentaje.setNum(porcentaje*100,'f',2);
         if (comadecimal) convacoma(cadporcentaje);*/
         QTableWidgetItem *newItem2 = new QTableWidgetItem(QString());
         newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.diariotableWidget->setItem(fila,1,newItem2);

         QString cadinput;
         cadinput.setNum(q.value(1).toDouble(),'f',4);
         // QMessageBox::warning( this, tr("Imputaciones"),cadinput);
         ui.cilineEdit->clear();

         total=total+q.value(1).toDouble();
         if (comadecimal) cadinput=convacoma(cadinput);
         QTableWidgetItem *newItem3 = new QTableWidgetItem(cadinput);
         newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.diariotableWidget->setItem(fila,2,newItem3);

        }
   if (total>-0.00001 && total<0.00001)
      {
       sin_imputaciones=true;
       return;
      }
   for (int veces=0; veces<ui.diariotableWidget->rowCount(); veces++)
     {
      double porcentaje=convapunto(ui.diariotableWidget->item(veces,2)->text()).toDouble()/
                        total*100;
      QString cadporcentaje; cadporcentaje.setNum(porcentaje,'f',2);
      if (comadecimal) cadporcentaje=convacoma(cadporcentaje);
      ui.diariotableWidget->item(veces,1)->setText(cadporcentaje);
     }
   total=CutAndRoundNumberToNDecimals (total, 2);
   QString cadtotal; cadtotal.setNum(total, 'f', 2);
   if (comadecimal) cadtotal=convacoma(cadtotal);
   ui.importelineEdit->setText(cadtotal);

   // ahora realizamos imputaciones si hay predefinidas

   QSqlQuery query = basedatos::instancia()->ci_input(cuenta);
   if (query.isActive())
   while (query.next())
      {
        pasainputacion(query.value(0).toString(),
                       cadtotal,
                       query.value(1).toDouble());
      }
   actualiza_total();
   pasatotal(cadtotal);

}

bool reasigna_input_diario::cuenta_sin_imputaciones()
{
  return sin_imputaciones;
}


void reasigna_input_diario::aceptar()
{
  tabla_asientos *t = new tabla_asientos(comadecimal,decimales,usuario);
  t->pasafechaasiento(ui.finaldateEdit->date());

  t->pasadiario(ui.diario_comboBox->currentText());

  // tablaasiento->pasadatos1(fila,cuenta,concepto,debe,
  //                 haber,documento,ci);

  double valor=convapunto(ui.importelineEdit->text()).toDouble();
  valor= -1*valor;
  QString cadnum;
  cadnum=formateanumero(valor, comadecimal, decimales);
  t->pasadatos1(0, cuenta,tr("Reasignación de imputaciones"),
                debe ? cadnum : "",
                !debe ? cadnum : "","","");

  t->pasadatos1(1, cuenta,tr("Reasignación de imputaciones"),
                debe ? ui.importelineEdit->text() : "",
                !debe ? ui.importelineEdit->text() : "","","");


  t->crea_tabla_ci(0);
  double total=0;
  total=convapunto(ui.importelineEdit->text()).toDouble();

  for (int veces=0; veces<ui.diariotableWidget->rowCount(); veces++)
      {
       t->pasainputacion(0, ui.diariotableWidget->item(veces,0)->text(),
                         total,
                         convapunto(ui.diariotableWidget->item(veces,2)->text()).toDouble()/total);
      }
    t->actualiza_inputacion(0, total, false);
    t->marca_ci(0);


  t->crea_tabla_ci(1);

  for (int veces=0; veces<ui.tableWidget->rowCount(); veces++)
        {
         t->pasainputacion(1, ui.tableWidget->item(veces,0)->text(),
                           total,
                           convapunto(ui.tableWidget->item(veces,2)->text()).toDouble()/total);
        }
  t->actualiza_inputacion(1, total, false);
  t->marca_ci(1);
  t->chequeatotales();
  int resultado=t->exec();
  if (resultado == QDialog::Accepted) accept();

}
