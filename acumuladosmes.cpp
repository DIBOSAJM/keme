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

#include "acumuladosmes.h"
#include "funciones.h"
#include "buscasubcuenta.h"
#include "basedatos.h"
#include <QPainter>
#include <QMessageBox>

acumuladosmes::acumuladosmes() : QDialog() {
  ui.setupUi(this);


  comadecimal=haycomadecimal();
  decimales=haydecimales();

  QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
  QStringList ej1,ej2;

  ej2 << noejercicio();

  if ( query.isActive() ) {
          while ( query.next() ) {
                ej1 << query.value(0).toString();
                ej2 << query.value(0).toString();
          }
      }
  ui.ejerciciocomboBox->addItems(ej1);
  ui.ejercicio2comboBox->addItems(ej2);
 QDate fechaactual;
 QString ejercicio;
 ejercicio=ejerciciodelafecha(fechaactual.currentDate());

 if (ejercicio!="") ui.ejerciciocomboBox->setCurrentIndex(
        ui.ejerciciocomboBox->findText ( ejercicio, Qt::MatchExactly ));

 QStringList columnas;
 columnas << tr("MES") << tr("AÑO") << tr("SUMA DEBE");
 columnas << tr("SUMA HABER") << tr("SALDO") ;
 columnas << tr("AÑO") << tr("SUMA DEBE");
 columnas << tr("SUMA HABER") << tr("SALDO") ;

chartView_sumaH=0;
vbox_sumaH=0;
chartView_sumaD=0;
vbox_sumaD=0;
chartView_sumaS=0;
vbox_sumaS=0;

 ui.tabla->setHorizontalHeaderLabels(columnas);

 connect(ui.buscapushButton,SIGNAL(clicked()),this,SLOT(botonbuscasubcuentapulsado()));
 connect(ui.subcuentalineEdit,SIGNAL(textChanged(QString)),this,SLOT(subcuentacambiada()));
 connect(ui.subcuentalineEdit,SIGNAL(editingFinished()),this,SLOT(finedicsubcuenta()));
 connect(ui.refrescarpushButton,SIGNAL(clicked()),this,SLOT(refrescar()));

 connect(ui.anteriorpushButton,SIGNAL(clicked()),this,SLOT(subcuentaprevia()));
 connect(ui.posteriorpushButton,SIGNAL(clicked()),this,SLOT(subcuentasiguiente()));

 connect(ui.copiarpushButton,SIGNAL(clicked()),this,SLOT(txtexport()));

 connect(ui.informepushButton,SIGNAL(clicked()),this,SLOT(imprimir()));

 connect(ui.ejerciciocomboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(ejercicio_cambiado()));

 connect(ui.tabWidget,SIGNAL(currentChanged(int)),this,SLOT(tabcambiada()));
}



void acumuladosmes::pasacodigocuenta(QString codigo,QDate fecha)
{
  QString cadena;
  ui.subcuentalineEdit->setText(codigo);
  if (existecodigoplan(codigo,&cadena)) ui.titulotextLabel->setText(cadena);
  cadena=ejerciciodelafecha(fecha);
  if (cadena!="") ui.ejerciciocomboBox->setCurrentIndex(
        ui.ejerciciocomboBox->findText ( cadena, Qt::MatchExactly ));
  // cargadatos();
}


void acumuladosmes::cargadatos()
{
  for (int fila=0;fila<ui.tabla->rowCount(); fila++)
      for (int col=0; col<ui.tabla->columnCount(); col++)
          if (ui.tabla->item(fila,col)!=0) ui.tabla->item(fila,col)->setText("");
  //if ((ui.subcuentalineEdit->text().length()!=anchocuentas() && !cod_longitud_variable())
  //     || (cod_longitud_variable() && !esauxiliar(ui.subcuentalineEdit->text()))) return;
  QDate fechainicial; fechainicial=inicioejercicio(ui.ejerciciocomboBox->currentText());
  QDate fechafinal; fechafinal=finejercicio(ui.ejerciciocomboBox->currentText());
  QDate fechainicial2;
  QDate fechafinal2;
  bool hayejercicio2=ui.ejercicio2comboBox->currentText()!=noejercicio();
  if (hayejercicio2) {
      fechainicial2=inicioejercicio(ui.ejercicio2comboBox->currentText());
      fechafinal2=finejercicio(ui.ejercicio2comboBox->currentText());
  }
  QDate fechacorriente(fechainicial.year(),fechainicial.month(),1);
  QDate guarda_fechacorriente=fechacorriente;
  int veces=0;
  ui.progressBar->setMaximum(3);
  while (fechacorriente<fechafinal)
       {
          fechacorriente=fechacorriente.addMonths(1);
          veces++;
       }
  ui.tabla->setRowCount(veces+3);
  fechacorriente=guarda_fechacorriente;
  if (ui.tabla->item(0,0)==0)
      {
       QTableWidgetItem *newItem = new QTableWidgetItem("");
       ui.tabla->setItem(0,0,newItem);
      }
  ui.tabla->item(0,0)->setText(diario_apertura());

  veces=1;
  while (fechacorriente<fechafinal)
       {
          if (ui.tabla->item(veces,0)==0)
             { 
              QTableWidgetItem *newItem = new QTableWidgetItem("");
              ui.tabla->setItem(veces,0,newItem);
             }
          ui.tabla->item(veces,0)->setText(cadmes(fechacorriente.month()));
          QString cadnum;
          cadnum.setNum(fechacorriente.year());
          if (ui.tabla->item(veces,1)==0)
             { 
              QTableWidgetItem *newItem = new QTableWidgetItem("");
              ui.tabla->setItem(veces,1,newItem);
             }

          ui.tabla->item(veces,1)->setText(cadnum);
          fechacorriente=fechacorriente.addMonths(1);
          veces++;
       }

// fijamos columna 5 para ejercicio2
  if (hayejercicio2) {
    QDate fechacorriente2(fechainicial2.year(),fechainicial2.month(),1);
    veces=1;
    while (fechacorriente2<fechafinal2)
       {
          QString cadnum;
          cadnum.setNum(fechacorriente2.year());
          if (ui.tabla->item(veces,5)==0)
             {
              QTableWidgetItem *newItem = new QTableWidgetItem("");
              ui.tabla->setItem(veces,5,newItem);
             }

          ui.tabla->item(veces,5)->setText(cadnum);
          fechacorriente2=fechacorriente2.addMonths(1);
          veces++;
       }
   }



// ------------------------------------

   if (ui.tabla->item(veces,0)==0)
       { 
         QTableWidgetItem *newItem = new QTableWidgetItem("");
         ui.tabla->setItem(veces,0,newItem);
       }
  ui.tabla->item(veces,0)->setText(diario_regularizacion());

   if (ui.tabla->item(veces+1,0)==0)
       { 
         QTableWidgetItem *newItem = new QTableWidgetItem("");
         ui.tabla->setItem(veces+1,0,newItem);
       }
  ui.tabla->item(veces+1,0)->setText(diario_cierre());

  int posreg=veces;
  
  QSqlQuery query = basedatos::instancia()->selectSubdebesumhabersumdebehaberdiariodiarioapertura (
          fechainicial,fechafinal,ui.subcuentalineEdit->text());
  if ( (query.isActive()) && (query.next()) )
       {
               if (ui.tabla->item(0,2)==0)
                  { 
                   QTableWidgetItem *newItem = new QTableWidgetItem("");
                   newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                   ui.tabla->setItem(0,2,newItem);
                  }
	       ui.tabla->item(0,2)->setText(formateanumero(query.value(0).toDouble(),comadecimal,decimales));
               if (ui.tabla->item(0,3)==0)
                  { 
                   QTableWidgetItem *newItem = new QTableWidgetItem("");
                   newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                   ui.tabla->setItem(0,3,newItem);
                  }
	       ui.tabla->item(0,3)->setText(formateanumero(query.value(1).toDouble(),comadecimal,decimales));
               if (ui.tabla->item(0,4)==0)
                  { 
                   QTableWidgetItem *newItem = new QTableWidgetItem("");
                   newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                   ui.tabla->setItem(0,4,newItem);
                  }
	       ui.tabla->item(0,4)->setText(formateanumero(query.value(2).toDouble(),comadecimal,decimales));
      }

  double saldo=0;
  saldo=query.value(2).toString().toDouble();

  // ejercicio 2
  // ---->
  if (hayejercicio2) {
    query = basedatos::instancia()->selectSubdebesumhabersumdebehaberdiariodiarioapertura (
            fechainicial2,fechafinal2,ui.subcuentalineEdit->text());
    if ( (query.isActive()) && (query.next()) )
         {
                 if (ui.tabla->item(0,6)==0)
                    {
                     QTableWidgetItem *newItem = new QTableWidgetItem("");
                     newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                     ui.tabla->setItem(0,6,newItem);
                    }
                 ui.tabla->item(0,6)->setText(formateanumero(query.value(0).toDouble(),comadecimal,decimales));
                 if (ui.tabla->item(0,7)==0)
                    {
                     QTableWidgetItem *newItem = new QTableWidgetItem("");
                     newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                     ui.tabla->setItem(0,7,newItem);
                    }
                 ui.tabla->item(0,7)->setText(formateanumero(query.value(1).toDouble(),comadecimal,decimales));
                 if (ui.tabla->item(0,8)==0)
                    {
                     QTableWidgetItem *newItem = new QTableWidgetItem("");
                     newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                     ui.tabla->setItem(0,8,newItem);
                    }
             ui.tabla->item(0,8)->setText(formateanumero(query.value(2).toDouble(),comadecimal,decimales));
        }
  }

  ui.progressBar->setValue(1);
  double saldo2=0;
  if (hayejercicio2) saldo2=query.value(2).toString().toDouble();

  query = basedatos::instancia()->selectMonthyeardebehaberdiario(fechainicial,fechafinal,ui.subcuentalineEdit->text());
 if ( query.isActive() ) {
        while ( query.next() )
         {
	      veces=0;
	      while (veces<posreg)
	       {
	        if (cadmes(query.value(0).toInt())==ui.tabla->item(veces,0)->text() &&
            query.value(1).toString()==ui.tabla->item(veces,1)->text())
	         {
                     if (ui.tabla->item(veces,2)==0)
                       { 
                        QTableWidgetItem *newItem = new QTableWidgetItem("");
                        newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                        ui.tabla->setItem(veces,2,newItem);
                       }
	             ui.tabla->item(veces,2)->setText(formateanumero(query.value(2).toDouble(),
                                                      comadecimal,decimales));
                     if (ui.tabla->item(veces,3)==0)
                       { 
                        QTableWidgetItem *newItem = new QTableWidgetItem("");
                        newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                        ui.tabla->setItem(veces,3,newItem);
                       }
	             ui.tabla->item(veces,3)->setText(formateanumero(query.value(3).toDouble(),
                                       comadecimal,decimales));
	             saldo+=query.value(4).toDouble();
                     if (ui.tabla->item(veces,4)==0)
                       { 
                        QTableWidgetItem *newItem = new QTableWidgetItem("");
                        newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                        ui.tabla->setItem(veces,4,newItem);
                       }
	             ui.tabla->item(veces,4)->setText(formateanumero(saldo,comadecimal,decimales));
	         }
	       veces++;
           }
         }
      }

 if (hayejercicio2) {
     query = basedatos::instancia()->selectMonthyeardebehaberdiario(fechainicial2,fechafinal2,ui.subcuentalineEdit->text());
    if ( query.isActive() ) {
           while ( query.next() )
            {
             veces=0;
             while (veces<posreg)
              {
               if (cadmes(query.value(0).toInt())==ui.tabla->item(veces,0)->text() &&
               query.value(1).toString()==ui.tabla->item(veces,5)->text())
                {
                        if (ui.tabla->item(veces,6)==0)
                          {
                           QTableWidgetItem *newItem = new QTableWidgetItem("");
                           newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                           ui.tabla->setItem(veces,6,newItem);
                          }
                        ui.tabla->item(veces,6)->setText(formateanumero(query.value(2).toDouble(),
                                                         comadecimal,decimales));
                        if (ui.tabla->item(veces,7)==0)
                          {
                           QTableWidgetItem *newItem = new QTableWidgetItem("");
                           newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                           ui.tabla->setItem(veces,7,newItem);
                          }
                        ui.tabla->item(veces,7)->setText(formateanumero(query.value(3).toDouble(),
                                          comadecimal,decimales));
                        saldo2+=query.value(4).toDouble();
                        if (ui.tabla->item(veces,8)==0)
                          {
                           QTableWidgetItem *newItem = new QTableWidgetItem("");
                           newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                           ui.tabla->setItem(veces,8,newItem);
                          }
                        ui.tabla->item(veces,8)->setText(formateanumero(saldo2,comadecimal,decimales));
                }
              veces++;
              }
            }
         }
 }
 
   ui.progressBar->setValue(2);
 
  query = basedatos::instancia()->selectSubdebesumhabersumdebehaberdiariodiarioregularizacion(fechainicial, fechafinal, ui.subcuentalineEdit->text());
 if ( query.isActive() ) {
          if ( query.next() )
              {
                if (ui.tabla->item(posreg,2)==0)
                   { 
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.tabla->setItem(posreg,2,newItem);
                   }
	        ui.tabla->item(posreg,2)->setText(formateanumero(query.value(0).toDouble(),
                    comadecimal,decimales));
                if (ui.tabla->item(posreg,3)==0)
                   { 
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.tabla->setItem(posreg,3,newItem);
                   }
	        ui.tabla->item(posreg,3)->setText(formateanumero(query.value(1).toDouble(),
                    comadecimal,decimales));
                saldo+=query.value(2).toDouble();
                if (ui.tabla->item(posreg,4)==0)
                   { 
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.tabla->setItem(posreg,4,newItem);
                   }
	        ui.tabla->item(posreg,4)->setText(formateanumero(saldo,comadecimal,decimales));
              }
      }
 
 if (hayejercicio2) {
     query = basedatos::instancia()->selectSubdebesumhabersumdebehaberdiariodiarioregularizacion(fechainicial2, fechafinal2, ui.subcuentalineEdit->text());
    if ( query.isActive() ) {
             if ( query.next() )
                 {
                   if (ui.tabla->item(posreg,6)==0)
                      {
                       QTableWidgetItem *newItem = new QTableWidgetItem("");
                       newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                       ui.tabla->setItem(posreg,6,newItem);
                      }
                   ui.tabla->item(posreg,6)->setText(formateanumero(query.value(0).toDouble(),
                       comadecimal,decimales));
                   if (ui.tabla->item(posreg,7)==0)
                      {
                       QTableWidgetItem *newItem = new QTableWidgetItem("");
                       newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                       ui.tabla->setItem(posreg,7,newItem);
                      }
                   ui.tabla->item(posreg,7)->setText(formateanumero(query.value(1).toDouble(),
                       comadecimal,decimales));
                   saldo2+=query.value(2).toDouble();
                   if (ui.tabla->item(posreg,8)==0)
                      {
                       QTableWidgetItem *newItem = new QTableWidgetItem("");
                       newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                       ui.tabla->setItem(posreg,8,newItem);
                      }
                   ui.tabla->item(posreg,8)->setText(formateanumero(saldo2,comadecimal,decimales));
                 }
         }
 }

 query = basedatos::instancia()->selectSubdebesumhabersumdebehaberdiariodiariocierre(fechainicial,  fechafinal, ui.subcuentalineEdit->text());
 if ( query.isActive() ) {
          if ( query.next() )
              {
                if (ui.tabla->item(posreg+1,2)==0)
                   { 
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.tabla->setItem(posreg+1,2,newItem);
                   }
                ui.tabla->item(posreg+1,2)->setText(formateanumero(query.value(0).toDouble(),
                      comadecimal,decimales));
                if (ui.tabla->item(posreg+1,3)==0)
                   { 
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.tabla->setItem(posreg+1,3,newItem);
                   }
                ui.tabla->item(posreg+1,3)->setText(formateanumero(query.value(1).toDouble(),
                    comadecimal,decimales));
                saldo+=query.value(2).toDouble();
                if (ui.tabla->item(posreg+1,4)==0)
                   { 
                    QTableWidgetItem *newItem = new QTableWidgetItem("");
                    newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                    ui.tabla->setItem(posreg+1,4,newItem);
                   }
	        ui.tabla->item(posreg+1,4)->setText(formateanumero(saldo,comadecimal,decimales));
              }
  }

 if (hayejercicio2) {
     query = basedatos::instancia()->selectSubdebesumhabersumdebehaberdiariodiariocierre(fechainicial2,  fechafinal2, ui.subcuentalineEdit->text());
     if ( query.isActive() ) {
              if ( query.next() )
                  {
                    if (ui.tabla->item(posreg+1,6)==0)
                       {
                        QTableWidgetItem *newItem = new QTableWidgetItem("");
                        newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                        ui.tabla->setItem(posreg+1,6,newItem);
                       }
                    ui.tabla->item(posreg+1,6)->setText(formateanumero(query.value(0).toDouble(),
                          comadecimal,decimales));
                    if (ui.tabla->item(posreg+1,7)==0)
                       {
                        QTableWidgetItem *newItem = new QTableWidgetItem("");
                        newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                        ui.tabla->setItem(posreg+1,7,newItem);
                       }
                    ui.tabla->item(posreg+1,7)->setText(formateanumero(query.value(1).toDouble(),
                        comadecimal,decimales));
                    saldo2+=query.value(2).toDouble();
                    if (ui.tabla->item(posreg+1,8)==0)
                       {
                        QTableWidgetItem *newItem = new QTableWidgetItem("");
                        newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                        ui.tabla->setItem(posreg+1,8,newItem);
                       }
                    ui.tabla->item(posreg+1,8)->setText(formateanumero(saldo2,comadecimal,decimales));
                  }
      }
 }

 ui.progressBar->setValue(3);
 
 veces=1;
 QString cad4;
 if (ui.tabla->item(0,4)==0)
     {
       QTableWidgetItem *newItem = new QTableWidgetItem("");
       newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
       ui.tabla->setItem(0,4,newItem);
     }
 cad4=ui.tabla->item(0,4)->text();
 if (cad4=="0" || cad4.length()==0) cad4=formateanumero(0,comadecimal,decimales);
 while (veces<ui.tabla->rowCount())
    {
       if (ui.tabla->item(veces,2)==0)
           { 
            QTableWidgetItem *newItem = new QTableWidgetItem("");
            newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
            ui.tabla->setItem(veces,2,newItem);
           }
       if (ui.tabla->item(veces,2)->text().length()==0) ui.tabla->item(veces,2)->setText(
                                                      formateanumero(0,comadecimal,decimales));
       if (ui.tabla->item(veces,3)==0)
           { 
            QTableWidgetItem *newItem = new QTableWidgetItem("");
            newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
            ui.tabla->setItem(veces,3,newItem);
           }
       if (ui.tabla->item(veces,3)->text().length()==0) ui.tabla->item(veces,3)->setText(
                                                      formateanumero(0,comadecimal,decimales));
       if (ui.tabla->item(veces,4)==0)
           { 
            QTableWidgetItem *newItem = new QTableWidgetItem("");
            newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
            ui.tabla->setItem(veces,4,newItem);
           }
       if (ui.tabla->item(veces,4)->text().length()==0) ui.tabla->item(veces,4)->setText(cad4);
       cad4=ui.tabla->item(veces,4)->text();
       veces++;
    }

 if (hayejercicio2) {
     veces=1;
     QString cad4;
     if (ui.tabla->item(0,8)==0)
         {
           QTableWidgetItem *newItem = new QTableWidgetItem("");
           newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui.tabla->setItem(0,8,newItem);
         }
     cad4=ui.tabla->item(0,8)->text();
     if (cad4=="0" || cad4.length()==0) cad4=formateanumero(0,comadecimal,decimales);
     while (veces<ui.tabla->rowCount())
        {
           if (ui.tabla->item(veces,6)==0)
               {
                QTableWidgetItem *newItem = new QTableWidgetItem("");
                newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                ui.tabla->setItem(veces,6,newItem);
               }
           if (ui.tabla->item(veces,6)->text().length()==0) ui.tabla->item(veces,6)->setText(
                                                          formateanumero(0,comadecimal,decimales));
           if (ui.tabla->item(veces,7)==0)
               {
                QTableWidgetItem *newItem = new QTableWidgetItem("");
                newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                ui.tabla->setItem(veces,7,newItem);
               }
           if (ui.tabla->item(veces,7)->text().length()==0) ui.tabla->item(veces,7)->setText(
                                                          formateanumero(0,comadecimal,decimales));
           if (ui.tabla->item(veces,8)==0)
               {
                QTableWidgetItem *newItem = new QTableWidgetItem("");
                newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
                ui.tabla->setItem(veces,8,newItem);
               }
           if (ui.tabla->item(veces,8)->text().length()==0) ui.tabla->item(veces,8)->setText(cad4);
           cad4=ui.tabla->item(veces,8)->text();
           veces++;
        }
 }


 ui.progressBar->reset();
 ui.tabla->resizeColumnToContents(0);
 ui.tabla->resizeColumnToContents(1);
 ui.tabla->resizeColumnToContents(5);

 //dibujagrafsaldos();
 //dibujagrafsumadebe();
 //dibujagrafsumahaber();

}

void acumuladosmes::dibujagrafsaldos()
{
    QBarSet *set0 = new QBarSet(ui.ejerciciocomboBox->currentText());
    QBarSet *set1;
    if (ui.ejercicio2comboBox->currentText()!=noejercicio())
        set1=new QBarSet(ui.ejercicio2comboBox->currentText());

    int positivos=0;
    int negativos=0;
    for (int veces=1; veces<ui.tabla->rowCount()-2; veces++)
       {
        if (convapunto(ui.tabla->item(veces,4)->text()).toDouble()>0) positivos++;
        if (convapunto(ui.tabla->item(veces,4)->text()).toDouble()<0) negativos++;
       }
    double mul=1;
    if (negativos>positivos) mul=-1;


    for (int veces=1; veces<ui.tabla->rowCount()-2; veces++) {
        //*set0 << ui.tabla->item(veces,3)->text().toDouble();
        set0->append(mul*convapunto(ui.tabla->item(veces,4)->text()).toDouble());
    }

    if (ui.ejercicio2comboBox->currentText()!=noejercicio())
        for (int veces=1; veces<ui.tabla->rowCount()-2; veces++) {
            //*set1 << ui.tabla->item(veces,7)->text().toDouble();
            set1->append(mul*convapunto(ui.tabla->item(veces,8)->text()).toDouble());
        }

    QBarSeries *series = new QBarSeries();
    series->append(set0);
    if (ui.ejercicio2comboBox->currentText()!=noejercicio())
       series->append(set1);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(tr("Evolución SALDOS"));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    for (int veces=1; veces<ui.tabla->rowCount()-2; veces++)
        categories << ui.tabla->item(veces,0)->text();

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    chartView_sumaS = new QChartView(chart);
    vbox_sumaS = new QVBoxLayout;
    vbox_sumaS->addWidget(chartView_sumaS);
    ui.sumaS_groupBox->setLayout(vbox_sumaS);

    chartView_sumaS->setRenderHint(QPainter::Antialiasing);

    ui.sumaS_groupBox->show();

    /*
    //buscamos máximo saldo (columna 4)
    double maxvalpos=0;
    double maxvalneg=0;
    int positivos=0;
    int negativos=0;
    for (int veces=1; veces<ui.tabla->rowCount()-2; veces++)
       {
        if (convapunto(ui.tabla->item(veces,4)->text()).toDouble()>0) positivos++;
        if (convapunto(ui.tabla->item(veces,4)->text()).toDouble()<0) negativos++;
        if (convapunto(ui.tabla->item(veces,4)->text()).toDouble()>0 &&
            convapunto(ui.tabla->item(veces,4)->text()).toDouble()>maxvalpos)
            maxvalpos=convapunto(ui.tabla->item(veces,4)->text()).toDouble();
        if (convapunto(ui.tabla->item(veces,4)->text()).toDouble()<0 &&
            convapunto(ui.tabla->item(veces,4)->text()).toDouble()<maxvalneg)
            maxvalneg=convapunto(ui.tabla->item(veces,4)->text()).toDouble();
       }
    double maxval=0;
    if (positivos>negativos) maxval=maxvalpos;
       else maxval=-maxvalneg;

    double mul100=CutAndRoundNumberToNDecimals(maxval/100,0);
    mul100=mul100+1;
    double maxejepos=mul100*100;

    QPixmap pixmap(850,450);
        pixmap.fill(QColor("transparent"));

        QPainter painter(&pixmap);
        // painter.setBrush(QBrush(Qt::black));
        // painter.drawRect(10, 10, 600, 400);


        // QPainter painter;
        // QFont font;
        painter.begin(this);
        Nightcharts Histograma;
        Histograma.setType(Nightcharts::Histogramm);//{Histogramm,Pie,DPie};
        // PieChart.setType(Nightcharts::Dpie);//{Histogramm,Pie,DPie};
        // PieChart.setLegendType(Nightcharts::Round);//{Round,Vertical}
        Histograma.setLegendType(Nightcharts::Vertical);//{Round,Vertical}
        //PieChart.setCords(150,30,this->width()/1.5,this->height()/1.5);
        Histograma.set_max_absolute(maxejepos);
        Histograma.setCords(100,60,500,300);
        double mul=1;
        if (negativos>positivos) mul=-1;
        Histograma.addPiece("Enero",Qt::red,mul*convapunto(ui.tabla->item(1,4)->text()).toDouble());
        Histograma.addPiece("Febrero",Qt::green,mul*convapunto(ui.tabla->item(2,4)->text()).toDouble());
        Histograma.addPiece("Marzo",Qt::cyan,mul*convapunto(ui.tabla->item(3,4)->text()).toDouble());
        Histograma.addPiece("Abril",Qt::yellow,mul*convapunto(ui.tabla->item(4,4)->text()).toDouble());
        Histograma.addPiece("Mayo",Qt::blue,mul*convapunto(ui.tabla->item(5,4)->text()).toDouble());
        Histograma.addPiece("Junio",Qt::darkGreen,mul*convapunto(ui.tabla->item(6,4)->text()).toDouble());
        Histograma.addPiece("Julio",Qt::darkCyan,mul*convapunto(ui.tabla->item(7,4)->text()).toDouble());
        Histograma.addPiece("Agosto",Qt::magenta,mul*convapunto(ui.tabla->item(8,4)->text()).toDouble());
        Histograma.addPiece("Septiembre",Qt::gray,mul*convapunto(ui.tabla->item(9,4)->text()).toDouble());
        Histograma.addPiece("Octubre",Qt::darkBlue,mul*convapunto(ui.tabla->item(10,4)->text()).toDouble());
        Histograma.addPiece("Noviembre",Qt::darkRed,mul*convapunto(ui.tabla->item(11,4)->text()).toDouble());
        Histograma.addPiece("Diciembre",Qt::darkYellow,mul*convapunto(ui.tabla->item(12,4)->text()).toDouble());
        Histograma.draw(&painter);
        Histograma.drawLegend(&painter);

        ui.saldosgraflabel->setPixmap(pixmap);
        */
}






void acumuladosmes::dibujagrafsumadebe()
{
    QBarSet *set0 = new QBarSet(ui.ejerciciocomboBox->currentText());
    QBarSet *set1;
    if (ui.ejercicio2comboBox->currentText()!=noejercicio())
        set1=new QBarSet(ui.ejercicio2comboBox->currentText());

    for (int veces=1; veces<ui.tabla->rowCount()-2; veces++) {
        //*set0 << ui.tabla->item(veces,3)->text().toDouble();
        set0->append(convapunto(ui.tabla->item(veces,2)->text()).toDouble());
    }

    if (ui.ejercicio2comboBox->currentText()!=noejercicio())
        for (int veces=1; veces<ui.tabla->rowCount()-2; veces++) {
            //*set1 << ui.tabla->item(veces,7)->text().toDouble();
            set1->append(convapunto(ui.tabla->item(veces,6)->text()).toDouble());
        }

    QBarSeries *series = new QBarSeries();
    series->append(set0);
    if (ui.ejercicio2comboBox->currentText()!=noejercicio())
       series->append(set1);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(tr("Evolución sumas DEBE"));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    for (int veces=1; veces<ui.tabla->rowCount()-2; veces++)
        categories << ui.tabla->item(veces,0)->text();

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    chartView_sumaD = new QChartView(chart);
    vbox_sumaD = new QVBoxLayout;
    vbox_sumaD->addWidget(chartView_sumaD);

    ui.sumaD_groupBox->setLayout(vbox_sumaD);

    chartView_sumaD->setRenderHint(QPainter::Antialiasing);

    ui.sumaD_groupBox->show();

    /*
    //buscamos máximo saldo (columna 2)
    double maxvalpos=0;
    double maxvalneg=0;
    int positivos=0;
    int negativos=0;
    for (int veces=1; veces<ui.tabla->rowCount()-2; veces++)
       {
        if (convapunto(ui.tabla->item(veces,2)->text()).toDouble()>0) positivos++;
        if (convapunto(ui.tabla->item(veces,2)->text()).toDouble()<0) negativos++;
        if (convapunto(ui.tabla->item(veces,2)->text()).toDouble()>0 &&
            convapunto(ui.tabla->item(veces,2)->text()).toDouble()>maxvalpos)
            maxvalpos=convapunto(ui.tabla->item(veces,2)->text()).toDouble();
        if (convapunto(ui.tabla->item(veces,2)->text()).toDouble()<0 &&
            convapunto(ui.tabla->item(veces,2)->text()).toDouble()<maxvalneg)
            maxvalneg=convapunto(ui.tabla->item(veces,2)->text()).toDouble();
       }
    double maxval=0;
    if (positivos>negativos) maxval=maxvalpos;
       else maxval=-maxvalneg;

    double mul100=CutAndRoundNumberToNDecimals(maxval/100,0);
    mul100=mul100+1;
    double maxejepos=mul100*100;

    QPixmap pixmap(850,450);
        pixmap.fill(QColor("transparent"));

        QPainter painter(&pixmap);
        // painter.setBrush(QBrush(Qt::black));
        // painter.drawRect(10, 10, 600, 400);


        // QPainter painter;
        // QFont font;
        painter.begin(this);
        Nightcharts Histograma;
        Histograma.setType(Nightcharts::Histogramm);//{Histogramm,Pie,DPie};
        // PieChart.setType(Nightcharts::Dpie);//{Histogramm,Pie,DPie};
        // PieChart.setLegendType(Nightcharts::Round);//{Round,Vertical}
        Histograma.setLegendType(Nightcharts::Vertical);//{Round,Vertical}
        //PieChart.setCords(150,30,this->width()/1.5,this->height()/1.5);
        Histograma.set_max_absolute(maxejepos);
        Histograma.setCords(100,60,500,300);
        double mul=1;
        if (negativos>positivos) mul=-1;
        Histograma.addPiece("Enero",Qt::red,mul*convapunto(ui.tabla->item(1,2)->text()).toDouble());
        Histograma.addPiece("Febrero",Qt::green,mul*convapunto(ui.tabla->item(2,2)->text()).toDouble());
        Histograma.addPiece("Marzo",Qt::cyan,mul*convapunto(ui.tabla->item(3,2)->text()).toDouble());
        Histograma.addPiece("Abril",Qt::yellow,mul*convapunto(ui.tabla->item(4,2)->text()).toDouble());
        Histograma.addPiece("Mayo",Qt::blue,mul*convapunto(ui.tabla->item(5,2)->text()).toDouble());
        Histograma.addPiece("Junio",Qt::darkGreen,mul*convapunto(ui.tabla->item(6,2)->text()).toDouble());
        Histograma.addPiece("Julio",Qt::darkCyan,mul*convapunto(ui.tabla->item(7,2)->text()).toDouble());
        Histograma.addPiece("Agosto",Qt::magenta,mul*convapunto(ui.tabla->item(8,2)->text()).toDouble());
        Histograma.addPiece("Septiembre",Qt::gray,mul*convapunto(ui.tabla->item(9,2)->text()).toDouble());
        Histograma.addPiece("Octubre",Qt::darkBlue,mul*convapunto(ui.tabla->item(10,2)->text()).toDouble());
        Histograma.addPiece("Noviembre",Qt::darkRed,mul*convapunto(ui.tabla->item(11,2)->text()).toDouble());
        Histograma.addPiece("Diciembre",Qt::darkYellow,mul*convapunto(ui.tabla->item(12,2)->text()).toDouble());
        Histograma.draw(&painter);
        Histograma.drawLegend(&painter);

        ui.sumadebegraflabel->setPixmap(pixmap);
       */
}

void acumuladosmes::tabcambiada() {

    if (ui.tabWidget->currentIndex()==1) {
        if (vbox_sumaD!=0) {
           delete(vbox_sumaD);
        }
       if (chartView_sumaD!=0) {
          delete(chartView_sumaD);
       }
       dibujagrafsumadebe();
    }

    // gráfico suma haber es el tab 2
    if (ui.tabWidget->currentIndex()==2) {
       if (chartView_sumaH!=0) {
          delete(chartView_sumaH);
       }
       if (vbox_sumaH!=0) {
          delete(vbox_sumaH);
       }
       dibujagrafsumahaber();
    }

    if (ui.tabWidget->currentIndex()==3) {
       if (chartView_sumaS!=0) {
          delete(chartView_sumaS);
       }
       if (vbox_sumaS!=0) {
          delete(vbox_sumaS);
       }
       dibujagrafsaldos();
    }

}


void acumuladosmes::dibujagrafsumahaber()
{
    QBarSet *set0 = new QBarSet(ui.ejerciciocomboBox->currentText());
    QBarSet *set1;
    if (ui.ejercicio2comboBox->currentText()!=noejercicio())
        set1=new QBarSet(ui.ejercicio2comboBox->currentText());

    for (int veces=1; veces<ui.tabla->rowCount()-2; veces++) {
        //*set0 << ui.tabla->item(veces,3)->text().toDouble();
        set0->append(convapunto(ui.tabla->item(veces,3)->text()).toDouble());
    }

    if (ui.ejercicio2comboBox->currentText()!=noejercicio())
        for (int veces=1; veces<ui.tabla->rowCount()-2; veces++) {
            //*set1 << ui.tabla->item(veces,7)->text().toDouble();
            set1->append(convapunto(ui.tabla->item(veces,7)->text()).toDouble());
        }

    QBarSeries *series = new QBarSeries();
    series->append(set0);
    if (ui.ejercicio2comboBox->currentText()!=noejercicio())
       series->append(set1);

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(tr("Evolución sumas HABER"));
    chart->setAnimationOptions(QChart::SeriesAnimations);

    QStringList categories;
    for (int veces=1; veces<ui.tabla->rowCount()-2; veces++)
        categories << ui.tabla->item(veces,0)->text();

    QBarCategoryAxis *axis = new QBarCategoryAxis();
    axis->append(categories);
    chart->createDefaultAxes();
    chart->setAxisX(axis, series);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    chartView_sumaH = new QChartView(chart);
    vbox_sumaH = new QVBoxLayout;
    vbox_sumaH->addWidget(chartView_sumaH);
    ui.sumaH_groupBox->setLayout(vbox_sumaH);

    chartView_sumaH->setRenderHint(QPainter::Antialiasing);

    ui.sumaH_groupBox->show();


/*
    //buscamos máximo saldo (columna 3)
    double maxvalpos=0;
    double maxvalneg=0;
    int positivos=0;
    int negativos=0;
    for (int veces=1; veces<ui.tabla->rowCount()-2; veces++)
       {
        if (convapunto(ui.tabla->item(veces,3)->text()).toDouble()>0) positivos++;
        if (convapunto(ui.tabla->item(veces,3)->text()).toDouble()<0) negativos++;
        if (convapunto(ui.tabla->item(veces,3)->text()).toDouble()>0 &&
            convapunto(ui.tabla->item(veces,3)->text()).toDouble()>maxvalpos)
            maxvalpos=convapunto(ui.tabla->item(veces,3)->text()).toDouble();
        if (convapunto(ui.tabla->item(veces,3)->text()).toDouble()<0 &&
            convapunto(ui.tabla->item(veces,3)->text()).toDouble()<maxvalneg)
            maxvalneg=convapunto(ui.tabla->item(veces,3)->text()).toDouble();
       }
    double maxval=0;
    if (positivos>negativos) maxval=maxvalpos;
       else maxval=-maxvalneg;

    double mul100=CutAndRoundNumberToNDecimals(maxval/100,0);
    mul100=mul100+1;
    double maxejepos=mul100*100;

    QPixmap pixmap(850,450);
        pixmap.fill(QColor("transparent"));

        QPainter painter(&pixmap);
        // painter.setBrush(QBrush(Qt::black));
        // painter.drawRect(10, 10, 600, 400);


        // QPainter painter;
        // QFont font;
        painter.begin(this);
        Nightcharts Histograma;
        Histograma.setType(Nightcharts::Histogramm);//{Histogramm,Pie,DPie};
        // PieChart.setType(Nightcharts::Dpie);//{Histogramm,Pie,DPie};
        // PieChart.setLegendType(Nightcharts::Round);//{Round,Vertical}
        Histograma.setLegendType(Nightcharts::Vertical);//{Round,Vertical}
        //PieChart.setCords(150,30,this->width()/1.5,this->height()/1.5);
        Histograma.set_max_absolute(maxejepos);
        Histograma.setCords(100,60,500,300);
        double mul=1;
        if (negativos>positivos) mul=-1;
        Histograma.addPiece("Enero",Qt::red,mul*convapunto(ui.tabla->item(1,3)->text()).toDouble());
        Histograma.addPiece("Febrero",Qt::green,mul*convapunto(ui.tabla->item(2,3)->text()).toDouble());
        Histograma.addPiece("Marzo",Qt::cyan,mul*convapunto(ui.tabla->item(3,3)->text()).toDouble());
        Histograma.addPiece("Abril",Qt::yellow,mul*convapunto(ui.tabla->item(4,3)->text()).toDouble());
        Histograma.addPiece("Mayo",Qt::blue,mul*convapunto(ui.tabla->item(5,3)->text()).toDouble());
        Histograma.addPiece("Junio",Qt::darkGreen,mul*convapunto(ui.tabla->item(6,3)->text()).toDouble());
        Histograma.addPiece("Julio",Qt::darkCyan,mul*convapunto(ui.tabla->item(7,3)->text()).toDouble());
        Histograma.addPiece("Agosto",Qt::magenta,mul*convapunto(ui.tabla->item(8,3)->text()).toDouble());
        Histograma.addPiece("Septiembre",Qt::gray,mul*convapunto(ui.tabla->item(9,3)->text()).toDouble());
        Histograma.addPiece("Octubre",Qt::darkBlue,mul*convapunto(ui.tabla->item(10,3)->text()).toDouble());
        Histograma.addPiece("Noviembre",Qt::darkRed,mul*convapunto(ui.tabla->item(11,3)->text()).toDouble());
        Histograma.addPiece("Diciembre",Qt::darkYellow,mul*convapunto(ui.tabla->item(12,3)->text()).toDouble());
        Histograma.draw(&painter);
        Histograma.drawLegend(&painter);

        ui.sumahabergraflabel->setPixmap(pixmap);
*/
}



void acumuladosmes::botonbuscasubcuentapulsado()
{
    QString cadena2;
    buscasubcuenta *labusqueda=new buscasubcuenta(ui.subcuentalineEdit->text());
    int cod=labusqueda->exec();
    cadena2=labusqueda->seleccioncuenta();
    if (cod==QDialog::Accepted && existesubcuenta(cadena2)) ui.subcuentalineEdit->setText(cadena2);
       else ui.subcuentalineEdit->setText("");
    delete labusqueda;
}


void acumuladosmes::subcuentacambiada()
{
  QString cadena;

  if (existecodigoplan(ui.subcuentalineEdit->text(),&cadena))
     { 
       ui.titulotextLabel->setText(cadena);
       // cargadatos();
       // más bien reset datos
       resetdatos();
     } else ui.titulotextLabel->setText("");
}


void acumuladosmes::ctaexpandepunto()
{
  if (ui.subcuentalineEdit->text().contains('.'))
    ui.subcuentalineEdit->setText(expandepunto(ui.subcuentalineEdit->text(),anchocuentas()));
}


void acumuladosmes::finedicsubcuenta()
{
 ctaexpandepunto();
 /*
 if (ui.subcuentalineEdit->text().length()<anchocuentas() && ui.subcuentalineEdit->text().length()>0
     && !cod_longitud_variable())
    botonbuscasubcuentapulsado();
 if (ui.subcuentalineEdit->text().length()<4 && ui.subcuentalineEdit->text().length()>0
     && cod_longitud_variable())
    botonbuscasubcuentapulsado();*/
}


void acumuladosmes::refrescar()
{
  cargadatos();
  tabcambiada();
}

void acumuladosmes::subcuentaprevia()
{
    QString cadena=subcuentaanterior(ui.subcuentalineEdit->text());
    if (cadena=="") return;
    ui.subcuentalineEdit->setText(cadena);
    refrescar();
}


void acumuladosmes::subcuentasiguiente()
{
    QString cadena=subcuentaposterior(ui.subcuentalineEdit->text());
    if (cadena=="") return;
    ui.subcuentalineEdit->setText(cadena);
    refrescar();
}


void acumuladosmes::txtexport()
{
   QClipboard *cb = QApplication::clipboard();
   QString global;
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("acum_cuenta.txt");
   // QMessageBox::warning( this, tr("HOLA"),qfichero);
   QString pasa;
   //if (eswindows()) pasa=QFile::encodeName(qfichero);
   //    else pasa=qfichero;
    pasa=qfichero;
    QFile fichero(pasa);
    if (! fichero.open( QIODevice::WriteOnly ) ) return;
    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);
    stream << tr("CUENTA:") << "\t" << ui.subcuentalineEdit->text() << "\t"
           << ui.titulotextLabel->text() <<"\n";
    global=tr("CUENTA:") + "\t"; global+=ui.subcuentalineEdit->text(); global+="\t";
    global+=ui.titulotextLabel->text();
    global+="\n";
    stream << tr("EJERCICIO:") <<" \t" << ui.ejerciciocomboBox->currentText()  << "\n";

    global+=tr("EJERCICIO") +": \t"; global+=ui.ejerciciocomboBox->currentText(); global+="\n";

    stream << tr("MES") << ": \t" << tr("AÑO") << ": \t" << tr("SUMA DEBE");
    stream << ": \t" << tr("SUMA HABER") << ": \t" << tr("SALDO") << "\n";

    global+=tr("MES"); global+=": \t"; global+=tr("AÑO"); global+=": \t"; global+=tr("SUMA DEBE");
    global+=": \t"; global+=tr("SUMA HABER"); global+=": \t"; global+=tr("SALDO"); global+="\n";

    for (int veces=0;veces<ui.tabla->rowCount();veces++)
       {
          QString cad;
          if (ui.tabla->item(veces,0)!=0) cad=ui.tabla->item(veces,0)->text(); else cad="";
          stream << cad << "\t";
          global+= cad + "\t";
          if (ui.tabla->item(veces,1)!=0) cad=ui.tabla->item(veces,1)->text(); else cad="";
          stream << cad << "\t";
          global+= cad + "\t";
          if (ui.tabla->item(veces,2)!=0) cad=ui.tabla->item(veces,2)->text(); else cad="";
          stream << cad << "\t";
          global+= cad + "\t";
          if (ui.tabla->item(veces,3)!=0) cad=ui.tabla->item(veces,3)->text(); else cad="";
          stream << cad << "\t";
          global+= cad + "\t";
          if (ui.tabla->item(veces,4)!=0) cad=ui.tabla->item(veces,4)->text(); else cad="";
          stream << cad << "\n";
          global+= cad + "\n";

       }
    fichero.close();
    cb->setText(global);
    QMessageBox::information( this, tr("ACUMULADOS MES"),
			      tr("Se ha pasado el contenido al portapales"));

}

void acumuladosmes::resetdatos()
{
  ui.tabla->setRowCount(0);
  /*for (int veces=0; veces<ui.tabla->rowCount();veces++)
      for (int veces2=0; veces<ui.tabla->columnCount(); veces2++)
        if (ui.tabla->item(veces,veces2)!=NULL) ui.tabla->item(veces,veces2)->setText("");*/
  //QPixmap foto;
  //ui.sumadebegraflabel->setPixmap(foto);
  //ui.sumahabergraflabel->setPixmap(foto);
  //ui.saldosgraflabel->setPixmap(foto);

}


void acumuladosmes::generalatex()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("acumulados.tex");
   QString pasa;
   //if (eswindows()) pasa=QFile::encodeName(qfichero);
   //    else pasa=qfichero;
   pasa=qfichero;
   QFile fichero(pasa);
    if (! fichero.open( QIODevice::WriteOnly ) ) return;
    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);
    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {";
    stream << filtracad(nombreempresa()) << "}}";
    stream << "\\end{center}\n";

    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << tr("CUENTA:") << " " << ui.subcuentalineEdit->text() << " "
            << ui.titulotextLabel->text() <<"}}\n";
    stream << "\\end{center}\n";

    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {";
    stream << tr("EJERCICIO:") <<" " << ui.ejerciciocomboBox->currentText()  << "}}\n";
    stream << "\\end{center}\n";


    stream << "\\begin{center}" << "\n";
    stream << "\\begin{tabular}{|l|l|r|r|r|}" << "\n";
    stream << "\\hline" << "\n";

    // --------------------------------------------------------------------------------------------------
    stream << "{\\textbf{" << tr("MES") << "}} & ";
    stream << "{\\textbf{" << tr("AÑO") << "}} & ";
    stream << "{\\textbf{" << tr("SUM.DEBE") << "}} & ";
    stream << "{\\textbf{" << tr("SUM.HABER") << "}} & ";
    stream << "{\\textbf{" << tr("SALDO") << "}} ";
    stream << " \\\\" << "\n";
    stream << "\\hline" << "\n";

    for (int veces=0;veces<ui.tabla->rowCount();veces++)
       {
          QString cad;
          if (ui.tabla->item(veces,0)!=0) cad=ui.tabla->item(veces,0)->text(); else cad="";
          stream << "{\\textbf{" << cad << "}} & ";;
          if (ui.tabla->item(veces,1)!=0) cad=ui.tabla->item(veces,1)->text(); else cad="";
          stream << "{\\textbf{" << cad << "}} & ";;
          if (ui.tabla->item(veces,2)!=0) cad=ui.tabla->item(veces,2)->text(); else cad="";
          if (convapunto(cad).toDouble()>-0.001 && convapunto(cad).toDouble()<0.001)
              cad.clear();
          stream << "{\\textbf{" << cad << "}} & ";;
          if (ui.tabla->item(veces,3)!=0) cad=ui.tabla->item(veces,3)->text(); else cad="";
          if (convapunto(cad).toDouble()>-0.001 && convapunto(cad).toDouble()<0.001)
              cad.clear();
          stream << "{\\textbf{" << cad << "}} & ";;
          if (ui.tabla->item(veces,4)!=0) cad=ui.tabla->item(veces,4)->text(); else cad="";
          if (convapunto(cad).toDouble()>-0.001 && convapunto(cad).toDouble()<0.001)
              cad.clear();
          stream << "{\\textbf{" << cad << "}} \\\\" << "\n";
          stream << "\\hline" << "\n";
       }

    stream << "\\end{tabular}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "% FIN_CUERPO\n";
    stream << "\\end{document}" << "\n";

    fichero.close();

}


void acumuladosmes::imprimir()
{
   generalatex();
   int valor=imprimelatex2(tr("acumulados"));
   if (valor==1)
       QMessageBox::warning( this, tr("Acumulados"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Acumulados"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Acumulados"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}

void acumuladosmes::ejercicio_cambiado()
{
 resetdatos();
}
