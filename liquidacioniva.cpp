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

#include "liquidacioniva.h"
#include "funciones.h"
#include "privilegios.h"
#include "basedatos.h"
#include "tabla_asientos.h"
#include "modelo300.h"
#include "consmayor.h"
#include <QProgressDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <qtrpt.h>

liquidacioniva::liquidacioniva(QString qusuario) : QDialog() {
    ui.setupUi(this);

 //ui.m300pushButton->hide();

  //ui.inicialdateEdit->setDate(QDate::currentDate());
  //ui.finaldateEdit->setDate(QDate::currentDate());

  QSqlQuery query = basedatos::instancia()->selectCodigoejerciciosordercodigo();
  QStringList ej;
  if ( query.isActive() ) {
          while ( query.next() )
                ej << query.value(0).toString();
          }
  ui.ejerciciocomboBox->addItems(ej);
  actufechas();

usuario=qusuario;
QStringList columnas;
 columnas << tr("Cuenta") << tr("Base imponible") << tr("Tipo");
 columnas << tr("Cuota") << tr("Total");
 columnas << tr("B.Inversión") << tr("Cuota efectiva") << tr("Cuota C.Caja");
  ui.soportadotable->setHorizontalHeaderLabels(columnas);

  ui.imp_soportadotable->setHorizontalHeaderLabels(columnas);

QStringList columnas3;
 columnas3 << tr("Cuenta") << tr("Base imponible") << tr("Tipo");
 columnas3 << tr("Cuota sop.") << tr("Cuota rep.");
 columnas3 << tr("B.Inversión");
 ui.aibtable->setHorizontalHeaderLabels(columnas3);

  QStringList columnas2;
 columnas2 << tr("Cuenta") << tr("Base imponible") << tr("Tipo");
 columnas2 << tr("Cuota") << tr("T.R.E.") << tr("R.E.") << tr("Cuota C.Caja");
  ui.repercutidotable->setHorizontalHeaderLabels(columnas2);


  QStringList columnas4;
   columnas4 << tr("Cuenta") << tr("Base imponible") << tr("Tipo");
   columnas4 << tr("Cuota sop.") << tr("Cuota rep.");
   ui.autofacturastable->setHorizontalHeaderLabels(columnas4);

   QStringList columnas5;
    columnas5 << tr("Cuenta") << tr("Base imponible") << tr("Tipo");
    columnas5 << tr("Cuota sop.") << tr("Cuota rep.");
    ui.autofacturas_exttable->setHorizontalHeaderLabels(columnas5);

    ui.autofacturas_interiorestable->setHorizontalHeaderLabels(columnas5);

    QStringList columnas6;
     columnas6 << tr("Cuenta") << tr("Base imponible") << tr("Tipo");
     columnas6 << tr("Cuota") << tr("TOTAL");
     ui.agrariotableWidget->setHorizontalHeaderLabels(columnas6);

if (!privilegios[nuev_asiento]) ui.asientopushButton->setEnabled(false);

if (conigic())
   {
    ui.textLabel4->setText(tr("IGIC SOPORTADO"));
    ui.cuotasoptextLabel->setText(tr("Cuota total IGIC soportado"));
    ui.textLabel5->setText(tr("IGIC REPERCUTIDO"));
    ui.textLabel6->setText(tr("TOTAL IGIC"));
    setWindowTitle(tr("Liquidación de IGIC"));
   }

  comadecimal=haycomadecimal(); decimales=haydecimales();
  // tabla_iva_sop *ts_caja_corr, *ts_caja_inv
  ts_caja_corr=new tabla_iva_sop(usuario,comadecimal,decimales);
  ts_caja_inv=new tabla_iva_sop(usuario,comadecimal,decimales);
  ts_caja_rep=new tabla_iva_rep(usuario,comadecimal,decimales);

  connect(ui.calcularpushButton,SIGNAL( clicked()),this,SLOT(calcular()));
  // connect(ui.imprimepushButton,SIGNAL( clicked()),this,SLOT(()));
  connect(ui.inicialdateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(fechacambiada()));
  connect(ui.finaldateEdit,SIGNAL(dateChanged(QDate)),this,SLOT(fechacambiada()));

  connect(ui.imprimepushButton,SIGNAL( clicked()),this,SLOT(imprimir()));
  connect(ui.informe_tex_pushButton,SIGNAL( clicked()),this,SLOT(imprimir_latex()));
  connect(ui.latexpushButton,SIGNAL(clicked()),SLOT(latex()));

  connect(ui.copiapushButton,SIGNAL(clicked()),SLOT(copia()));

  connect(ui.caja_corrientespushButton,SIGNAL(clicked()),SLOT(ver_iva_caja_soportado_corrientes()));
  connect(ui.caja_inversionpushButton,SIGNAL(clicked()),SLOT(ver_iva_caja_soportado_inversion()));
  connect(ui.iva_caja_reppushButton,SIGNAL(clicked()),SLOT(ver_iva_caja_repercutido()));

  connect(ui.asientopushButton,SIGNAL(clicked()),SLOT(gen_asiento()));

  connect(ui.periodocomboBox,SIGNAL(currentIndexChanged (int)),SLOT(actufechas()));
  connect(ui.ejerciciocomboBox,SIGNAL(currentIndexChanged (int)),SLOT(actufechas()));

  connect(ui.m300pushButton,SIGNAL(clicked(bool)),SLOT(mod303()));
  connect(ui.mayor_pushButton,SIGNAL(clicked(bool)),SLOT(consulta_mayor()));

  ui.concepto_liq_label->setText(" - - ");

  ui.textLabel2_5->hide();
  ui.exentas_deduclineEdit->hide();

#ifdef NOEDITTEX
  ui.latexpushButton->hide();
#endif

}

liquidacioniva::~liquidacioniva()
{
    delete(ts_caja_corr);
    delete(ts_caja_inv);
    delete(ts_caja_rep);
}

void liquidacioniva::calcular()
{
  borrar();
  
  ts_caja_corr->pasa_info_liquidacion(ui.inicialdateEdit->date(),
                                ui.finaldateEdit->date(),false);
  // QMessageBox::information( this, tr("INFO"),"primera consulta");

  ts_caja_inv->pasa_info_liquidacion(ui.inicialdateEdit->date(),
                                ui.finaldateEdit->date(),true);
  // QMessageBox::information( this, tr("INFO"),"2 consulta");

  // ----------------------------------------------------

  ts_caja_rep->pasa_info_liquidacion(ui.inicialdateEdit->date(),
                                ui.finaldateEdit->date());
  // QMessageBox::information( this, tr("INFO"),"3 consulta");

  ui.caja_corrientespushButton->setEnabled(true);
  ui.caja_inversionpushButton->setEnabled(true);
  ui.iva_caja_reppushButton->setEnabled(true);

  // QMessageBox::information( this, tr("INFO"),"inicio");


  QProgressDialog progress(tr("Calculando liquidación ..."), 0, 0, 31, this);
  progress.setMinimumDuration ( 100 );
  progress.setValue(1);
  QApplication::processEvents();


  QSqlQuery consulta = basedatos::instancia()->libroivafechas_sop_corrientes( ui.inicialdateEdit->date(),
                                                                            ui.finaldateEdit->date() );

  int fila=0;
  double suma=0;
  while (consulta.next())
      {
         ui.soportadotable->insertRow(fila);
         ui.soportadotable->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));
         QTableWidgetItem *newItem1 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(2).toDouble(),comadecimal,decimales));
         newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.soportadotable->setItem(fila,1,newItem1);

         QString tipoiva=consulta.value(1).toString();
         if (comadecimal) tipoiva=convacoma(tipoiva);
         QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
         newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.soportadotable->setItem(fila,2,newItem2);

         
         QTableWidgetItem *newItem3 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(3).toDouble(),comadecimal,decimales));
         newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.soportadotable->setItem(fila,3,newItem3);

         QTableWidgetItem *newItem4 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(4).toDouble(),comadecimal,decimales));
         newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.soportadotable->setItem(fila,4,newItem4);

         QTableWidgetItem *newItem5 = new QTableWidgetItem(QString());
         newItem5->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
         ui.soportadotable->setItem(fila,5,newItem5);

         QTableWidgetItem *newItem6 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(6).toDouble(),comadecimal,decimales));
         newItem6->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.soportadotable->setItem(fila,6,newItem6);

         ui.soportadotable->setItem(fila,7,new QTableWidgetItem(QString()));

         if (consulta.value(3).toDouble() - consulta.value(5).toDouble()>0.001)
             ui.cuotasoptextLabel->setText(tr("Cuota iva soportado corregida por prorrata/no afectación:"));
         suma+=redond(consulta.value(6).toDouble(),2);
         QApplication::processEvents();
         fila++;
      }

  // ---------------------------------------------------------
  // QMessageBox::information( this, tr("INFO"),"sop corrientes");
  // ---------------------------------------------------------
  progress.setValue(2);
  QApplication::processEvents();

  consulta = basedatos::instancia()->libroivafechas_sop_inversion( ui.inicialdateEdit->date(),
                                                                            ui.finaldateEdit->date() );
  while (consulta.next())
      {
         ui.soportadotable->insertRow(fila);
         ui.soportadotable->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));
         QTableWidgetItem *newItem1 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(2).toDouble(),comadecimal,decimales));
         newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.soportadotable->setItem(fila,1,newItem1);

         QString tipoiva=consulta.value(1).toString();
         if (comadecimal) tipoiva=convacoma(tipoiva);
         QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
         newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.soportadotable->setItem(fila,2,newItem2);


         QTableWidgetItem *newItem3 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(3).toDouble(),comadecimal,decimales));
         newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.soportadotable->setItem(fila,3,newItem3);

         QTableWidgetItem *newItem4 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(4).toDouble(),comadecimal,decimales));
         newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.soportadotable->setItem(fila,4,newItem4);

         QTableWidgetItem *newItem5 = new QTableWidgetItem("*");
         newItem5->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
         ui.soportadotable->setItem(fila,5,newItem5);

         QTableWidgetItem *newItem6 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(6).toDouble(),comadecimal,decimales));
         newItem6->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.soportadotable->setItem(fila,6,newItem6);

         ui.soportadotable->setItem(fila,7,new QTableWidgetItem(QString()));

         if (consulta.value(3).toDouble() - consulta.value(6).toDouble()>0.001)
             ui.cuotasoptextLabel->setText(tr("Cuota iva soportado corregida por prorrata/no afectación:"));
         suma+=redond(consulta.value(5).toDouble(),2);
         fila++;
         QApplication::processEvents();
      }

  // ---------------------------------------------------------
  // QMessageBox::information( this, tr("INFO"),"sop inversion");
  // ---------------------------------------------------------
  progress.setValue(3);
  QApplication::processEvents();

  // vamos con el IVA soportado de caja bienes corrientes
  QStringList selcuenta_iva, seltipo;
  QList<double> selbi,selcuota,seltotal,selcuota_efectiva,selcuota_liquidada;

  ts_caja_corr->calcula_listas_sel();

  selcuenta_iva=ts_caja_corr->qselcuenta_iva();
  seltipo=ts_caja_corr->qseltipo();
  selbi=ts_caja_corr->qselbi();
  selcuota=ts_caja_corr->qselcuota();
  seltotal=ts_caja_corr->qseltotal();
  selcuota_efectiva=ts_caja_corr->qselcuota_efectiva();
  selcuota_liquidada=ts_caja_corr->qselcuota_liquidada();

  for (int veces=0; veces<selcuenta_iva.count(); veces++)
    {
      ui.soportadotable->insertRow(fila);
      ui.soportadotable->setItem(fila,0,new QTableWidgetItem(selcuenta_iva.at(veces)));
      QTableWidgetItem *newItem1 = new QTableWidgetItem(
                   formatea_redondeado(selbi.at(veces),comadecimal,decimales));
      newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.soportadotable->setItem(fila,1,newItem1);

      QString tipoiva=seltipo.at(veces);
      if (comadecimal) tipoiva=convacoma(tipoiva);
      QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
      newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.soportadotable->setItem(fila,2,newItem2);


      QTableWidgetItem *newItem3 = new QTableWidgetItem(
                   formatea_redondeado(selcuota.at(veces),comadecimal,decimales));
      newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.soportadotable->setItem(fila,3,newItem3);

      QTableWidgetItem *newItem4 = new QTableWidgetItem(
                   formatea_redondeado(seltotal.at(veces),comadecimal,decimales));
      newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.soportadotable->setItem(fila,4,newItem4);

      QTableWidgetItem *newItem5 = new QTableWidgetItem(""); // no es inversión
      newItem5->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
      ui.soportadotable->setItem(fila,5,newItem5);

      QTableWidgetItem *newItem6 = new QTableWidgetItem(
                   formatea_redondeado(selcuota_efectiva.at(veces),comadecimal,decimales));
      newItem6->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.soportadotable->setItem(fila,6,newItem6);

      QTableWidgetItem *newItem7 = new QTableWidgetItem(
                   formatea_redondeado(selcuota_liquidada.at(veces),comadecimal,decimales));
      newItem7->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.soportadotable->setItem(fila,7,newItem7);

      if (selcuota.at(veces) - selcuota_efectiva.at(veces)>0.001)
          ui.cuotasoptextLabel->setText(tr("Cuota iva soportado corregida por prorrata/no afectación:"));
      suma+=redond(selcuota_liquidada.at(veces),2);
      fila++;
      QApplication::processEvents();
    }

  progress.setValue(4);
  QApplication::processEvents();

  // vamos con el IVA soportado de caja bienes inversion
  selcuenta_iva.clear();
  seltipo.clear();
  selbi.clear();
  selcuota.clear();
  seltotal.clear();
  selcuota_efectiva.clear();
  selcuota_liquidada.clear();

  ts_caja_inv->calcula_listas_sel();

  selcuenta_iva=ts_caja_inv->qselcuenta_iva();
  seltipo=ts_caja_inv->qseltipo();
  selbi=ts_caja_inv->qselbi();
  selcuota=ts_caja_inv->qselcuota();
  seltotal=ts_caja_inv->qseltotal();
  selcuota_efectiva=ts_caja_inv->qselcuota_efectiva();
  selcuota_liquidada=ts_caja_inv->qselcuota_liquidada();

  for (int veces=0; veces<selcuenta_iva.count(); veces++)
    {
      ui.soportadotable->insertRow(fila);
      ui.soportadotable->setItem(fila,0,new QTableWidgetItem(selcuenta_iva.at(veces)));
      QTableWidgetItem *newItem1 = new QTableWidgetItem(
                   formatea_redondeado(selbi.at(veces),comadecimal,decimales));
      newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.soportadotable->setItem(fila,1,newItem1);

      QString tipoiva=seltipo.at(veces);
      if (comadecimal) tipoiva=convacoma(tipoiva);
      QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
      newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.soportadotable->setItem(fila,2,newItem2);


      QTableWidgetItem *newItem3 = new QTableWidgetItem(
                   formatea_redondeado(selcuota.at(veces),comadecimal,decimales));
      newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.soportadotable->setItem(fila,3,newItem3);

      QTableWidgetItem *newItem4 = new QTableWidgetItem(
                   formatea_redondeado(seltotal.at(veces),comadecimal,decimales));
      newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.soportadotable->setItem(fila,4,newItem4);

      QTableWidgetItem *newItem5 = new QTableWidgetItem("*"); // es inversión
      newItem5->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
      ui.soportadotable->setItem(fila,5,newItem5);

      QTableWidgetItem *newItem6 = new QTableWidgetItem(
                   formatea_redondeado(selcuota_efectiva.at(veces),comadecimal,decimales));
      newItem6->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.soportadotable->setItem(fila,6,newItem6);

      QTableWidgetItem *newItem7 = new QTableWidgetItem(
                   formatea_redondeado(selcuota_liquidada.at(veces),comadecimal,decimales));
      newItem7->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.soportadotable->setItem(fila,7,newItem7);

      if (selcuota.at(veces) - selcuota_efectiva.at(veces)>0.001)
          ui.cuotasoptextLabel->setText(tr("Cuota iva soportado corregida por prorrata/no afectación:"));
      suma+=redond(selcuota_liquidada.at(veces),2);
      fila++;
      QApplication::processEvents();
    }



  progress.setValue(5);
  QApplication::processEvents();

  // calculamos ahora IVA soportado al tipo 0 (exento) e insertamos fila

    consulta = basedatos::instancia()->baserecibidasexentas_corrientes(
          ui.inicialdateEdit->date() , ui.finaldateEdit->date() );

 if (consulta.next() && !(consulta.value(0).toDouble()<0.0001 && consulta.value(0).toDouble()>-0.0001))
  {
    ui.soportadotable->insertRow(fila);
    ui.soportadotable->setItem(fila,0,new QTableWidgetItem(tr("B.I. al tipo 0")));
    QTableWidgetItem *newItem1 = new QTableWidgetItem(
                   formatea_redondeado(consulta.value(0).toDouble(),comadecimal,decimales));
    newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
    ui.soportadotable->setItem(fila,1,newItem1);

    QTableWidgetItem *newItem2 = new QTableWidgetItem("");
    ui.soportadotable->setItem(fila,2,newItem2);

    QTableWidgetItem *newItem3 = new QTableWidgetItem("");
    ui.soportadotable->setItem(fila,3,newItem3);

    QTableWidgetItem *newItem4 = new QTableWidgetItem("");
    ui.soportadotable->setItem(fila,4,newItem4);

    QTableWidgetItem *newItem5 = new QTableWidgetItem(QString());
    newItem5->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
    ui.soportadotable->setItem(fila,5,newItem5);

    ui.soportadotable->setItem(fila,6,new QTableWidgetItem(QString()));
    ui.soportadotable->setItem(fila,7,new QTableWidgetItem(QString()));

    fila++;
    QApplication::processEvents();
  }

 progress.setValue(6);
 QApplication::processEvents();

  // calculamos ahora IVA soportado al tipo 0 (exento) de INVERSIONES e insertamos fila

    consulta = basedatos::instancia()->baserecibidasexentas_inversion(
          ui.inicialdateEdit->date() , ui.finaldateEdit->date() );

 if (consulta.next() && !(consulta.value(0).toDouble()<0.0001 && consulta.value(0).toDouble()>-0.0001))
  {
    ui.soportadotable->insertRow(fila);
    ui.soportadotable->setItem(fila,0,new QTableWidgetItem(tr("B.I. al tipo 0")));
    QTableWidgetItem *newItem1 = new QTableWidgetItem(
                   formatea_redondeado(consulta.value(0).toDouble(),comadecimal,decimales));
    newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
    ui.soportadotable->setItem(fila,1,newItem1);

    QTableWidgetItem *newItem2 = new QTableWidgetItem("");
    ui.soportadotable->setItem(fila,2,newItem2);

    QTableWidgetItem *newItem3 = new QTableWidgetItem("");
    ui.soportadotable->setItem(fila,3,newItem3);

    QTableWidgetItem *newItem4 = new QTableWidgetItem("");
    ui.soportadotable->setItem(fila,4,newItem4);

    QTableWidgetItem *newItem5 = new QTableWidgetItem("*");
    newItem5->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
    ui.soportadotable->setItem(fila,5,newItem5);
    ui.soportadotable->setItem(fila,6,new QTableWidgetItem(QString()));
    ui.soportadotable->setItem(fila,7,new QTableWidgetItem(QString()));

    fila++;
  }

  double sumaliquidacion=0;
  if (suma>0.001 || suma<-0.001)
     {
      ui.cuotalineEdit->setText(formatea_redondeado(suma,comadecimal,decimales));
      sumaliquidacion+=suma;
     }

  // --------------------------------------------------------------------------------------
  // ---------------soportando en importación----------------------------------------------
  progress.setValue(7);
  QApplication::processEvents();

  consulta = basedatos::instancia()->libroivafechas_sop_corrientes_importacion
                 ( ui.inicialdateEdit->date(),
                  ui.finaldateEdit->date() );
  fila=0;
  suma=0;
  while (consulta.next())
      {
         ui.imp_soportadotable->insertRow(fila);
         ui.imp_soportadotable->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));
         QTableWidgetItem *newItem1 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(2).toDouble(),comadecimal,decimales));
         newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.imp_soportadotable->setItem(fila,1,newItem1);

         QString tipoiva=consulta.value(1).toString();
         if (comadecimal) tipoiva=convacoma(tipoiva);
         QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
         newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.imp_soportadotable->setItem(fila,2,newItem2);


         QTableWidgetItem *newItem3 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(3).toDouble(),comadecimal,decimales));
         newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.imp_soportadotable->setItem(fila,3,newItem3);

         QTableWidgetItem *newItem4 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(4).toDouble(),comadecimal,decimales));
         newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.imp_soportadotable->setItem(fila,4,newItem4);

         QTableWidgetItem *newItem5 = new QTableWidgetItem(QString());
         newItem5->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
         ui.imp_soportadotable->setItem(fila,5,newItem5);

         QTableWidgetItem *newItem6 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(6).toDouble(),comadecimal,decimales));
         newItem6->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.imp_soportadotable->setItem(fila,6,newItem6);


         if (consulta.value(3).toDouble() - consulta.value(5).toDouble()>0.001)
             ui.cuotasop_imptextLabel->setText(tr("Cuota iva soportado corregida por prorrata/no afectación:"));
         suma+=redond(consulta.value(6).toDouble(),2);
         fila++;
         QApplication::processEvents();
      }

  progress.setValue(8);
  QApplication::processEvents();


  consulta = basedatos::instancia()->libroivafechas_sop_inversion_importacion(
          ui.inicialdateEdit->date(),
          ui.finaldateEdit->date() );
  while (consulta.next())
      {
         ui.imp_soportadotable->insertRow(fila);
         ui.imp_soportadotable->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));
         QTableWidgetItem *newItem1 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(2).toDouble(),comadecimal,decimales));
         newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.imp_soportadotable->setItem(fila,1,newItem1);

         QString tipoiva=consulta.value(1).toString();
         if (comadecimal) tipoiva=convacoma(tipoiva);
         QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
         newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.imp_soportadotable->setItem(fila,2,newItem2);


         QTableWidgetItem *newItem3 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(3).toDouble(),comadecimal,decimales));
         newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.imp_soportadotable->setItem(fila,3,newItem3);

         QTableWidgetItem *newItem4 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(4).toDouble(),comadecimal,decimales));
         newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.imp_soportadotable->setItem(fila,4,newItem4);

         QTableWidgetItem *newItem5 = new QTableWidgetItem("*");
         newItem5->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
         ui.imp_soportadotable->setItem(fila,5,newItem5);

         QTableWidgetItem *newItem6 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(6).toDouble(),comadecimal,decimales));
         newItem6->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.imp_soportadotable->setItem(fila,6,newItem6);

         if (consulta.value(3).toDouble() - consulta.value(5).toDouble()>0.001)
             ui.cuotasop_imptextLabel->setText(tr("Cuota iva soportado corregida por prorrata/no afectación:"));
         suma+=redond(consulta.value(6).toDouble(),2);
         fila++;
         QApplication::processEvents();
      }

  progress.setValue(9);
  QApplication::processEvents();

  // calculamos ahora IVA soportado al tipo 0 (exento) e insertamos fila

    consulta = basedatos::instancia()->baserecibidasexentas_corrientes_importacion(
          ui.inicialdateEdit->date() , ui.finaldateEdit->date() );

 if (consulta.next() && !(consulta.value(0).toDouble()<0.0001 && consulta.value(0).toDouble()>-0.0001))
  {
    ui.imp_soportadotable->insertRow(fila);
    ui.imp_soportadotable->setItem(fila,0,new QTableWidgetItem(tr("B.I. al tipo 0")));
    QTableWidgetItem *newItem1 = new QTableWidgetItem(
                   formatea_redondeado(consulta.value(0).toDouble(),comadecimal,decimales));
    newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
    ui.imp_soportadotable->setItem(fila,1,newItem1);

    QTableWidgetItem *newItem2 = new QTableWidgetItem("");
    ui.imp_soportadotable->setItem(fila,2,newItem2);

    QTableWidgetItem *newItem3 = new QTableWidgetItem("");
    ui.imp_soportadotable->setItem(fila,3,newItem3);

    QTableWidgetItem *newItem4 = new QTableWidgetItem("");
    ui.imp_soportadotable->setItem(fila,4,newItem4);

    QTableWidgetItem *newItem5 = new QTableWidgetItem(QString());
    newItem5->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
    ui.imp_soportadotable->setItem(fila,5,newItem5);
    fila++;
    QApplication::processEvents();
  }

 progress.setValue(10);
 QApplication::processEvents();

  // calculamos ahora IVA soportado al tipo 0 (exento) de INVERSIONES e insertamos fila

    consulta = basedatos::instancia()->baserecibidasexentas_inversion_importacion(
          ui.inicialdateEdit->date() , ui.finaldateEdit->date() );

 if (consulta.next() && !(consulta.value(0).toDouble()<0.0001 && consulta.value(0).toDouble()>-0.0001))
  {
    ui.imp_soportadotable->insertRow(fila);
    ui.imp_soportadotable->setItem(fila,0,new QTableWidgetItem(tr("B.I. al tipo 0")));
    QTableWidgetItem *newItem1 = new QTableWidgetItem(
                   formatea_redondeado(consulta.value(0).toDouble(),comadecimal,decimales));
    newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
    ui.imp_soportadotable->setItem(fila,1,newItem1);

    QTableWidgetItem *newItem2 = new QTableWidgetItem("");
    ui.imp_soportadotable->setItem(fila,2,newItem2);

    QTableWidgetItem *newItem3 = new QTableWidgetItem("");
    ui.imp_soportadotable->setItem(fila,3,newItem3);

    QTableWidgetItem *newItem4 = new QTableWidgetItem("");
    ui.imp_soportadotable->setItem(fila,4,newItem4);

    QTableWidgetItem *newItem5 = new QTableWidgetItem("*");
    newItem5->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
    ui.imp_soportadotable->setItem(fila,5,newItem5);
    fila++;
    QApplication::processEvents();
  }

  if (suma>0.001 || suma<-0.001)
     {
      ui.cuota_implineEdit->setText(formatea_redondeado(suma,comadecimal,decimales));
      sumaliquidacion+=suma;
     }

  progress.setValue(11);
  QApplication::processEvents();


  // AQUÍ TERMINA SOPORTADO EN IMPORTACIONES
  // --------------------------------------------------------------------------------------



  // ---------------------------------------------------------------------------------------
  //   REPERCUTIDO
  // ---------------------------------------------------------------------------------------
  progress.setValue(12);
  QApplication::processEvents();

  fila=0;

  consulta = basedatos::instancia()->select7DiarioLibroivafechasgroupcuentatipo_ivatipo_re(
          ui.inicialdateEdit->date() , ui.finaldateEdit->date() );

  while (consulta.next())
      {
         ui.repercutidotable->insertRow(fila);
         ui.repercutidotable->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));
         QTableWidgetItem *newItem1 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(2).toDouble(),comadecimal,decimales)); // cuota total - re
         newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.repercutidotable->setItem(fila,1,newItem1);

         QString tipoiva=consulta.value(1).toString(); // tipoiva
         if (comadecimal) tipoiva=convacoma(tipoiva);
         QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
         newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.repercutidotable->setItem(fila,2,newItem2);

         double num;
         num=consulta.value(7).toDouble()-consulta.value(5).toDouble();
         QTableWidgetItem *newItem3 = new QTableWidgetItem(
                      formatea_redondeado(num,comadecimal,decimales));
         newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.repercutidotable->setItem(fila,3,newItem3);

         QString tipore=consulta.value(4).toString();
         if (comadecimal) tipore=convacoma(tipore);
         QTableWidgetItem *newItem4 = new QTableWidgetItem(tipore);
         newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.repercutidotable->setItem(fila,4,newItem4); // tipo re

         QTableWidgetItem *newItem5 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(5).toDouble(),comadecimal,decimales)); // cuota_re
         newItem5->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.repercutidotable->setItem(fila,5,newItem5); // cuota re
         ui.repercutidotable->setItem(fila,6,new QTableWidgetItem("")); // cuota caja vacía
         fila++;
         QApplication::processEvents();
      }

  // QMessageBox::information( this, tr("INFO"),"*********");

  // ---------------------------------------------------------------------------------
  // Repercutido, criterio de caja
  progress.setValue(13);
  QApplication::processEvents();
  selcuenta_iva.clear();
  seltipo.clear();
  selbi.clear();
  selcuota.clear();
  seltotal.clear();
  selcuota_liquidada.clear();

  ts_caja_rep->calcula_listas_sel();

  selcuenta_iva=ts_caja_rep->qselcuenta_iva();
  seltipo=ts_caja_rep->qseltipo();
  selbi=ts_caja_rep->qselbi();
  selcuota=ts_caja_rep->qselcuota();
  seltotal=ts_caja_rep->qseltotal();
  selcuota_liquidada=ts_caja_rep->qselcuota_liquidada();

  for (int veces=0; veces<selcuenta_iva.count(); veces++)
    {
      if (convapunto(seltipo.at(veces)).toDouble()<0.0001) continue;
      ui.repercutidotable->insertRow(fila);
      ui.repercutidotable->setItem(fila,0,new QTableWidgetItem(selcuenta_iva.at(veces)));
      QTableWidgetItem *newItem1 = new QTableWidgetItem(
                   formatea_redondeado(selbi.at(veces),comadecimal,decimales));
      newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.repercutidotable->setItem(fila,1,newItem1);

      QString tipoiva=seltipo.at(veces);
      if (comadecimal) tipoiva=convacoma(tipoiva);
      QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
      newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.repercutidotable->setItem(fila,2,newItem2);


      QTableWidgetItem *newItem3 = new QTableWidgetItem(
                   formatea_redondeado(selcuota.at(veces),comadecimal,decimales));
      newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.repercutidotable->setItem(fila,3,newItem3);

      ui.repercutidotable->setItem(fila,4,new QTableWidgetItem(""));
      ui.repercutidotable->setItem(fila,5,new QTableWidgetItem(""));

      QTableWidgetItem *newItem7 = new QTableWidgetItem(
                   formatea_redondeado(selcuota_liquidada.at(veces),comadecimal,decimales));
      newItem7->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
      ui.repercutidotable->setItem(fila,6,newItem7);

      fila++;
      QApplication::processEvents();
    }

  progress.setValue(14);
  QApplication::processEvents();

  // -----------------------------------------------------------------------------
  suma=0;
  double suma2=0;
  for (int veces=0;veces<ui.repercutidotable->rowCount();veces++)
    {
      QString cuota=convapunto(ui.repercutidotable->item(veces,3)->text());
      QString cuota_caja=convapunto(ui.repercutidotable->item(veces,6)->text());
      suma+= cuota_caja.isEmpty() ? cuota.toDouble() : cuota_caja.toDouble();
      suma2+=convapunto(ui.repercutidotable->item(veces,5)->text()).toDouble();
    }
  if (suma>0.001 || suma<-0.001)
    {
      ui.ivalineEdit->setText(formatea_redondeado(suma,comadecimal,decimales));
      sumaliquidacion-=suma;
    }
  if (suma2>0.001 || suma2<-0.001)
    {
      ui.relineEdit->setText(formatea_redondeado(suma2,comadecimal,decimales));
      sumaliquidacion-=suma2;
    }
  // for (int veces=1;veces<5;veces++) soportadotable->adjustColumn(veces);
  // for (int veces=1;veces<6;veces++) repercutidotable->adjustColumn(veces);

  // Base imponible exenta de emitidas no eib
  /*----------------------------------------------------------------------------
    consulta = basedatos::instancia()->baseemitidasexentasnoeib (
          ui.inicialdateEdit->date() , ui.finaldateEdit->date() );
   if (consulta.next() && !(consulta.value(0).toDouble()<0.0001 && consulta.value(0).toDouble()>-0.0001))
   {
    ui.repercutidotable->insertRow(fila);
    ui.repercutidotable->setItem(fila,0,new QTableWidgetItem(tr("B.I. al tipo 0")));
    QTableWidgetItem *newItem1 = new QTableWidgetItem(
                   formatea_redondeado(consulta.value(0).toDouble(),comadecimal,decimales));
    newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
    ui.repercutidotable->setItem(fila,1,newItem1);

    QTableWidgetItem *newItem2 = new QTableWidgetItem("");
    ui.repercutidotable->setItem(fila,2,newItem2);

    QTableWidgetItem *newItem3 = new QTableWidgetItem("");
    ui.repercutidotable->setItem(fila,3,newItem3);

    QTableWidgetItem *newItem4 = new QTableWidgetItem("");
    ui.repercutidotable->setItem(fila,4,newItem4);

    QTableWidgetItem *newItem5 = new QTableWidgetItem(QString());
    newItem5->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
    ui.repercutidotable->setItem(fila,5,newItem5);
   }
  ------------------------------------------------------------------------------------------*/
  progress.setValue(15);
  QApplication::processEvents();

  consulta = basedatos::instancia()->libroiva_aib_corrientes( ui.inicialdateEdit->date() ,
                                                              ui.finaldateEdit->date() );
  fila=0;

  while (consulta.next())
      {
         ui.aibtable->insertRow(fila);
         ui.aibtable->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));

         QTableWidgetItem *newItem1 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(2).toDouble(),comadecimal,decimales));
         newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.aibtable->setItem(fila,1,newItem1);

         QString tipoiva=consulta.value(1).toString();
         if (comadecimal) tipoiva=convacoma(tipoiva);
         QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
         newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.aibtable->setItem(fila,2,newItem2);

         QTableWidgetItem *newItem3 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(3).toDouble(),comadecimal,decimales));
         sumaliquidacion+=consulta.value(3).toDouble();
         newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.aibtable->setItem(fila,3,newItem3);
         QTableWidgetItem *newItem4 = new QTableWidgetItem("");
         newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.aibtable->setItem(fila,4,newItem4);
         QTableWidgetItem *newItem5 = new QTableWidgetItem("");
         newItem5->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
         ui.aibtable->setItem(fila,5,newItem5);
         fila++;
      }

  // ----------------------------------------------------------------------------------------------
  progress.setValue(16);
  QApplication::processEvents();

  consulta = basedatos::instancia()->libroiva_aib_inversion( ui.inicialdateEdit->date() ,
                                                              ui.finaldateEdit->date() );

  while (consulta.next())
      {
         ui.aibtable->insertRow(fila);
         ui.aibtable->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));

         QTableWidgetItem *newItem1 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(2).toDouble(),comadecimal,decimales));
         newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.aibtable->setItem(fila,1,newItem1);

         QString tipoiva=consulta.value(1).toString();
         if (comadecimal) tipoiva=convacoma(tipoiva);
         QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
         newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.aibtable->setItem(fila,2,newItem2);

         QTableWidgetItem *newItem3 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(3).toDouble(),comadecimal,decimales));
         sumaliquidacion+=consulta.value(3).toDouble();
         newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.aibtable->setItem(fila,3,newItem3);
         QTableWidgetItem *newItem4 = new QTableWidgetItem("");
         newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.aibtable->setItem(fila,4,newItem4);
         QTableWidgetItem *newItem5 = new QTableWidgetItem("*");
         newItem5->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
         ui.aibtable->setItem(fila,5,newItem5);
         fila++;
      }

  progress.setValue(17);
  QApplication::processEvents();

  // ----------------------------------------------------------------------------------------------
  // Esto es para la parte de repercutido de las aib
  consulta = basedatos::instancia()->select4DiarioLibroivafechasgroupcuentatipo_ivasinprorrata(
          ui.inicialdateEdit->date() , ui.finaldateEdit->date() );

  while (consulta.next())
      {
         ui.aibtable->insertRow(fila);
         ui.aibtable->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));

         QTableWidgetItem *newItem1 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(2).toDouble(),comadecimal,decimales));
         newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.aibtable->setItem(fila,1,newItem1);

         QString tipoiva=consulta.value(1).toString();
         if (comadecimal) tipoiva=convacoma(tipoiva);
         QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
         newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.aibtable->setItem(fila,2,newItem2);

         QTableWidgetItem *newItem3 = new QTableWidgetItem("");
         newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.aibtable->setItem(fila,3,newItem3);
         QTableWidgetItem *newItem4 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(3).toDouble(),comadecimal,decimales));
         sumaliquidacion-=consulta.value(3).toDouble();
         newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.aibtable->setItem(fila,4,newItem4);
         QTableWidgetItem *newItem5 = new QTableWidgetItem("");
         newItem5->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
         ui.aibtable->setItem(fila,5,newItem5);
         fila++;
      }

  // AIB exentas a incluir aquí
  progress.setValue(18);
  QApplication::processEvents();

    consulta = basedatos::instancia()->baseemitidasexentasaib_corrientes (
          ui.inicialdateEdit->date() , ui.finaldateEdit->date() );
   if (consulta.next() && !(consulta.value(0).toDouble()<0.0001 && consulta.value(0).toDouble()>-0.0001))
   {
    ui.aibtable->insertRow(fila);
    ui.aibtable->setItem(fila,0,new QTableWidgetItem(tr("B.I. al tipo 0")));
    QTableWidgetItem *newItem1 = new QTableWidgetItem(
                   formatea_redondeado(consulta.value(0).toDouble(),comadecimal,decimales));
    newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
    ui.aibtable->setItem(fila,1,newItem1);

    QTableWidgetItem *newItem2 = new QTableWidgetItem("");
    ui.aibtable->setItem(fila,2,newItem2);

    QTableWidgetItem *newItem3 = new QTableWidgetItem("");
    ui.aibtable->setItem(fila,3,newItem3);

    QTableWidgetItem *newItem4 = new QTableWidgetItem("");
    ui.aibtable->setItem(fila,4,newItem4);

    QTableWidgetItem *newItem5 = new QTableWidgetItem("");
    ui.aibtable->setItem(fila,5,newItem5);
   }



  // AIB exentas inversión a incluir aquí
   progress.setValue(19);
   QApplication::processEvents();

    consulta = basedatos::instancia()->baseemitidasexentasaib_inversion (
          ui.inicialdateEdit->date() , ui.finaldateEdit->date() );
   if (consulta.next() && !(consulta.value(0).toDouble()<0.0001 && consulta.value(0).toDouble()>-0.0001))
   {
    ui.aibtable->insertRow(fila);
    ui.aibtable->setItem(fila,0,new QTableWidgetItem(tr("B.I. al tipo 0")));
    QTableWidgetItem *newItem1 = new QTableWidgetItem(
                   formatea_redondeado(consulta.value(0).toDouble(),comadecimal,decimales));
    newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
    ui.aibtable->setItem(fila,1,newItem1);

    QTableWidgetItem *newItem2 = new QTableWidgetItem("");
    ui.aibtable->setItem(fila,2,newItem2);

    QTableWidgetItem *newItem3 = new QTableWidgetItem("");
    ui.aibtable->setItem(fila,3,newItem3);

    QTableWidgetItem *newItem4 = new QTableWidgetItem("");
    ui.aibtable->setItem(fila,4,newItem4);

    QTableWidgetItem *newItem5 = new QTableWidgetItem("*");
    ui.aibtable->setItem(fila,5,newItem5);
   }


  // -------------------------------------------------------------------------------------
  // suma base eib
  consulta = basedatos::instancia()->selectSumbase_ivalibroivadiariofechas( ui.inicialdateEdit->date() ,
                                                                            ui.finaldateEdit->date() );
  fila=0;
  if (consulta.next())  ui.eiblineEdit->setText(formatea_redondeado(consulta.value(0).toDouble(),
                                                 comadecimal,decimales));
  progress.setValue(20);
  QApplication::processEvents();

  // llenamos cuadro autofacturas ue
  consulta = basedatos::instancia()->libroiva_autofacturas_ue( ui.inicialdateEdit->date() ,
                                                              ui.finaldateEdit->date() );
  while (consulta.next())
      {
         ui.autofacturastable->insertRow(fila);
         ui.autofacturastable->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));

         QTableWidgetItem *newItem1 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(2).toDouble(),comadecimal,decimales));
         newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.autofacturastable->setItem(fila,1,newItem1);

         QString tipoiva=consulta.value(1).toString();
         if (comadecimal) tipoiva=convacoma(tipoiva);
         QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
         newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.autofacturastable->setItem(fila,2,newItem2);

         QTableWidgetItem *newItem3 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(3).toDouble(),comadecimal,decimales));
         sumaliquidacion+=consulta.value(3).toDouble();
         newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.autofacturastable->setItem(fila,3,newItem3);
         QTableWidgetItem *newItem4 = new QTableWidgetItem("");
         newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.autofacturastable->setItem(fila,4,newItem4);
         fila++;
      }

  // llenamos cuadro autofacturas ue parte repercutido
  consulta = basedatos::instancia()->libroiva_autofacturas_ue_emitidas( ui.inicialdateEdit->date(),
                                                              ui.finaldateEdit->date() );
  progress.setValue(21);
  QApplication::processEvents();

  while (consulta.next())
      {
         ui.autofacturastable->insertRow(fila);
         ui.autofacturastable->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));

         QTableWidgetItem *newItem1 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(2).toDouble(),comadecimal,decimales));
         newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.autofacturastable->setItem(fila,1,newItem1);

         QString tipoiva=consulta.value(1).toString();
         if (comadecimal) tipoiva=convacoma(tipoiva);
         QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
         newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.autofacturastable->setItem(fila,2,newItem2);

         QTableWidgetItem *newItem3 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(3).toDouble(),comadecimal,decimales));
         sumaliquidacion-=consulta.value(3).toDouble();
         newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.autofacturastable->setItem(fila,4,newItem3);
         QTableWidgetItem *newItem4 = new QTableWidgetItem("");
         newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.autofacturastable->setItem(fila,3,newItem4);
         fila++;
      }


  // calculamos ahora IVA soportado por autofacturas al tipo 0 (exento) e insertamos fila
    consulta = basedatos::instancia()->baseautofacturasrecibidasexentas(
          ui.inicialdateEdit->date() , ui.finaldateEdit->date() );

    progress.setValue(22);
    QApplication::processEvents();

 if (consulta.next() && !(consulta.value(0).toDouble()<0.0001 && consulta.value(0).toDouble()>-0.0001))
  {
    ui.autofacturastable->insertRow(fila);
    ui.autofacturastable->setItem(fila,0,new QTableWidgetItem(tr("Autofacturas UE al tipo 0")));
    QTableWidgetItem *newItem1 = new QTableWidgetItem(
                   formatea_redondeado(consulta.value(0).toDouble(),comadecimal,decimales));
    newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
    ui.autofacturastable->setItem(fila,1,newItem1);

    QTableWidgetItem *newItem2 = new QTableWidgetItem("");
    ui.autofacturastable->setItem(fila,2,newItem2);

    QTableWidgetItem *newItem3 = new QTableWidgetItem("");
    ui.autofacturastable->setItem(fila,3,newItem3);

    QTableWidgetItem *newItem4 = new QTableWidgetItem("");
    ui.autofacturastable->setItem(fila,4,newItem4);
    // QMessageBox::information( this, tr("INFO"),consulta.value(0).toString());

    QTableWidgetItem *newItem5 = new QTableWidgetItem(QString());
    newItem5->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
    ui.autofacturastable->setItem(fila,5,newItem5);
    fila++;
  }

  // Autofacturas no ue

  fila=0;
  progress.setValue(23);
  QApplication::processEvents();

  // llenamos cuadro autofacturas fuera ue
  consulta = basedatos::instancia()->libroiva_autofacturas_no_ue( ui.inicialdateEdit->date() ,
                                                              ui.finaldateEdit->date() );
  while (consulta.next())
      {
         ui.autofacturas_exttable->insertRow(fila);
         ui.autofacturas_exttable->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));

         QTableWidgetItem *newItem1 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(2).toDouble(),comadecimal,decimales));
         newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.autofacturas_exttable->setItem(fila,1,newItem1);

         QString tipoiva=consulta.value(1).toString();
         if (comadecimal) tipoiva=convacoma(tipoiva);
         QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
         newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.autofacturas_exttable->setItem(fila,2,newItem2);

         QTableWidgetItem *newItem3 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(3).toDouble(),comadecimal,decimales));
         sumaliquidacion+=consulta.value(3).toDouble();
         newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.autofacturas_exttable->setItem(fila,3,newItem3);
         QTableWidgetItem *newItem4 = new QTableWidgetItem("");
         newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.autofacturas_exttable->setItem(fila,4,newItem4);
         fila++;
      }

  // llenamos cuadro autofacturas fuera ue parte repercutido
  consulta = basedatos::instancia()->libroiva_autofacturas_no_ue_emitidas( ui.inicialdateEdit->date(),
                                                              ui.finaldateEdit->date() );
  progress.setValue(24);
  QApplication::processEvents();
  while (consulta.next())
      {
         ui.autofacturas_exttable->insertRow(fila);
         ui.autofacturas_exttable->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));

         QTableWidgetItem *newItem1 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(2).toDouble(),comadecimal,decimales));
         newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.autofacturas_exttable->setItem(fila,1,newItem1);

         QString tipoiva=consulta.value(1).toString();
         if (comadecimal) tipoiva=convacoma(tipoiva);
         QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
         newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.autofacturas_exttable->setItem(fila,2,newItem2);

         QTableWidgetItem *newItem3 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(3).toDouble(),comadecimal,decimales));
         sumaliquidacion-=consulta.value(3).toDouble();
         newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.autofacturas_exttable->setItem(fila,4,newItem3);
         QTableWidgetItem *newItem4 = new QTableWidgetItem("");
         newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.autofacturas_exttable->setItem(fila,3,newItem4);
         fila++;
      }

  progress.setValue(25);
  QApplication::processEvents();


  // calculamos ahora IVA soportado por autofacturas no ue al tipo 0 (exento) e insertamos fila
    consulta = basedatos::instancia()->baseautofacturasnouerecibidasexentas(
          ui.inicialdateEdit->date() , ui.finaldateEdit->date() );

 if (consulta.next() && !(consulta.value(0).toDouble()<0.0001 && consulta.value(0).toDouble()>-0.0001))
  {
    ui.autofacturas_exttable->insertRow(fila);
    ui.autofacturas_exttable->setItem(fila,0,new QTableWidgetItem(tr("Autofacturas no UE al tipo 0")));
    QTableWidgetItem *newItem1 = new QTableWidgetItem(
                   formatea_redondeado(consulta.value(0).toDouble(),comadecimal,decimales));
    newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
    ui.autofacturas_exttable->setItem(fila,1,newItem1);

    QTableWidgetItem *newItem2 = new QTableWidgetItem("");
    ui.autofacturas_exttable->setItem(fila,2,newItem2);

    QTableWidgetItem *newItem3 = new QTableWidgetItem("");
    ui.autofacturas_exttable->setItem(fila,3,newItem3);

    QTableWidgetItem *newItem4 = new QTableWidgetItem("");
    ui.autofacturas_exttable->setItem(fila,4,newItem4);

    QTableWidgetItem *newItem5 = new QTableWidgetItem(QString());
    newItem5->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
    ui.autofacturas_exttable->setItem(fila,5,newItem5);
  }

  // --------------------------------------------------------------------------------------
  // -------- ISP OPERACIONES INTERIORES

 // Autofacturas no ue

 fila=0;
 progress.setValue(26);
 QApplication::processEvents();

 // llenamos cuadro ISP interiores
 consulta = basedatos::instancia()->libroiva_isp_interiores_recibidas( ui.inicialdateEdit->date() ,
                                                             ui.finaldateEdit->date() );
 while (consulta.next())
     {
        ui.autofacturas_interiorestable->insertRow(fila);
        ui.autofacturas_interiorestable->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));

        QTableWidgetItem *newItem1 = new QTableWidgetItem(
                     formatea_redondeado(consulta.value(2).toDouble(),comadecimal,decimales));
        newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
        ui.autofacturas_interiorestable->setItem(fila,1,newItem1);

        QString tipoiva=consulta.value(1).toString();
        if (comadecimal) tipoiva=convacoma(tipoiva);
        QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
        newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
        ui.autofacturas_interiorestable->setItem(fila,2,newItem2);

        QTableWidgetItem *newItem3 = new QTableWidgetItem(
                     formatea_redondeado(consulta.value(3).toDouble(),comadecimal,decimales));
        sumaliquidacion+=consulta.value(3).toDouble();
        newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
        ui.autofacturas_interiorestable->setItem(fila,3,newItem3);
        QTableWidgetItem *newItem4 = new QTableWidgetItem("");
        newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
        ui.autofacturas_interiorestable->setItem(fila,4,newItem4);
        fila++;
     }

 // llenamos cuadro autofacturas fuera ue parte repercutido
 progress.setValue(27);
 QApplication::processEvents();
 consulta = basedatos::instancia()->libroiva_isp_interiores_emitidas(
                  ui.inicialdateEdit->date(),
                  ui.finaldateEdit->date() );
 while (consulta.next())
     {
        ui.autofacturas_interiorestable->insertRow(fila);
        ui.autofacturas_interiorestable->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));

        QTableWidgetItem *newItem1 = new QTableWidgetItem(
                     formatea_redondeado(consulta.value(2).toDouble(),comadecimal,decimales));
        newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
        ui.autofacturas_interiorestable->setItem(fila,1,newItem1);

        QString tipoiva=consulta.value(1).toString();
        if (comadecimal) tipoiva=convacoma(tipoiva);
        QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
        newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
        ui.autofacturas_interiorestable->setItem(fila,2,newItem2);

        QTableWidgetItem *newItem3 = new QTableWidgetItem(
                     formatea_redondeado(consulta.value(3).toDouble(),comadecimal,decimales));
        sumaliquidacion-=consulta.value(3).toDouble();
        newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
        ui.autofacturas_interiorestable->setItem(fila,4,newItem3);
        QTableWidgetItem *newItem4 = new QTableWidgetItem("");
        newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
        ui.autofacturas_interiorestable->setItem(fila,3,newItem4);
        fila++;
     }

 progress.setValue(28);
 QApplication::processEvents();


 // calculamos ahora IVA soportado por isp interiores al tipo 0 (exento) e insertamos fila
   consulta = basedatos::instancia()->base_isp_interiores_recibidasexentas(
         ui.inicialdateEdit->date() , ui.finaldateEdit->date() );

if (consulta.next() && !(consulta.value(0).toDouble()<0.0001 && consulta.value(0).toDouble()>-0.0001))
 {
   ui.autofacturas_interiorestable->insertRow(fila);
   ui.autofacturas_interiorestable->setItem(fila,0,new QTableWidgetItem(tr("Autofacturas no UE al tipo 0")));
   QTableWidgetItem *newItem1 = new QTableWidgetItem(
                  formatea_redondeado(consulta.value(0).toDouble(),comadecimal,decimales));
   newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
   ui.autofacturas_interiorestable->setItem(fila,1,newItem1);

   QTableWidgetItem *newItem2 = new QTableWidgetItem("");
   ui.autofacturas_interiorestable->setItem(fila,2,newItem2);

   QTableWidgetItem *newItem3 = new QTableWidgetItem("");
   ui.autofacturas_interiorestable->setItem(fila,3,newItem3);

   QTableWidgetItem *newItem4 = new QTableWidgetItem("");
   ui.autofacturas_interiorestable->setItem(fila,4,newItem4);

   QTableWidgetItem *newItem5 = new QTableWidgetItem(QString());
   newItem5->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
   ui.autofacturas_interiorestable->setItem(fila,5,newItem5);
 }

progress.setValue(29);
QApplication::processEvents();

  // -------------------------------------------------------------------------------------
  // suma base eib
  consulta = basedatos::instancia()->selectSumbase_serv_ue_libroivadiariofechas( ui.inicialdateEdit->date() , ui.finaldateEdit->date() );
  fila=0;
  if (consulta.next())  ui.pr_servlineEdit->setText(formatea_redondeado(consulta.value(0).toDouble(),
                                                 comadecimal,decimales));

  // --------------------------------------------------------------------------------------
  // EXPORTACIONES

  consulta = basedatos::instancia()->selectSumbase_expor_fechas ( ui.inicialdateEdit->date(),
                                                                  ui.finaldateEdit->date() );
  fila=0;
  if (consulta.next())  ui.exportlineEdit->setText(formatea_redondeado(consulta.value(0).toDouble(),
                                                 comadecimal,decimales));

  consulta = basedatos::instancia()->selectSumbase_expor_fechas_no_sujetas(  ui.inicialdateEdit->date(),
                                                                  ui.finaldateEdit->date() );
  // añadimos isp emitidas internas; es para la casilla 61
  fila=0;
  if (consulta.next())  ui.export_no_sujeta_lineEdit->setText(formatea_redondeado(consulta.value(0).toDouble(),
                                                 comadecimal,decimales));


  consulta = basedatos::instancia()->selectSumbase_emitidas_isp(  ui.inicialdateEdit->date(),
                                                                  ui.finaldateEdit->date() );
  // añadimos isp emitidas internas; es para la casilla 61
  fila=0;
  if (consulta.next())  ui.sujetas_isp_lineEdit->setText(formatea_redondeado(consulta.value(0).toDouble(),
                                                 comadecimal,decimales));


  progress.setValue(30);
  QApplication::processEvents();

  // EMITIDAS EXENTAS CON DCHO A DEDUCCIÓN Y SIN DERECHO A DEDUCCIÓN

  consulta = basedatos::instancia()->baseemitidasexentas_noeib_noexport  (
        ui.inicialdateEdit->date() , ui.finaldateEdit->date() );

  double exentas_todas=0;

  if (consulta.next()) exentas_todas=consulta.value(0).toDouble();

  consulta = basedatos::instancia()->baseemitidasexentas_noeib_noexport_nodeduc   (
        ui.inicialdateEdit->date() , ui.finaldateEdit->date() );

  double exentas_no_deduc=0;

  if (consulta.next()) exentas_no_deduc=consulta.value(0).toDouble();

  ui.exentas_deduclineEdit->setText(formatea_redondeado(exentas_todas-exentas_no_deduc,
                                                   comadecimal,decimales));

  ui.exentas_no_deduclineEdit->setText(formatea_redondeado(exentas_no_deduc,
                                                      comadecimal,decimales));
  progress.setValue(31);
  QApplication::processEvents();

  // -------------------------------------------------------------------------------------
  consulta = basedatos::instancia()->libroivafechas_sop_agrario (
                                                       ui.inicialdateEdit->date(),
                                                       ui.finaldateEdit->date() );

  // RÉGIMEN AGRARIO
  fila=0;
  while (consulta.next())
      {
         ui.agrariotableWidget->insertRow(fila);
         ui.agrariotableWidget->setItem(fila,0,new QTableWidgetItem(consulta.value(0).toString()));
         // base
         QTableWidgetItem *newItem1 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(2).toDouble(),comadecimal,decimales));
         newItem1->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.agrariotableWidget->setItem(fila,1,newItem1);

         QString tipoiva=consulta.value(1).toString();
         if (comadecimal) tipoiva=convacoma(tipoiva);
         QTableWidgetItem *newItem2 = new QTableWidgetItem(tipoiva);
         newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.agrariotableWidget->setItem(fila,2,newItem2);

         // cuota
         QTableWidgetItem *newItem3 = new QTableWidgetItem(
                      formatea_redondeado(consulta.value(3).toDouble(),comadecimal,decimales));
         newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.agrariotableWidget->setItem(fila,3,newItem3);
         double imp_total=consulta.value(2).toDouble() + consulta.value(3).toDouble();
         sumaliquidacion+=consulta.value(3).toDouble();
         QTableWidgetItem *newItem4 = new QTableWidgetItem(formatea_redondeado(imp_total,
                                                           comadecimal,decimales));
         newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui.agrariotableWidget->setItem(fila,4,newItem4);
         fila++;
      }
  if (sumaliquidacion>0)
    {
     ui.concepto_liq_label->setText(tr("A compensar/devolver"));
    }
    else
         {
          ui.concepto_liq_label->setText(tr("A ingresar en Tesoro"));
         }
 ui.resultadolineEdit->setText(formatea_redondeado(sumaliquidacion,
                                              comadecimal,decimales));
}


void liquidacioniva::borrar()
{
  ui.soportadotable->setRowCount(0);
  ui.repercutidotable->setRowCount(0);
  ui.aibtable->setRowCount(0);
  ui.autofacturastable->setRowCount(0);
  ui.autofacturas_exttable->setRowCount(0);
  ui.imp_soportadotable->setRowCount(0);
  ui.autofacturas_interiorestable->setRowCount(0);
  ui.cuota_implineEdit->clear();
  ui.cuotalineEdit->clear();
  ui.ivalineEdit->clear();
  ui.relineEdit->clear();
  ui.eiblineEdit->clear();
  ui.exportlineEdit->clear();
  ui.pr_servlineEdit->clear();
  ui.resultadolineEdit->clear();
  ui.concepto_liq_label->setText(" - - ");
}


void liquidacioniva::fechacambiada()
{
  borrar();
}


void liquidacioniva::generalatex()
{
   QString qfichero=dirtrabajo();
   qfichero.append(QDir::separator());
   qfichero=qfichero+tr("liquidacion_IVA.tex");
   QFile fichero(adapta(qfichero));
   if (! fichero.open( QIODevice::WriteOnly ) ) return;

    QTextStream stream( &fichero );
    stream.setEncoding(QStringConverter::Utf8);

    stream << cabeceralatex();
    stream << margen_extra_latex();

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {" << filtracad(nombreempresa()) << "}}" << "\n";
    stream << "\\end{center}" << "\n";
    stream << "\n";
    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {" << tr("LIQUIDACIÓN DE IVA") <<  "}}" << "\n";
    stream << "\\end{center}" << "\n";
    stream << "\\begin{center}" << "\n";
    stream << "{\\textbf {" << tr("Período de ") << ui.inicialdateEdit->date().toString("dd.MM.yyyy") <<
    tr(" a ") << ui.finaldateEdit->date().toString("dd.MM.yyyy") << "}}" << "\n";
    stream << "\\end{center}" << "\n";
    
    stream << "\\\n";


    stream << "\\begin{center}\n";
    stream << "{\\textbf {" << tr("RESULTADO LIQUIDACIÓN: ") <<
          ui.concepto_liq_label->text() << " "
         << formatea_redondeado_sep(convapunto(ui.resultadolineEdit->text()).toDouble(),
                              comadecimal,decimales) <<  "}}" << "\n";
    stream << "\\end{center}\n";

    stream << "\\\n";



    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {" << tr("IVA REPERCUTIDO") <<  "}}" << "\n";
    stream << "\\end{center}" << "\n";

     // --------------------------------------------------------------------------------------------------
    stream << "\\begin{center}\n";
    stream << "\\begin{tabular}{|c|r|r|r|r|r|r|r|}\n";
    stream << "\\hline\n";
    stream << "{\\scriptsize " << tr("CUENTA") << "} & {\\scriptsize " << tr("Base Imponible") << 
      "} & {\\scriptsize " << tr("Tipo") << "} & {\\scriptsize " << tr("Cuota") << "} & {\\scriptsize " << 
      tr("T.R.E.") << "} & {\\scriptsize " << tr("R.E.") << "} & {\\scriptsize "
      << tr("Total")  << "} & {\\scriptsize "  << tr("Cuota C.Caja") << "}  \\\\\n";
    stream << "\\hline\n";
    int pos=0;
    double tcuotare=0;
    double tbi=0; double tcuota=0; double ttotal=0; double tcuotacaja=0;
    while (pos<ui.repercutidotable->rowCount())
        {
          double bi,cuota,cuotare,total;
          stream << "{\\scriptsize " << ui.repercutidotable->item(pos,0)->text() << 
		  "} & {\\scriptsize " <<
                      formatea_redondeado_sep(convapunto(ui.repercutidotable->item(pos,1)->text()).toDouble(),
                      comadecimal,decimales) << 
		  "} & {\\scriptsize ";
          bi=convapunto(ui.repercutidotable->item(pos,1)->text()).toDouble();
          stream << formatea_redondeado_sep(convapunto(ui.repercutidotable->item(pos,2)->text()).toDouble(),
                   comadecimal,decimales) << "} & {\\scriptsize ";
          if (ui.repercutidotable->item(pos,6)->text().isEmpty())
             stream << formatea_redondeado_sep(convapunto(ui.repercutidotable->item(pos,3)->text()).toDouble(),
                   comadecimal,decimales);
          stream << "} & {\\scriptsize " <<
               formatea_redondeado_sep(convapunto(ui.repercutidotable->item(pos,4)->text()).toDouble(),
                   comadecimal,decimales) << "} & {\\scriptsize ";
         cuota=convapunto(ui.repercutidotable->item(pos,3)->text()).toDouble();
         stream << formatea_redondeado_sep(convapunto(ui.repercutidotable->item(pos,5)->text()).toDouble(),
                   comadecimal,decimales);
         stream << "} & {\\scriptsize ";
         cuotare=convapunto(ui.repercutidotable->item(pos,5)->text()).toDouble();
         total=bi+cuota+cuotare;
         stream << formatea_redondeado_sep(total,comadecimal,decimales) << "} & {\\scriptsize ";
         stream << formatea_redondeado_sep(convapunto(ui.repercutidotable->item(pos,6)->text()).toDouble(),
                                           comadecimal, decimales);
         tbi+=bi;
         if (ui.repercutidotable->item(pos,6)->text().isEmpty())
            tcuota+=cuota;
         tcuotare+=cuotare;
         ttotal+=total;
         tcuotacaja+=convapunto(ui.repercutidotable->item(pos,6)->text()).toDouble();
         stream << "} \\\\ \n " << "\\hline\n";
         pos++;
       }

    stream << "{\\scriptsize \\textbf {";
    stream << tr("Totales:");
    stream << "}}";
    // colocamos línea de totales
    stream << " & {\\scriptsize ";
    stream << formatea_redondeado_sep(tbi,comadecimal,decimales);
    stream << "} & & ";

    // ahora viene la cuota
    stream << "{\\scriptsize ";
    stream << formatea_redondeado_sep(tcuota,comadecimal,decimales);
    // stream << "} & {\\scriptsize ";


    stream << "} & & ";

    // ahora viene la cuota RE
    stream << "{\\scriptsize ";
    stream << formatea_redondeado_sep(tcuotare,comadecimal,decimales);
    stream << "} & {\\scriptsize ";

    // total
    stream << formatea_redondeado_sep(ttotal,comadecimal,decimales);

    stream << "} & {\\scriptsize ";

    // tcuotacaja
    stream << formatea_redondeado_sep(tcuotacaja,comadecimal,decimales);

    stream << "} \\\\ \n";
    stream << "\\hline\n";



    stream << "\\end{tabular}\n";
    stream << "\\end{center}\n";

    stream << "\\begin{center}\n";
    stream << "{\\textbf {" << tr("Total IVA repercutido:  ") << formatea_redondeado_sep(convapunto(ui.ivalineEdit->text()).toDouble(),
                   comadecimal,decimales) << 
            tr(" - Total recargo equivalencia: ") << formatea_redondeado_sep(convapunto(ui.relineEdit->text()).toDouble(),
                   comadecimal,decimales) << "}}" << "\n";
    stream << "\\end{center}\n";
    
    stream << "\\\n";
    stream << "\\\n";
    //----------------------------------------------------------------------------------------------------------

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {" << tr("IVA SOPORTADO") <<  "}}" << "\n";
    stream << "\\end{center}" << "\n";

     // ----------------------------------------------------------------------------------------------
    stream << "\\begin{center}\n";
    stream << "\\begin{tabular}{|c|r|r|r|r|c|r|r|}\n";
    stream << "\\hline\n";
    stream << "{\\scriptsize " << tr("CUENTA") << "} & {\\scriptsize " << tr("Base Imponible") <<
      "} & {\\scriptsize " << tr("Tipo") << "} & {\\scriptsize " << tr("Cuota") << "} & {\\scriptsize " <<
      tr("Total") << "} & {\\scriptsize " << tr("Inversión")
      << "} & {\\scriptsize " << tr("Cuota efectiva")
      << "} & {\\scriptsize " << tr("Cuota C.Caja") << "}  \\\\\n";
    stream << "\\hline\n";
    pos=0;
    tbi=0; tcuota=0; ttotal=0; double tcuota_efectiva=0; double tcuota_caja=0;
    while (pos<ui.soportadotable->rowCount())
        {
          stream << "{\\scriptsize " << ui.soportadotable->item(pos,0)->text() <<
                  "} & {\\scriptsize " << formatea_redondeado_sep(convapunto(
                                 ui.soportadotable->item(pos,1)->text()).toDouble(),comadecimal,decimales) <<
                  "} & {\\scriptsize ";
          stream << formatea_redondeado_sep(convapunto(
                       ui.soportadotable->item(pos,2)->text()).toDouble(),comadecimal,decimales)
                       << "} & {\\scriptsize ";
          if (ui.soportadotable->item(pos,7)->text().isEmpty())
             stream << formatea_redondeado_sep(convapunto(ui.soportadotable->item(pos,3)->text()).toDouble(),
                                     comadecimal,decimales);
          stream << "} & {\\scriptsize ";
          // stream << convacoma(soportadotable->text(pos,4));
          stream << formatea_redondeado_sep(convapunto(ui.soportadotable->item(pos,4)->text()).toDouble(),
                                     comadecimal,decimales);
          QString estrella=(ui.soportadotable->item(pos,5)!=0) ? ui.soportadotable->item(pos,5)->text() : "";
          stream << "} & {\\scriptsize " << estrella << "} & {\\scriptsize ";
          if (ui.soportadotable->item(pos,7)->text().isEmpty())
            {
             if (ui.soportadotable->item(pos,6)!=NULL)
              stream << formatea_redondeado_sep(convapunto(ui.soportadotable->item(pos,6)->text()).toDouble(),
                                     comadecimal,decimales);
            }
         tbi+= convapunto(ui.soportadotable->item(pos,1)->text()).toDouble();
         if (ui.soportadotable->item(pos,7)->text().isEmpty())
            tcuota+=convapunto(ui.soportadotable->item(pos,3)->text()).toDouble();
         ttotal+=convapunto(ui.soportadotable->item(pos,4)->text()).toDouble();
         if (ui.soportadotable->item(pos,7)->text().isEmpty())
            tcuota_efectiva+= ui.soportadotable->item(pos,6)!=NULL ?
                           convapunto(ui.soportadotable->item(pos,6)->text()).toDouble() : 0;
         tcuota_caja+=convapunto(ui.soportadotable->item(pos,7)->text()).toDouble();
         stream << "} & {\\scriptsize ";
         stream << formatea_redondeado_sep(convapunto(ui.soportadotable->item(pos,7)->text()).toDouble(),
                                    comadecimal,decimales);
         stream << "} \\\\ \n " << "\\hline\n";
         // QMessageBox::information( this, tr("INFO"),ui.soportadotable->item(pos,0)->text());
         pos++;
       }
    stream << "{\\scriptsize \\textbf {";
    stream << tr("Totales:");
    stream << "}}";
    // colocamos línea de totales
    stream << " & {\\scriptsize ";
    stream << formatea_redondeado_sep(tbi,comadecimal,decimales);
    stream << "} & & ";

    // ahora viene la cuota
    stream << "{\\scriptsize ";
    stream << formatea_redondeado_sep(tcuota,comadecimal,decimales);
    stream << "} & {\\scriptsize ";

    // total
    stream << formatea_redondeado_sep(ttotal,comadecimal,decimales);
    stream << "} & & {\\scriptsize ";
    stream << formatea_redondeado_sep(tcuota_efectiva,comadecimal,decimales);
    stream << "} &  {\\scriptsize ";
    stream << formatea_redondeado_sep(tcuota_caja,comadecimal,decimales);
    stream << "} \\\\ \n";
    stream << "\\hline\n";


    stream << "\\end{tabular}\n";
    stream << "\\end{center}\n";
    stream << "\\begin{center}\n";
    stream << "{\\textbf {" << ui.cuotasoptextLabel->text() << " " <<
           formatea_redondeado_sep(convapunto(ui.cuotalineEdit->text()).toDouble(),comadecimal,decimales) << "}}" << "\n";
    stream << "\\end{center}\n";

    stream << "\\\n";
    stream << "\\\n";


   // ----------------------------------------------------------------------------------
   // --- IVA SOPORTADO EN IMPORTACIONES


    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {" << tr("IVA SOPORTADO IMPORTACIONES") <<  "}}" << "\n";
    stream << "\\end{center}" << "\n";

     // ----------------------------------------------------------------------------------------------
    stream << "\\begin{center}\n";
    stream << "\\begin{tabular}{|c|r|r|r|r|c|r|}\n";
    stream << "\\hline\n";
    stream << "{\\scriptsize " << tr("CUENTA") << "} & {\\scriptsize " << tr("Base Imponible") <<
      "} & {\\scriptsize " << tr("Tipo") << "} & {\\scriptsize " << tr("Cuota") << "} & {\\scriptsize " <<
      tr("Total") << "} & {\\scriptsize " << tr("Inversión")
      << "} & {\\scriptsize " << tr("Cuota efectiva") << "}  \\\\\n";
    stream << "\\hline\n";
    pos=0;
    tbi=0, tcuota=0, ttotal=0, tcuota_efectiva=0;
    while (pos<ui.imp_soportadotable->rowCount())
        {
          stream << "{\\scriptsize " << ui.imp_soportadotable->item(pos,0)->text() <<
                  "} & {\\scriptsize " << formatea_redondeado_sep(convapunto(
                                 ui.imp_soportadotable->item(pos,1)->text()).toDouble(),comadecimal,decimales) <<
                  "} & {\\scriptsize ";
          stream << formatea_redondeado_sep(convapunto(
                       ui.imp_soportadotable->item(pos,2)->text()).toDouble(),comadecimal,decimales)
                       << "} & {\\scriptsize " <<
                    formatea_redondeado_sep(convapunto(ui.imp_soportadotable->item(pos,3)->text()).toDouble(),
                                     comadecimal,decimales) << "} & {\\scriptsize ";
          // stream << convacoma(soportadotable->text(pos,4));
          stream << formatea_redondeado_sep(convapunto(ui.imp_soportadotable->item(pos,4)->text()).toDouble(),
                                     comadecimal,decimales);
          QString estrella=(ui.imp_soportadotable->item(pos,5)!=0) ? ui.imp_soportadotable->item(pos,5)->text() : "";
          stream << "} & {\\scriptsize " << estrella << "} & {\\scriptsize ";
          if (ui.imp_soportadotable->item(pos,6)!=NULL)
            stream << formatea_redondeado_sep(convapunto(ui.imp_soportadotable->item(pos,6)->text()).toDouble(),
                                     comadecimal,decimales);
         tbi+= convapunto(ui.imp_soportadotable->item(pos,1)->text()).toDouble();
         tcuota+=convapunto(ui.imp_soportadotable->item(pos,3)->text()).toDouble();
         ttotal+=convapunto(ui.imp_soportadotable->item(pos,4)->text()).toDouble();
         tcuota_efectiva+= ui.imp_soportadotable->item(pos,6)!=NULL ?
                           convapunto(ui.imp_soportadotable->item(pos,6)->text()).toDouble() : 0;
         stream << "} \\\\ \n " << "\\hline\n";
         // QMessageBox::information( this, tr("INFO"),ui.soportadotable->item(pos,0)->text());
         pos++;
       }
    stream << "{\\scriptsize \\textbf {";
    stream << tr("Totales:");
    stream << "}}";
    // colocamos línea de totales
    stream << " & {\\scriptsize ";
    stream << formatea_redondeado_sep(tbi,comadecimal,decimales);
    stream << "} & & ";

    // ahora viene la cuota
    stream << "{\\scriptsize ";
    stream << formatea_redondeado_sep(tcuota,comadecimal,decimales);
    stream << "} & {\\scriptsize ";

    // total
    stream << formatea_redondeado_sep(ttotal,comadecimal,decimales);
    stream << "} & & {\\scriptsize ";
    stream << formatea_redondeado_sep(tcuota_efectiva,comadecimal,decimales);
    stream << "} \\\\ \n";
    stream << "\\hline\n";


    stream << "\\end{tabular}\n";
    stream << "\\end{center}\n";
    stream << "\\begin{center}\n";
    stream << "{\\textbf {" << ui.cuotasop_imptextLabel->text() << " " <<
           formatea_redondeado_sep(convapunto(ui.cuota_implineEdit->text()).toDouble(),comadecimal,decimales) << "}}" << "\n";
    stream << "\\end{center}\n";

    stream << "\\\n";
    stream << "\\\n";



   // ----------------------------------------------------------------------------------
    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {" << tr("ADQUISICIONES INTRACOMUNITARIAS DE BIENES")
	  <<  "}}" << "\n";
    stream << "\\end{center}" << "\n";
    
     // -------------------------------------------------------------------------------------------------------
    stream << "\\begin{center}\n";
    stream << "\\begin{tabular}{|c|r|r|r|r|c|}\n";
    stream << "\\hline\n";
    stream << "{\\scriptsize " << tr("CUENTA") << "} & {\\scriptsize " << tr("Base Imponible") << 
       "} & {\\scriptsize " << tr("Tipo") << "} & {\\scriptsize " << tr("Cuota Sop.") << 
       "} & {\\scriptsize " << tr("Cuota Rep") << "} & {\\scriptsize " << tr("Inversión")<< "}  \\\\\n";
    stream << "\\hline\n";
    pos=0;
    while (pos<ui.aibtable->rowCount())
        {
          stream << "{\\scriptsize " << ui.aibtable->item(pos,0)->text() << 
		  "} & {\\scriptsize " <<
                                 formatea_redondeado_sep(convapunto(ui.aibtable->item(pos,1)->text()).toDouble(),
                                 comadecimal,decimales) << 
		  "} & {\\scriptsize ";
          stream << formatea_redondeado_sep(convapunto(ui.aibtable->item(pos,2)->text()).toDouble(),
                                 comadecimal,decimales) << "} & {\\scriptsize " << 
               formatea_redondeado_sep(convapunto(ui.aibtable->item(pos,3)->text()).toDouble(),
                                 comadecimal,decimales) << "} & {\\scriptsize ";
          stream << formatea_redondeado_sep(convapunto(ui.aibtable->item(pos,4)->text()).toDouble(),
                                 comadecimal,decimales);
          QString estrella=(ui.aibtable->item(pos,5)!=0) ? ui.aibtable->item(pos,5)->text() : "";
          stream << "} & {\\scriptsize " << estrella;
          stream << "} \\\\ \n " << "\\hline\n";
          pos++;
        }
    stream << "\\end{tabular}\n";
    stream << "\\end{center}\n";


    stream << "\\\n";
    stream << "\\\n";
// -----------------------------------------------------------------------------------

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {" << tr("ADQUISICIONES INTRACOMUNITARIAS DE SERVICIOS")
          <<  "}}" << "\n";
    stream << "\\end{center}" << "\n";

     // -------------------------------------------------------------------------------------------------------
    stream << "\\begin{center}\n";
    stream << "\\begin{tabular}{|c|r|r|r|r|}\n";
    stream << "\\hline\n";
    stream << "{\\scriptsize " << tr("CUENTA") << "} & {\\scriptsize " << tr("Base Imponible") <<
       "} & {\\scriptsize " << tr("Tipo") << "} & {\\scriptsize " << tr("Cuota Sop.") <<
       "} & {\\scriptsize " << tr("Cuota Rep") << "}  \\\\\n";
    stream << "\\hline\n";
    pos=0;
    while (pos<ui.autofacturastable->rowCount())
        {
          stream << "{\\scriptsize " << ui.autofacturastable->item(pos,0)->text() <<
                  "} & {\\scriptsize " <<
                                 formatea_redondeado_sep(convapunto(ui.autofacturastable->item(pos,1)->text()).toDouble(),
                                 comadecimal,decimales) <<
                  "} & {\\scriptsize ";
          stream << formatea_redondeado_sep(convapunto(ui.autofacturastable->item(pos,2)->text()).toDouble(),
                                 comadecimal,decimales) << "} & {\\scriptsize " <<
               formatea_redondeado_sep(convapunto(ui.autofacturastable->item(pos,3)->text()).toDouble(),
                                 comadecimal,decimales) << "} & {\\scriptsize ";
          stream << formatea_redondeado_sep(convapunto(ui.autofacturastable->item(pos,4)->text()).toDouble(),
                                 comadecimal,decimales);
          stream << "} \\\\ \n " << "\\hline\n";
          pos++;
        }
    stream << "\\end{tabular}\n";
    stream << "\\end{center}\n";

// -----------------------------------------------------------------------------------
    stream << "\\\n";
    stream << "\\\n";

    stream << "\\begin{center}" << "\n";
    stream << "{\\Large \\textbf {" << tr("COMPRAS DE SERVICIOS EXTERIORES UE")
          <<  "}}" << "\n";
    stream << "\\end{center}" << "\n";

    stream << "\\begin{center}\n";
    stream << "\\begin{tabular}{|c|r|r|r|r|}\n";
    stream << "\\hline\n";
    stream << "{\\scriptsize " << tr("CUENTA") << "} & {\\scriptsize " << tr("Base Imponible") <<
       "} & {\\scriptsize " << tr("Tipo") << "} & {\\scriptsize " << tr("Cuota Sop.") <<
       "} & {\\scriptsize " << tr("Cuota Rep") << "}  \\\\\n";
    stream << "\\hline\n";
    pos=0;
    while (pos<ui.autofacturas_exttable->rowCount())
        {
          stream << "{\\scriptsize " << ui.autofacturas_exttable->item(pos,0)->text() <<
                  "} & {\\scriptsize " <<
                                 formatea_redondeado_sep(convapunto(ui.autofacturas_exttable->item(pos,1)->text()).toDouble(),
                                 comadecimal,decimales) <<
                  "} & {\\scriptsize ";
          stream << formatea_redondeado_sep(convapunto(ui.autofacturas_exttable->item(pos,2)->text()).toDouble(),
                                 comadecimal,decimales) << "} & {\\scriptsize " <<
               formatea_redondeado_sep(convapunto(ui.autofacturas_exttable->item(pos,3)->text()).toDouble(),
                                 comadecimal,decimales) << "} & {\\scriptsize ";
          stream << formatea_redondeado_sep(convapunto(ui.autofacturas_exttable->item(pos,4)->text()).toDouble(),
                                 comadecimal,decimales);
          stream << "} \\\\ \n " << "\\hline\n";
          pos++;
        }
    stream << "\\end{tabular}\n";
    stream << "\\end{center}\n";

// -----------------------------------------------------------------------------------
// ISP operaciones interiores

    // -----------------------------------------------------------------------------------
        stream << "\\\n";
        stream << "\\\n";

        stream << "\\begin{center}" << "\n";
        stream << "{\\Large \\textbf {" << tr("INVERSIÓN SUJETO PASIVO OPERACIONES INTERIORES")
              <<  "}}" << "\n";
        stream << "\\end{center}" << "\n";

        stream << "\\begin{center}\n";
        stream << "\\begin{tabular}{|c|r|r|r|r|}\n";
        stream << "\\hline\n";
        stream << "{\\scriptsize " << tr("CUENTA") << "} & {\\scriptsize " << tr("Base Imponible") <<
           "} & {\\scriptsize " << tr("Tipo") << "} & {\\scriptsize " << tr("Cuota Sop.") <<
           "} & {\\scriptsize " << tr("Cuota Rep") << "}  \\\\\n";
        stream << "\\hline\n";
        pos=0;
        while (pos<ui.autofacturas_interiorestable->rowCount())
            {
              stream << "{\\scriptsize " << ui.autofacturas_interiorestable->item(pos,0)->text() <<
                      "} & {\\scriptsize " <<
                                     formatea_redondeado_sep(convapunto(ui.autofacturas_interiorestable->item(pos,1)->text()).toDouble(),
                                     comadecimal,decimales) <<
                      "} & {\\scriptsize ";
              stream << formatea_redondeado_sep(convapunto(ui.autofacturas_interiorestable->item(pos,2)->text()).toDouble(),
                                     comadecimal,decimales) << "} & {\\scriptsize " <<
                   formatea_redondeado_sep(convapunto(ui.autofacturas_interiorestable->item(pos,3)->text()).toDouble(),
                                     comadecimal,decimales) << "} & {\\scriptsize ";
              stream << formatea_redondeado_sep(convapunto(ui.autofacturas_interiorestable->item(pos,4)->text()).toDouble(),
                                     comadecimal,decimales);
              stream << "} \\\\ \n " << "\\hline\n";
              pos++;
            }
        stream << "\\end{tabular}\n";
        stream << "\\end{center}\n";

    // -----------------------------------------------------------------------------------


        // -----------------------------------------------------------------------------------
        // RÉGIMEN AGRARIO/PESCA

            // -----------------------------------------------------------------------------------
                stream << "\\\n";
                stream << "\\\n";

                stream << "\\begin{center}" << "\n";
                stream << "{\\Large \\textbf {" << tr("IVA COMPENSACIÓN EN RÉGIMEN AGRARIO/PESCA")
                      <<  "}}" << "\n";
                stream << "\\end{center}" << "\n";

                stream << "\\begin{center}\n";
                stream << "\\begin{tabular}{|c|r|r|r|r|}\n";
                stream << "\\hline\n";
                stream << "{\\scriptsize " << tr("CUENTA") << "} & {\\scriptsize " << tr("Base Imponible") <<
                   "} & {\\scriptsize " << tr("Tipo") << "} & {\\scriptsize " << tr("Cuota Sop.") <<
                   "} & {\\scriptsize " << tr("Total") << "}  \\\\\n";
                stream << "\\hline\n";
                pos=0;
                while (pos<ui.agrariotableWidget->rowCount())
                    {
                      stream << "{\\scriptsize " << ui.agrariotableWidget->item(pos,0)->text() <<
                              "} & {\\scriptsize " <<
                                             formatea_redondeado_sep(convapunto(ui.agrariotableWidget->item(pos,1)->text()).toDouble(),
                                             comadecimal,decimales) <<
                              "} & {\\scriptsize ";
                      stream << formatea_redondeado_sep(convapunto(ui.agrariotableWidget->item(pos,2)->text()).toDouble(),
                                             comadecimal,decimales) << "} & {\\scriptsize " <<
                           formatea_redondeado_sep(convapunto(ui.agrariotableWidget->item(pos,3)->text()).toDouble(),
                                             comadecimal,decimales) << "} & {\\scriptsize ";
                      stream << formatea_redondeado_sep(convapunto(ui.agrariotableWidget->item(pos,4)->text()).toDouble(),
                                             comadecimal,decimales);
                      stream << "} \\\\ \n " << "\\hline\n";
                      pos++;
                    }
                stream << "\\end{tabular}\n";
                stream << "\\end{center}\n";

            // -----------------------------------------------------------------------------------


    stream << "\\\n";
    stream << "\\\n";


    stream << "\\begin{center}\n";
    stream << "{\\Large \\textbf {" << tr("Entregas intracomunitarias de bienes:  ") << 
         formatea_redondeado_sep(convapunto(ui.eiblineEdit->text()).toDouble(),comadecimal,decimales) <<  "}}" << "\n";
    stream << "\\end{center}\n";
    stream << "\\begin{center}\n";
    stream << "{\\Large \\textbf {" << tr("Prestaciones de servicios intracomunitarias:  ") <<
         formatea_redondeado_sep(convapunto(ui.pr_servlineEdit->text()).toDouble(),comadecimal,decimales) <<  "}}" << "\n";
    stream << "\\end{center}\n";

    stream << "\\\n";

    stream << "\\begin{center}\n";
    stream << "{\\Large \\textbf {" << tr("Exportaciones bienes y prestación servicios exterior:  ") <<
         formatea_redondeado_sep(convapunto(ui.exportlineEdit->text()).toDouble(),comadecimal,decimales) <<  "}}" << "\n";
    stream << "\\end{center}\n";

    stream << "\\begin{center}\n";
    stream << "{\\Large \\textbf {" << tr("Operaciones no sujetas por reglas de localización:  ") <<
         formatea_redondeado_sep(convapunto(ui.export_no_sujeta_lineEdit->text()).toDouble(),comadecimal,decimales) <<  "}}" << "\n";
    stream << "\\end{center}\n";

    stream << "\\begin{center}\n";
    stream << "{\\large \\textbf {" << tr("Operaciones emitidas sujetas con inversión del sujeto pasivo:  ") <<
         formatea_redondeado_sep(convapunto(ui.sujetas_isp_lineEdit->text()).toDouble(),comadecimal,decimales) <<  "}}" << "\n";
    stream << "\\end{center}\n";

    stream << "\\begin{center}\n";
    stream << "{\\large \\textbf {" << tr("Facturas de operaciones emitidas exentas que NO originan derecho a deducción:  ") <<
         formatea_redondeado_sep(convapunto(ui.exentas_no_deduclineEdit->text()).toDouble(),comadecimal,decimales) <<  "}}" << "\n";
    stream << "\\end{center}\n";


     // imprimimos final del documento latex
    stream << "% FIN_CUERPO" << "\n";
    stream << "\\end{document}" << "\n";

    fichero.close();


}



void liquidacioniva::imprimir()
{
   informe();
}

void liquidacioniva::imprimir_latex()
{
   generalatex();

   int valor=imprimelatex2(tr("liquidacion_IVA"));
   if (valor==1)
       QMessageBox::warning( this, tr("Imprimir liquidación IVA"),tr("PROBLEMAS al llamar a Latex"));
   if (valor==2)
       QMessageBox::warning( this, tr("Imprimir liquidación IVA"),
                                tr("PROBLEMAS al llamar a 'dvips'"));
   if (valor==3)
       QMessageBox::warning( this, tr("Imprimir liquidación IVA"),
                             tr("PROBLEMAS al llamar a ")+programa_imprimir());

}


void liquidacioniva::latex()
{
    generalatex();
    int valor=editalatex("liquidacion_IVA");
    if (valor==1)
        QMessageBox::warning( this, tr("LIQUIDACIÓN IVA"),tr("PROBLEMAS al llamar al editor Latex"));

}


void liquidacioniva::copia()
{
    QClipboard *cb = QApplication::clipboard();
    QString global;

    global=nombreempresa();
    global+="\n";
    global+="\n";
    global+=tr("LIQUIDACIÓN DE IVA") + "\n\n";
    global+=tr("Período de ") + ui.inicialdateEdit->date().toString("dd.MM.yyyy");
    global+=tr(" a ") + ui.finaldateEdit->date().toString("dd.MM.yyyy") + "\n\n";


    global +=  tr("IVA REPERCUTIDO") + "\n";

     // --------------------------------------------------------------------------------------------------
    global += tr("CUENTA") + "\t" + tr("Base Imponible") + "\t" +
      tr("Tipo") + "\t" + tr("Cuota") + "\t" + tr("T.R.E.") + "\t" + tr("R.E.")
      + "\t" + tr("Cuota C.Caja") + "\n";
    int pos=0;
    while (pos<ui.repercutidotable->rowCount())
        {
          global += ui.repercutidotable->item(pos,0)->text();
          global += "\t";
          global += ui.repercutidotable->item(pos,1)->text();
          global += "\t";
          global += ui.repercutidotable->item(pos,2)->text();
          global += "\t";
          if (ui.repercutidotable->item(pos,6)->text().isEmpty())
             global += ui.repercutidotable->item(pos,3)->text();
          global += "\t";
          global += ui.repercutidotable->item(pos,4)->text();
          global += "\t";
          global += ui.repercutidotable->item(pos,5)->text();
          global += "\t";
          global += ui.repercutidotable->item(pos,6)->text();
          global += "\n ";
         pos++;
       }
    global += tr("Total IVA repercutido:  ");
    global += "\t";
    global += ui.ivalineEdit->text();
    global += "\n";
    global += tr(" - Total recargo equivalencia: \t");
    global += ui.relineEdit->text();
    global += "\n";
    global += "\n";

    // --------------------------------------------------------------------------------------

    global+= tr("IVA SOPORTADO") + "\n\n";

    global+=tr("CUENTA\t") + tr("Base Imponible\t") + tr("Tipo\t") + tr("Cuota\t");
    global+=tr("Total\t") + tr("Inversión\t") + tr("Cuota efectiva") + tr("Cuota C.Caja") + "\n";
    pos=0;
    while (pos<ui.soportadotable->rowCount())
        {
          global += ui.soportadotable->item(pos,0)->text();
          global += "\t";
          global += ui.soportadotable->item(pos,1)->text();
          global += "\t";
          global += ui.soportadotable->item(pos,2)->text();
          global += "\t";
          global += ui.soportadotable->item(pos,3)->text();
          global += "\t";
          global += ui.soportadotable->item(pos,4)->text();
          global += "\t";
          QString estrella=(ui.soportadotable->item(pos,5)!=0) ? ui.soportadotable->item(pos,5)->text() : "";
          global += estrella;
          global += "\t";
          if (ui.soportadotable->item(pos,7)->text().isEmpty())
             {
              global += (ui.soportadotable->item(pos,6)!=0) ?
                    ui.soportadotable->item(pos,6)->text() : "";
             }
          global += "\t";
          global += ui.soportadotable->item(pos,7)->text();
          global += "\n ";
         // QMessageBox::information( this, tr("INFO"),ui.soportadotable->item(pos,0)->text());
         pos++;
       }
    global += "\n";
    global += ui.cuotasoptextLabel->text();
    global += "\t";
    global += ui.cuotalineEdit->text();
    global += "\n";

    global += "\n";

    //------------------------------------------------------------------------
    //------------- IVA SOPORTADO EN IMPORTACIONES

    global+= tr("IVA SOPORTADO EN IMPORTACIONES") + "\n\n";

    global+=tr("CUENTA\t") + tr("Base Imponible\t") + tr("Tipo\t") + tr("Cuota\t");
    global+=tr("Total\t") + tr("Inversión\t") + tr("Cuota efectiva") + "\n";
    pos=0;
    while (pos<ui.imp_soportadotable->rowCount())
        {
          global += ui.imp_soportadotable->item(pos,0)->text();
          global += "\t";
          global += ui.imp_soportadotable->item(pos,1)->text();
          global += "\t";
          global += ui.imp_soportadotable->item(pos,2)->text();
          global += "\t";
          global += ui.imp_soportadotable->item(pos,3)->text();
          global += "\t";
          global += ui.imp_soportadotable->item(pos,4)->text();
          global += "\t";
          QString estrella=(ui.imp_soportadotable->item(pos,5)!=0) ?
                           ui.imp_soportadotable->item(pos,5)->text() : "";
          global += estrella;
          global += "\t";
          global += (ui.imp_soportadotable->item(pos,6)!=0) ?
                    ui.imp_soportadotable->item(pos,6)->text() : "";
          global += "\n ";
         // QMessageBox::information( this, tr("INFO"),ui.soportadotable->item(pos,0)->text());
         pos++;
       }
    global += "\n";
    global += ui.cuotasop_imptextLabel->text();
    global += "\t";
    global += ui.cuota_implineEdit->text();
    global += "\n";

    global += "\n";
    //----------------------------------------------------------------------------------------------------------

    global += tr("ADQUISICIONES INTRACOMUNITARIAS DE BIENES");
    global += "\n";

     // -------------------------------------------------------------------------------------------------------
    global += tr("CUENTA") + "\t" + tr("Base Imponible") +
       "\t" + tr("Tipo") + "\t" + tr("Cuota Sop.") +
       "\t" + tr("Cuota Rep") + "\t" + tr("Inversión") + "\n";
    pos=0;
    while (pos<ui.aibtable->rowCount())
        {
          global += ui.aibtable->item(pos,0)->text();
          global += "\t";
          global += ui.aibtable->item(pos,1)->text();
          global += "\t";
          global += ui.aibtable->item(pos,2)->text();
          global += "\t";
          global += ui.aibtable->item(pos,3)->text();
          global += "\t";
          global += ui.aibtable->item(pos,4)->text();
          global += "\t";
          QString estrella=(ui.aibtable->item(pos,5)!=0) ? ui.aibtable->item(pos,5)->text() : "";
          global += estrella;
          global += "\n ";
          pos++;
        }
    global += "\n";
    global += "\n";


// -----------------------------------------------------------------------------------

    global += tr("ADQUISICIONES INTRACOMUNITARIAS DE SERVICIOS") + "\n";

     // -------------------------------------------------------------------------------------------------------
    global += tr("CUENTA") + "\t" + tr("Base Imponible") +
       "\t" + tr("Tipo") +"\t" + tr("Cuota Sop.") +
       "\t" + tr("Cuota Rep") + "\n";
    pos=0;
    while (pos<ui.autofacturastable->rowCount())
        {
          global += ui.autofacturastable->item(pos,0)->text();
          global += "\t";
          global += ui.autofacturastable->item(pos,1)->text();
          global += "\t";
          global += ui.autofacturastable->item(pos,2)->text();
          global += "\t";
          global += ui.autofacturastable->item(pos,3)->text();
          global += "\t";
          global += ui.autofacturastable->item(pos,4)->text();
          global += "\n ";
          pos++;
        }
    global += "\n";
    global += "\n";

// -----------------------------------------------------------------------------------
    global += tr("INVERSIÓN SUJETO PASIVO OP. EXTERIORES UE");
    global += "\n";

     // -------------------------------------------------------------------------------------------------------
    global += tr("CUENTA") + "\t" + tr("Base Imponible") +
       "\t" + tr("Tipo") + "\t" + tr("Cuota Sop.") +
       "\t" + tr("Cuota Rep") + "\n";
    pos=0;
    while (pos<ui.autofacturas_exttable->rowCount())
        {
          global +=  ui.autofacturas_exttable->item(pos,0)->text();
          global += "\t";
          global +=  ui.autofacturas_exttable->item(pos,1)->text();
          global += "\t";
          global += ui.autofacturas_exttable->item(pos,2)->text();
          global += "\t";
          global += ui.autofacturas_exttable->item(pos,3)->text();
          global += "\t";
          global += ui.autofacturas_exttable->item(pos,4)->text();
          global += "\n";
          pos++;
        }

    global += "\n";
    global += "\n";
// -----------------------------------------------------------------------------------


    // -----------------------------------------------------------------------------------
        global += tr("INVERSIÓN SUJETO PASIVO EN OPERACIONES INTERIORES");
        global += "\n";

         // -------------------------------------------------------------------------------------------------------
        global += tr("CUENTA") + "\t" + tr("Base Imponible") +
           "\t" + tr("Tipo") + "\t" + tr("Cuota Sop.") +
           "\t" + tr("Cuota Rep") + "\n";
        pos=0;
        while (pos<ui.autofacturas_interiorestable->rowCount())
            {
              global +=  ui.autofacturas_interiorestable->item(pos,0)->text();
              global += "\t";
              global +=  ui.autofacturas_interiorestable->item(pos,1)->text();
              global += "\t";
              global += ui.autofacturas_interiorestable->item(pos,2)->text();
              global += "\t";
              global += ui.autofacturas_interiorestable->item(pos,3)->text();
              global += "\t";
              global += ui.autofacturas_interiorestable->item(pos,4)->text();
              global += "\n";
              pos++;
            }

    // -----------------------------------------------------------------------------------


    global += "\n";
    global += "\n";


    // -----------------------------------------------------------------------------------
        global += tr("IVA COMPENSACIÓN EN RÉGIMEN AGRARIO/PESCA");
        global += "\n";

         // -------------------------------------------------------------------------------------------------------
        global += tr("CUENTA") + "\t" + tr("Base Imponible") +
           "\t" + tr("Tipo") + "\t" + tr("Cuota Sop.") +
           "\t" + tr("Total") + "\n";
        pos=0;
        while (pos<ui.agrariotableWidget->rowCount())
            {
              global +=  ui.agrariotableWidget->item(pos,0)->text();
              global += "\t";
              global +=  ui.agrariotableWidget->item(pos,1)->text();
              global += "\t";
              global += ui.agrariotableWidget->item(pos,2)->text();
              global += "\t";
              global += ui.agrariotableWidget->item(pos,3)->text();
              global += "\t";
              global += ui.agrariotableWidget->item(pos,4)->text();
              global += "\n";
              pos++;
            }

    // -----------------------------------------------------------------------------------


    global += "\n";
    global += "\n";


    global +=  tr("Entregas intracomunitarias de bienes:  ") + "\t" +
         ui.eiblineEdit->text();
    global += "\n";

    global += "\n";

    global +=  tr("Prestaciones intracomunitarias de servicios:  ") + "\t" +
         ui.pr_servlineEdit->text();
    global += "\n";

    global += "\n";

    global +=  tr("Exportaciones bienes y servicios:  ") + "\t" +
         ui.exportlineEdit->text();
    global += "\n";

    global += "\n";

    global +=  tr("Exentas derecho deducción:  ") + "\t" +
         ui.exentas_deduclineEdit->text();
    global += "\n";


    global += "\n";

    global +=  tr("Exentas no derecho deducción:  ") + "\t" +
         ui.exentas_no_deduclineEdit->text();
    global += "\n";


    cb->setText(global);
    QMessageBox::information( this, tr("Liquidación de IVA"),
                              tr("Se ha pasado el contenido al portapapeles") );
}


void liquidacioniva::ver_iva_caja_soportado_corrientes()
{
 ts_caja_corr->exec();
}


void liquidacioniva::ver_iva_caja_soportado_inversion()
{
 ts_caja_inv->exec();
}


void liquidacioniva::ver_iva_caja_repercutido()
{
 ts_caja_rep->exec();
}


void liquidacioniva::gen_asiento()
{
  tabla_asientos *t = new tabla_asientos(comadecimal,decimales,usuario);

  //void pasadatos1( int fila, codigocuenta, concepto, debe,
  //                 haber, documento, ci);
  // se encarga de crear todas las celdas
  QString concepto=tr("Regularización IVA");
  t->marca_regul_iva();
  int pos=0;
  int fila=0;
  while (pos<ui.repercutidotable->rowCount())
      {
        if (!existesubcuenta(ui.repercutidotable->item(pos,0)->text()))
           {
            pos++;
            continue;
           }
        double cuota_mas_re=0;
        cuota_mas_re=convapunto(ui.repercutidotable->item(pos,3)->text()).toDouble() +
                     convapunto(ui.repercutidotable->item(pos,5)->text()).toDouble();
        QString debe=ui.repercutidotable->item(pos,6)->text().isEmpty() ?
                     formateanumero(cuota_mas_re,comadecimal,decimales) :
                     ui.repercutidotable->item(pos,6)->text();
        QString conceptoad=tr(" al tipo ");
        conceptoad+=ui.repercutidotable->item(pos,2)->text();
        // hay RE ?
        if (convapunto(ui.repercutidotable->item(pos,4)->text()).toDouble()>0.0001)
           {
             conceptoad+=tr(" RE al ") ;
             conceptoad+=ui.repercutidotable->item(pos,4)->text();
           }
        if (!ui.repercutidotable->item(pos,6)->text().isEmpty())
            conceptoad+=tr(" régimen especial criterio de caja");
        if (convapunto(debe).toDouble()>0.001 || convapunto(debe).toDouble()<-0.001 )
          {
           t->pasadatos1(fila,ui.repercutidotable->item(pos,0)->text(),concepto+conceptoad,debe,"","","");
           fila++;
          }
        pos++;
       }

  pos=0;
  concepto=tr("Regularización IVA");
  while (pos<ui.soportadotable->rowCount())
      {
       if (!existesubcuenta(ui.soportadotable->item(pos,0)->text()))
         {
          pos++;
          continue;
         }
       QString haber=ui.soportadotable->item(pos,7)->text().isEmpty() ?
                      ui.soportadotable->item(pos,6)->text() :
                      ui.soportadotable->item(pos,7)->text();

       QString conceptoad=tr(" al tipo ");
       conceptoad+=ui.soportadotable->item(pos,2)->text();
       if (!ui.soportadotable->item(pos,5)->text().isEmpty())
          conceptoad+= tr(" bienes inversión");
       if (!ui.soportadotable->item(pos,7)->text().isEmpty())
          conceptoad+= tr(" régimen especial criterio de caja");
       // t->pasadatos1(fila,ui.soportadotable->item(pos,0)->text(),concepto+conceptoad,"",haber,"","");
       if (convapunto(haber).toDouble()>0.001 || convapunto(haber).toDouble()<-0.001 )
         {
          t->pasadatos1(fila,ui.soportadotable->item(pos,0)->text(),concepto+conceptoad,"",haber,"","");
          fila++;
         }
       pos++;
     }
  //------------------------------------------------------------------------
  //------------- IVA SOPORTADO EN IMPORTACIONES

  pos=0;
  concepto=tr("Regularización IVA importaciones ");
  while (pos<ui.imp_soportadotable->rowCount())
      {
      if (!existesubcuenta(ui.imp_soportadotable->item(pos,0)->text()))
        {
         pos++;
         continue;
        }
      QString haber=ui.imp_soportadotable->item(pos,6)->text();

      QString conceptoad=tr("al tipo ");
      conceptoad+=ui.imp_soportadotable->item(pos,2)->text();
      if (!ui.imp_soportadotable->item(pos,5)->text().isEmpty())
         conceptoad+= tr(" bienes inversión");
      t->pasadatos1(fila,ui.imp_soportadotable->item(pos,0)->text(),concepto+conceptoad,"",haber,"","");
      pos++; fila++;
     }

  //----------------------------------------------------------------------------------------------------------

  // "ADQUISICIONES INTRACOMUNITARIAS DE BIENES"
  pos=0;
  concepto=tr("Regularización IVA AIB");
  while (pos<ui.aibtable->rowCount())
      {
       if (!existesubcuenta(ui.aibtable->item(pos,0)->text()))
        {
         pos++;
         continue;
        }
       QString haber=ui.aibtable->item(pos,3)->text();
       QString debe=ui.aibtable->item(pos,4)->text();
       QString conceptoad;
       if (!ui.aibtable->item(pos,5)->text().isEmpty())
           conceptoad=tr(" bien inversión");
       t->pasadatos1(fila,ui.aibtable->item(pos,0)->text(),concepto+conceptoad,debe,haber,"","");
       pos++; fila++;
      }

// -----------------------------------------------------------------------------------
// "ADQUISICIONES INTRACOMUNITARIAS DE SERVICIOS"

  pos=0;
  concepto=tr("Regularización IVA Adquisiciones intracomunitarias de servicios");
  while (pos<ui.autofacturastable->rowCount())
      {
      if (!existesubcuenta(ui.autofacturastable->item(pos,0)->text()))
       {
        pos++;
        continue;
       }
      QString haber=ui.autofacturastable->item(pos,3)->text();
      QString debe=ui.autofacturastable->item(pos,4)->text();
      t->pasadatos1(fila,ui.autofacturastable->item(pos,0)->text(),concepto,debe,haber,"","");
      pos++; fila++;
      }

// -----------------------------------------------------------------------------------
// "COMPRAS DE SERVICIOS EXTERIORES UE"
  pos=0;
  concepto=tr("Regularización IVA compra servicios exteriores UE");
  while (pos<ui.autofacturas_exttable->rowCount())
      {
      if (!existesubcuenta(ui.autofacturas_exttable->item(pos,0)->text()))
       {
        pos++;
        continue;
       }
      QString haber=ui.autofacturas_exttable->item(pos,3)->text();
      QString debe=ui.autofacturas_exttable->item(pos,4)->text();
      t->pasadatos1(fila,ui.autofacturas_exttable->item(pos,0)->text(),concepto,debe,haber,"","");
      pos++; fila++;
      }

// -----------------------------------------------------------------------------------


// -----------------------------------------------------------------------------------
// "INVERSIÓN SUJETO PASIVO EN OPERACIONES INTERIORES"
      pos=0;
      concepto=tr("Regularización IVA ISP operaciones interiores");
      while (pos<ui.autofacturas_interiorestable->rowCount())
          {
          if (!existesubcuenta(ui.autofacturas_interiorestable->item(pos,0)->text()))
           {
            pos++;
            continue;
           }
          QString haber=ui.autofacturas_interiorestable->item(pos,3)->text();
          QString debe=ui.autofacturas_interiorestable->item(pos,4)->text();
          t->pasadatos1(fila,ui.autofacturas_interiorestable->item(pos,0)->text(),concepto,debe,haber,"","");
          pos++; fila++;
          }

  // -----------------------------------------------------------------------------------
  //"IVA COMPENSACIÓN EN RÉGIMEN AGRARIO/PESCA"
      concepto=tr("Regularización IVA compensación en régimen agrario/pesca");
      pos=0;
      while (pos<ui.agrariotableWidget->rowCount())
          {
          if (!existesubcuenta(ui.agrariotableWidget->item(pos,0)->text()))
           {
            pos++;
            continue;
           }
          QString haber=ui.agrariotableWidget->item(pos,3)->text();
          QString debe;
          t->pasadatos1(fila,ui.agrariotableWidget->item(pos,0)->text(),concepto,debe,haber,"","");
          pos++; fila++;
          }

  // -----------------------------------------------------------------------------------
t->pasafechaasiento(ui.finaldateEdit->date());
t->chequeatotales();
t->exec();
}

bool liquidacioniva::genfich303(QString qnombre)
{
 QFile doc( adapta(qnombre)  );
   if ( !doc.open( QIODevice::WriteOnly ) )
      {
       QMessageBox::warning( this, tr("Fichero 303"),tr("Error: Imposible grabar fichero"));
       return false;
      }
   QTextStream docstream( &doc );
   docstream.setEncoding(QStringConverter::Latin1);

   QString contenido="<T3030";

   QString cadanyo;
   cadanyo.setNum(ui.finaldateEdit->date().year());
   contenido+=cadanyo.trimmed();

   // PERÍODO
   QString periodo=ui.periodocomboBox->currentText().left(2);
   contenido+=periodo;

   contenido+="0000>";
   contenido+="<AUX>";
   QString str; str.fill(' ', 70);
   contenido+=str;
   contenido+="3000";
   str.clear(); str.fill(' ',4);
   contenido+=str;
   str.clear(); str.fill(' ',9); // nif empresa desarrollo
   contenido+=str;
   str.clear(); str.fill(' ',213);
   contenido+=str;
   contenido+="</AUX>";
   // ----------------------------------------
   contenido+="<T30301000>";
   contenido+=" "; // indicador de página en blanco
   // tipo declaración
   //if (ui.resultadolineEdit->text().toDouble()>0) contenido+="C";
   //  else contenido+="I";
   // QMessageBox::warning( this, tr("Fichero 303"),tr("PARADA"));
   contenido+=tipo_operacion;
   //nif=completacadnum(nif,9);

   contenido+=completacadnum(nif,9);
   QString nombred;
   nombred=razon;
   if (!nombre.isEmpty()) nombred=nombred+" "+nombre;
   nombred=completacadcad(adaptacad_aeat(nombred),80);
   contenido+=nombred;

   contenido+=cadanyo.trimmed(); // ejercicio
   contenido+=periodo; // periodo

   contenido+="2"; // tributación no exclusivamente foral

   contenido+= redeme ? "1" : "2"; // redeme
   contenido+= "3"; // solo régimen general
   contenido+= "2"; // no autoliquidación conjunta
   contenido+="2"; // no criterio caja
   contenido+="2"; // no destinatario criterio caja
   contenido+="2"; // no prorrata especial
   contenido+="2"; // no revocación prorrata especial

   contenido+= "2"; // no en concurso acreedores
   str.clear(); str.fill(' ',8);
   contenido+=str; // fecha concurso
   contenido+=" "; // no auto concurso

   contenido+="2"; // no acogido SII voluntariamente

   // Exonerado 390 -- 0 para todos menos 12 y 4T - resto valor 2 (NO)
   if (periodo=="4T" || periodo=="12") contenido+="2"; //contenido+="2";
      else contenido+="0";

   // volumen operaciones -- 0 para todos menos 12 y 4T - resto 1 SI
   if (periodo=="4T" || periodo=="12") contenido+="0";
      else contenido+="0";

   // iva repercutido *****************
   double base0=0,base1=0,base5=0,base2=0,base3=0,tipo1=0,tipo2=0,tipo3=0,cuota0=0,cuota1=0,cuota5=0,cuota2=0,cuota3=0;
   double base175re=0, cuota175re=0, base1re=0, base2re=0, base3re=0, tipo1re=0, tipo2re=0, tipo3re=0, cuota1re=0, cuota2re=0, cuota3re=0;
   for (int veces=0; veces<ui.repercutidotable->rowCount(); veces++)
     {
      if (ui.repercutidotable->item(veces,0)==NULL) continue;
      if (ui.repercutidotable->item(veces,1)==NULL) continue;
      if (ui.repercutidotable->item(veces,2)==NULL) continue;
      if (ui.repercutidotable->item(veces,3)==NULL) continue;
      double valor=convapunto(ui.repercutidotable->item(veces,2)->text()).toDouble();
      if (valor<0.001) {
          //suponemos tipo cero nuevo
          base0+=convapunto(ui.repercutidotable->item(veces,1)->text()).toDouble();
          cuota0+=convapunto(ui.repercutidotable->item(veces,3)->text()).toDouble();
      }
      if (valor<5 && valor>0.001)
        {
         // suponemos tipo superreducido
          tipo1=valor;
          base1+=convapunto(ui.repercutidotable->item(veces,1)->text()).toDouble();
          cuota1+=convapunto(ui.repercutidotable->item(veces,3)->text()).toDouble();
        }
      if (valor<10 && valor>4.001)
        {
         // suponemos tipo 5 nuevo
          base5+=convapunto(ui.repercutidotable->item(veces,1)->text()).toDouble();
          cuota5+=convapunto(ui.repercutidotable->item(veces,3)->text()).toDouble();
        }
      // -----------------
      if (valor<17 && valor>6)
        {
         // suponemos tipo reducido
          tipo2=valor;
          base2+=convapunto(ui.repercutidotable->item(veces,1)->text()).toDouble();
          cuota2+=convapunto(ui.repercutidotable->item(veces,3)->text()).toDouble();
        }
      if (valor>17)
        {
         // suponemos tipo normal
          tipo3=valor;
          base3+=convapunto(ui.repercutidotable->item(veces,1)->text()).toDouble();
          cuota3+=convapunto(ui.repercutidotable->item(veces,3)->text()).toDouble();
        }

      if (ui.repercutidotable->item(veces,4)==NULL) continue;
      if (ui.repercutidotable->item(veces,5)==NULL) continue;

      valor=convapunto(ui.repercutidotable->item(veces,4)->text()).toDouble();
      //if (valor<0.01) continue;
      if (valor<0.176 && valor>0.174) {
          base175re=convapunto(ui.repercutidotable->item(veces,1)->text()).toDouble();
          cuota175re=convapunto(ui.repercutidotable->item(veces,5)->text()).toDouble();
      }
      if (valor<0.9 && !(valor<0.176 && valor>0.174) && valor>0)
         {
          // suponemos tipo superreducido
          tipo1re=valor;
          base1re=convapunto(ui.repercutidotable->item(veces,1)->text()).toDouble();
          cuota1re+=convapunto(ui.repercutidotable->item(veces,5)->text()).toDouble();
         }
      if (valor<2 && valor>0.89)
         {
          // suponemos tipo reducido
          tipo2re=valor;
          base2re=convapunto(ui.repercutidotable->item(veces,1)->text()).toDouble();
          cuota2re+=convapunto(ui.repercutidotable->item(veces,5)->text()).toDouble();
         }
      if (valor>2)
         {
          // suponemos tipo normal
          tipo3re=valor;
          base3re=convapunto(ui.repercutidotable->item(veces,1)->text()).toDouble();
          cuota3re+=convapunto(ui.repercutidotable->item(veces,5)->text()).toDouble();
         }
     }
   //QMessageBox::warning( this, tr("Fichero 347"),tr("PARADA"));
   // pasamos valores de IVA repercutido
   // ------------------------------------------
   // repercutido tipo 0

   contenido+=cadnum2dec_cadena_signo(base0,17);
   contenido+=cadnum2dec_cadena_signo(0,5);
   contenido+=cadnum2dec_cadena_signo(cuota0,17);

   // tipo superreducido 4
   contenido+=cadnum2dec_cadena_signo(base1,17);
   //contenido+=cadnum2dec_cadena_signo(tipo1,5);
   contenido+="00400";
   contenido+=cadnum2dec_cadena_signo(cuota1,17);

   // tipo 5
   contenido+=cadnum2dec_cadena_signo(base5,17);
   contenido+=cadnum2dec_cadena_signo(5,5);
   contenido+=cadnum2dec_cadena_signo(cuota5,17);

   // tipo 10
   contenido+=cadnum2dec_cadena_signo(base2,17);
   //contenido+=cadnum2dec_cadena_signo(tipo2,5);
   contenido+="01000";
   contenido+=cadnum2dec_cadena_signo(cuota2,17);

   // tipo normal 21
   contenido+=cadnum2dec_cadena_signo(base3,17);
   //contenido+=cadnum2dec_cadena_signo(tipo3,5);
   contenido+="02100";
   contenido+=cadnum2dec_cadena_signo(cuota3,17);

   // ahora vamos a por las AIB
   double base_aib=0,base_aib_corrientes=0, base_aib_inversion=0,sop_aib_corrientes=0, sop_aib_inversion=0, rep_aib=0;
   for (int veces=0; veces < ui.aibtable->rowCount(); veces++)
      {
       if (ui.aibtable->item(veces,1)==NULL) continue;
       // sumamos bases solo si no hay cuota repercutido
       if (ui.aibtable->item(veces,4)->text().isEmpty() ||
               (convapunto(ui.aibtable->item(veces,4)->text()).toDouble()<0.001
               && convapunto(ui.aibtable->item(veces,4)->text()).toDouble()>-0.001))
          {
            base_aib+=convapunto(ui.aibtable->item(veces,1)->text()).toDouble();
            if (ui.aibtable->item(veces,5)==NULL || (ui.aibtable->item(veces,5)->text()!="*"))
                base_aib_corrientes+=convapunto(ui.aibtable->item(veces,1)->text()).toDouble();
              else
                base_aib_inversion+=convapunto(ui.aibtable->item(veces,1)->text()).toDouble();
          }
       if (ui.aibtable->item(veces,5)==NULL || (ui.aibtable->item(veces,5)->text()!="*"))
         {
          sop_aib_corrientes+=convapunto(ui.aibtable->item(veces,3)->text()).toDouble();
         }
        else
          {
           sop_aib_inversion+=convapunto(ui.aibtable->item(veces,3)->text()).toDouble();
          }
       rep_aib+=convapunto(ui.aibtable->item(veces,4)->text()).toDouble();

      }

   // seguimos con las AIS
   for (int veces=0; veces < ui.autofacturastable->rowCount(); veces++)
      {
       if (ui.autofacturastable->item(veces,3)==NULL) continue;
       if (ui.autofacturastable->item(veces,4)==NULL) continue;
       if (ui.autofacturastable->item(veces,1)==NULL) continue;
       if (ui.autofacturastable->item(veces,4)->text().isEmpty() ||
               (convapunto(ui.autofacturastable->item(veces,4)->text()).toDouble()<0.001
               && convapunto(ui.autofacturastable->item(veces,4)->text()).toDouble()>-0.001))
         {
          base_aib+=convapunto(ui.autofacturastable->item(veces,1)->text()).toDouble();
          base_aib_corrientes+=convapunto(ui.autofacturastable->item(veces,1)->text()).toDouble();
         }
       sop_aib_corrientes+=convapunto(ui.autofacturastable->item(veces,3)->text()).toDouble();
       rep_aib+=convapunto(ui.autofacturastable->item(veces,4)->text()).toDouble();
      }

   contenido+=cadnum2dec_cadena_signo(base_aib,17);
   contenido+=cadnum2dec_cadena_signo(rep_aib,17);

   //QMessageBox::warning( this, tr("Fichero 303"),tr("PARADA"));

   // cogemos otras operaciones inversión sujeto pasivo
   // ui.autofacturas_exttable == adquisicions no ue
   // ui.autofacturas_interiorestable  == adquisiciones inversión interiores
   double base_ooisp=0, cuota_sop_ooisp=0, cuota_rep_ooisp=0;
   for (int veces=0; veces < ui.autofacturas_exttable->rowCount(); veces++)
      {
        if (ui.autofacturas_exttable->item(veces,3)==NULL) continue;
        if (ui.autofacturas_exttable->item(veces,4)->text().isEmpty() ||
                (convapunto(ui.autofacturas_exttable->item(veces,4)->text()).toDouble()<0.001
                && convapunto(ui.autofacturas_exttable->item(veces,4)->text()).toDouble()>-0.001))
            base_ooisp+=convapunto(ui.autofacturas_exttable->item(veces,1)->text()).toDouble();
        cuota_sop_ooisp+=convapunto(ui.autofacturas_exttable->item(veces,3)->text()).toDouble();
        cuota_rep_ooisp+=convapunto(ui.autofacturas_exttable->item(veces,4)->text()).toDouble();
      }

   for (int veces=0; veces < ui.autofacturas_interiorestable->rowCount(); veces++)
     {
       if (ui.autofacturas_interiorestable->item(veces,3)==NULL) continue;
       if (ui.autofacturas_interiorestable->item(veces,4)->text().isEmpty() ||
               (convapunto(ui.autofacturas_interiorestable->item(veces,4)->text()).toDouble()<0.001
               && convapunto(ui.autofacturas_interiorestable->item(veces,4)->text()).toDouble()>-0.001))
       base_ooisp+=convapunto(ui.autofacturas_interiorestable->item(veces,1)->text()).toDouble();
       cuota_sop_ooisp+=convapunto(ui.autofacturas_interiorestable->item(veces,3)->text()).toDouble();
       cuota_rep_ooisp+=convapunto(ui.autofacturas_interiorestable->item(veces,4)->text()).toDouble();
     }

   contenido+=cadnum2dec_cadena_signo(base_ooisp,17);
   contenido+=cadnum2dec_cadena_signo(cuota_rep_ooisp,17);


   // modificaciones bases y cuotas
   str.clear(); str.fill('0',17);
   contenido+=str;
   contenido+=str;

   // bases y tipos de recargo de equivalencia
   // tipo 175
   contenido+=cadnum2dec_cadena_signo(base175re,17);
   contenido+=cadnum2dec_cadena_signo(1.75,5);
   contenido+=cadnum2dec_cadena_signo(cuota175re,17);


   contenido+=cadnum2dec_cadena_signo(base1re,17);
   contenido+=cadnum2dec_cadena_signo(tipo1re,5);
   contenido+=cadnum2dec_cadena_signo(cuota1re,17);

   contenido+=cadnum2dec_cadena_signo(base2re,17);
   //contenido+=cadnum2dec_cadena_signo(tipo2re,5);
   contenido+="00140";
   contenido+=cadnum2dec_cadena_signo(cuota2re,17);

   contenido+=cadnum2dec_cadena_signo(base3re,17);
   //contenido+=cadnum2dec_cadena_signo(tipo3re,5);
   contenido+="00520";
   contenido+=cadnum2dec_cadena_signo(cuota3re,17);

   // modificaciones bases y cuotas RE
   contenido+=str;
   contenido+=str;

   // total cuota devengada
   double sumadevengado=cuota1+cuota2+cuota3+cuota5+rep_aib+cuota_rep_ooisp+cuota1re+cuota2re+cuota3re+cuota175re;
   contenido+=cadnum2dec_cadena_signo(sumadevengado,17);

   // soportado
   double bi_soportado_corrientes=0, bi_soportado_inversion=0, cuota_soportado_corrientes=0, cuota_soportado_inversion=0;
   for (int veces=0;veces<ui.soportadotable->rowCount();veces++)
      {
        if (ui.soportadotable->item(veces,2)->text().isEmpty()) continue;
        if (ui.soportadotable->item(veces,5)->text().isEmpty())
           {
            // bienes corrientes
            bi_soportado_corrientes+=convapunto(ui.soportadotable->item(veces,1)->text()).toDouble();
            cuota_soportado_corrientes+=convapunto(ui.soportadotable->item(veces,6)->text()).toDouble();
           }
           else
               {
                 // bienes inversión
                 bi_soportado_inversion+=convapunto(ui.soportadotable->item(veces,1)->text()).toDouble();
                 cuota_soportado_inversion+=convapunto(ui.soportadotable->item(veces,6)->text()).toDouble();
               }
       }

   // base soportado corrientes
   contenido+=cadnum2dec_cadena_signo(bi_soportado_corrientes+base_ooisp,17);
   // cuota soportado corrientes
   contenido+=cadnum2dec_cadena_signo(cuota_soportado_corrientes+cuota_sop_ooisp,17);
   // base soportado inversión
   contenido+=cadnum2dec_cadena_signo(bi_soportado_inversion,17);
   // cuota soportado corrientes
   contenido+=cadnum2dec_cadena_signo(cuota_soportado_inversion,17);


   // soportado importaciones
   double bi_soportado_corrientes_imp=0, bi_soportado_inversion_imp=0, cuota_soportado_corrientes_imp=0,
           cuota_soportado_inversion_imp=0;
   for (int veces=0;veces<ui.imp_soportadotable->rowCount();veces++)
      {
        if (ui.imp_soportadotable->item(veces,2)->text().isEmpty()) continue;
        if (ui.imp_soportadotable->item(veces,5)->text().isEmpty())
           {
            // bienes corrientes
            bi_soportado_corrientes_imp+=convapunto(ui.imp_soportadotable->item(veces,1)->text()).toDouble();
            cuota_soportado_corrientes_imp+=convapunto(ui.imp_soportadotable->item(veces,3)->text()).toDouble();
           }
           else
               {
                 // bienes inversión
                 bi_soportado_inversion_imp+=convapunto(ui.imp_soportadotable->item(veces,1)->text()).toDouble();
                 cuota_soportado_inversion_imp+=convapunto(ui.imp_soportadotable->item(veces,3)->text()).toDouble();
               }
       }

   // base soportado importaciones corrientes
   contenido+=cadnum2dec_cadena_signo(bi_soportado_corrientes_imp,17);
   // cuota soportado importaciones corrientes
   contenido+=cadnum2dec_cadena_signo(cuota_soportado_corrientes_imp,17);
   // base soportado inversión importaciones
   contenido+=cadnum2dec_cadena_signo(bi_soportado_inversion_imp,17);
   // cuota soportado corrientes importaciones
   contenido+=cadnum2dec_cadena_signo(cuota_soportado_inversion_imp,17);

   // base y cuota AIB corrintes y de inversión
   contenido+=cadnum2dec_cadena_signo(base_aib_corrientes,17);
   contenido+=cadnum2dec_cadena_signo(sop_aib_corrientes,17);
   contenido+=cadnum2dec_cadena_signo(base_aib_inversion,17);
   contenido+=cadnum2dec_cadena_signo(sop_aib_inversion,17);

   // base y cuota rectificación de deducciones
   contenido+=str;
   contenido+=str;

   // compensaciones régimen especial agricultura y pesca (cuota)
   double compens_ap=0;
   for (int veces=0; veces<ui.agrariotableWidget->rowCount(); veces++)
      {
       compens_ap+=convapunto(ui.agrariotableWidget->item(veces,3)->text()).toDouble();
      }
   contenido+=cadnum2dec_cadena_signo(compens_ap,17);

   // regularización inversiones
   contenido+=str;

   // regularización porcentaje definitivo prorrata
   contenido+=str;

   // total a deducir
   double sumadeducir=cuota_soportado_corrientes+ cuota_soportado_inversion+
           cuota_soportado_corrientes_imp+ cuota_soportado_inversion_imp+
           sop_aib_corrientes+ sop_aib_inversion+ compens_ap+cuota_sop_ooisp;
   contenido+=cadnum2dec_cadena_signo(sumadeducir,17);

   // resultado regimen general
   contenido+=cadnum2dec_cadena_signo(sumadevengado-sumadeducir,17);
   // QMessageBox::warning( this, tr("Fichero 303"),cadnum2dec_cadena_signo_signo(sumadevengado-sumadeducir,17));


   // 600 espacios
   str.clear(); str.fill(' ',600);
   contenido+=str;

   // espacio para el sello electrónico
   str.clear(); str.fill(' ',13);
   contenido+=str;

   // indicador de fin de registro
   contenido+="</T30301000>";

   // PÁGINA 3 ---- ********** ----
   contenido+="<T30303000>";

   // EIB'S
   contenido+=cadnum2dec_cadena_signo(convapunto(ui.eiblineEdit->text()).toDouble() + convapunto(ui.pr_servlineEdit->text()).toDouble(),17);
   // Exportaciones
   contenido+=cadnum2dec_cadena_signo(convapunto(ui.exportlineEdit->text()).toDouble(),17);

   // operaciones no sujetas reglas localizacion
   contenido+=cadnum2dec_cadena_signo(convapunto(ui.export_no_sujeta_lineEdit->text()).toDouble(),17);
   //QMessageBox::warning( this, tr("Fichero 303"),cadnum2dec_cadena_signo(convapunto(ui.export_no_sujeta_lineEdit->text()).toDouble(),17));
   // SUJETAS CON ISP
   contenido+=cadnum2dec_cadena_signo(convapunto(ui.sujetas_isp_lineEdit->text()).toDouble(),17);

   str.clear(); str.fill('0',17);

   //contenido+=str; // SUJETAS CON ISP
   contenido+=str; // NO SUJETAS OSS
   contenido+=str; // OSS
   //contenido+=str; // caja
   //contenido+=str; // caja


   // importes devengados art. 75 base imponible
   contenido+=str; // caja

   // importes devengados art. 75 cuota
   contenido+=str; // caja

   // importes devengados según regla gral devengo art. 75 base imponible
   contenido+=str;

   // importes devengados según regla gral devengo art. 75 cuota
   contenido+=str;

   // resultado regularización cuotas art. 80.cinco.5ª LIVA
   contenido+=str;


   // resultado
   contenido+=cadnum2dec_cadena_signo(sumadevengado-sumadeducir,17);

   // % atribuible administración del estado
   contenido+="10000";

   // atribuible administración del estado
   contenido+=cadnum2dec_cadena_signo(sumadevengado-sumadeducir,17);

   // RESULTADO IVA a la importación liquidado por la aduana pendiente de ingreso
   contenido+=str;


   // cuotas a compensar PENDIENTES de periodos anteriores
   if (compensar>0.009 || compensar<-0.009)
     contenido+=cadnum2dec_cadena_signo(compensar,17);
    else contenido+=str;

   // coutas a compensar periodos anteriores aplicadas en este periodo
   double aplicar_ejercicio=0;
   if (sumadevengado-sumadeducir<=0.001) aplicar_ejercicio=0;
      else {
            if (compensar<=sumadevengado-sumadeducir-0.001)
                aplicar_ejercicio=compensar;
            else
                aplicar_ejercicio=sumadevengado-sumadeducir;
      }
   if (aplicar_ejercicio>0.009 || aplicar_ejercicio<-0.009)
     contenido+=cadnum2dec_cadena_signo(aplicar_ejercicio,17);
    else contenido+=str;

   // cuotas a compensar periodos previos pendientes para periodos posteriores
   double compensar_periodos_posteriores=compensar-aplicar_ejercicio;

   if (compensar_periodos_posteriores>0.009)
       contenido+=cadnum2dec_cadena_signo(compensar_periodos_posteriores,17);
      else contenido+=str;

   // resultado reg. anual dip. forales
   contenido+=str;

   // resultado
   contenido+=cadnum2dec_cadena_signo(sumadevengado-sumadeducir-aplicar_ejercicio,17);

   // ingresar anteriores periodos
   contenido+=str;

   // devoluciones anteriores periodos
   contenido+=str;

   // resultado liquidación
   contenido+=cadnum2dec_cadena_signo(sumadevengado-sumadeducir-aplicar_ejercicio,17);

   // declaración sin actividad
    contenido+=" ";

  // declaración complementaria
   if (escomplementaria) contenido+="X"; else contenido +=" ";
   if (escomplementaria) contenido+=contenido+=completacadnum(declaracion_anterior,13);
      else contenido+="             ";

   str.clear(); str.fill(' ',35);
   contenido+=str;

   str.clear(); str.fill(' ',52);
   contenido+=str;

   str.clear(); str.fill(' ',513);
   contenido+=str;

   contenido+="</T30303000>";


   contenido+="<T303DID00>";


   // IBAN
   QString ccc, iban, cedente1, cedente2, nriesgo, sufijo;

   // DEVOLUCIÓN SWIFT/BIC
   // contenido+="           ";
   str.clear(); str.fill(' ',11);
   contenido+=str;

   if (!cta_banco.isEmpty())
     basedatos::instancia()->datos_cuenta_banco(cta_banco, &ccc, &iban, &cedente1, &cedente2, &nriesgo,& sufijo);

   contenido+=completacadcad(adaptacad_aeat(iban),34);

   //str.clear(); str.fill(' ',17);
   //contenido+=str;

   // Devolución Banco/Bank name
   str.clear(); str.fill(' ',70);
   contenido+=str;

   // Devolución Dirección de Banco/
   str.clear(); str.fill(' ',35);
   contenido+=str;

   // Devolución ciudad
   str.clear(); str.fill(' ',30);
   contenido+=str;

   // Devolución país
   str.clear(); str.fill(' ',2);
   contenido+=str;

   // Devolución marca sepa
   contenido+="0";

   // espacio reservado AEAT 600 blancos
   str.clear(); str.fill(' ',617);
   contenido+=str;

   // indicador fin de registro
   contenido+="</T303DID00>";

   // FIN DEL FICHERO
   contenido+="</T3030";
   contenido+=cadanyo.trimmed();
   contenido+=periodo;
   contenido+="0000>";
   //contenido+="\r\n";
  // FIN DEL fichero

   docstream << contenido;
   doc.close();

   QMessageBox::information( this, tr("Fichero 303"),tr("303 generado en archivo: ")+qnombre);

  return true;
}




void liquidacioniva::mod303()
{

    escomplementaria=false;
    essustitutiva=false;

    modelo300 m;// = new modelo300();
    int resultado=m.exec();
    m.parametros(&nif,&razon,&nombre,&escomplementaria,&essustitutiva,
                  &declaracion_anterior,&redeme,&cta_banco, &tipo_operacion, &compensar);
    //delete(m);
    if (resultado!=QDialog::Accepted) return;


#ifdef NOMACHINE
  directorio *dir = new directorio();
  dir->pasa_directorio(dirtrabajobd());
  dir->activa_pide_archivo(tr("modelo.303"));
  if (dir->exec() == QDialog::Accepted)
    {
      // QMessageBox::information( this, tr("selección"),
      //                         dir->selec_actual() );
      // QMessageBox::information( this, tr("selección"),
      //                         dir->ruta_actual() );
      QString nombre=dir->respuesta_arch();
      if (nombre.length()>0)
          {
           // QString nombre=nombre.fromLocal8Bit(fileNames.at(0));
           if (nombre.right(4)!=".303") nombre=nombre+".303";
           // QMessageBox::information( this, tr("selección"),
           //                         nombre );
           QString rutadir=dir->directorio_actual();
           if (!rutadir.endsWith(QDir::separator())) rutadir.append(QDir::separator());
           nombre=rutadir+nombre;
           // QMessageBox::information( this, tr("selección"),
           //                          nombre );
           // if (!genfich347(nombre))
           //   QMessageBox::information( this, tr("Fichero 347"),tr("NO se ha generado correctamente el fichero 303"));
           if (nombre.right(4)!=".303") nombre=nombre+".303";
           if (!genfich303(nombre))
              QMessageBox::information( this, tr("Fichero 303"),tr("NO se ha generado correctamente el fichero 303"));
          }
    }
   delete(dir);
#else


  QFileDialog dialogofich(this);
  dialogofich.setFileMode(QFileDialog::AnyFile);
  dialogofich.setOption(QFileDialog::DontConfirmOverwrite,true);
  dialogofich.setAcceptMode (QFileDialog::AcceptSave );
  dialogofich.setLabelText ( QFileDialog::LookIn, tr("Directorio:") );
  dialogofich.setLabelText ( QFileDialog::FileName, tr("Archivo:") );
  dialogofich.setLabelText ( QFileDialog::FileType, tr("Tipo de archivo:") );
  dialogofich.setLabelText ( QFileDialog::Accept, tr("Aceptar") );
  dialogofich.setLabelText ( QFileDialog::Reject, tr("Cancelar") );

  QStringList filtros;
  QString filtrotexto=tr("Archivos 303 (*.303)");
  filtros << filtrotexto;
  filtros << tr("Archivos 303 (*.303)");
  dialogofich.setNameFilters(filtros);
  dialogofich.setDirectory(adapta(dirtrabajo()));
  dialogofich.setWindowTitle(tr("ARCHIVO 303"));
  // dialogofich.exec();
  //QString fileName = dialogofich.getOpenFileName(this, tr("Seleccionar archivo para importar asientos"),
  //                                              dirtrabajo,
  //                                              tr("Ficheros de texto (*.txt)"));
  QStringList fileNames;
  if (dialogofich.exec())
     {
      fileNames = dialogofich.selectedFiles();
      // QMessageBox::information( this, tr("EXPORTAR PLAN"),dialogofich.selectedNameFilter () );
      if (fileNames.at(0).length()>0)
          {
           // QString nombre=nombre.fromLocal8Bit(fileNames.at(0));
           QString nombre=fileNames.at(0);
           if (nombre.right(4)!=".303") nombre=nombre+".303";
           QFile doc( adapta(nombre)  );
           if (doc.exists() && QMessageBox::question(this,
                 tr("¿ Sobreescribir ?"),
                 tr("'%1' ya existe."
                "¿ Desea sobreescribirlo ?")
                 .arg( nombre ),
                 tr("&Sí"), tr("&No"),
                 QString(), 0, 1 ))
                     return;
           if (!genfich303(nombre))
              QMessageBox::information( this, tr("Fichero 303"),tr("NO se ha generado correctamente el fichero 303"));
          }
       }
#endif


}


void liquidacioniva::actufechas()
{
  QString cadcombo=ui.periodocomboBox->currentText();
  QString periodo=cadcombo.section(' ',0,0);

  QDate fechaini=inicioejercicio(ui.ejerciciocomboBox->currentText());
  QDate fechafin=finejercicio(ui.ejerciciocomboBox->currentText());
  int anyo=fechaini.year();

    if (periodo=="1T")
      {
        fechaini.setDate(anyo,1,1);
        fechafin.setDate(anyo,3,31);
      }
    if (periodo=="2T")
     {
        fechaini.setDate(anyo,4,1);
        fechafin.setDate(anyo,6,30);
     }
    if (periodo=="3T")
     {
        fechaini.setDate(anyo,7,1);
        fechafin.setDate(anyo,9,30);
     }
    if (periodo=="4T")
     {
        fechaini.setDate(anyo,10,1);
        fechafin.setDate(anyo,12,31);
     }
    if (!periodo.contains('T'))
      {
        int mes=periodo.toInt();
        if (mes>0)
          {
            fechaini.setDate(anyo,mes,1);
            if (mes<12)
              {
               fechafin.setDate(anyo,mes+1,1);
               fechafin=fechafin.addDays(-1);
              }
              else
                   {
                     fechafin.setDate(anyo,12,31);
                   }
          }
      }

  ui.inicialdateEdit->setDate(fechaini);
  ui.finaldateEdit->setDate(fechafin);
}

void liquidacioniva::consulta_mayor() {
 consmayor *c = new consmayor(comadecimal,decimales,usuario);
 c->exec();
 delete(c);
}



void liquidacioniva::informe()
{

    QStringList conceptos;
    QStringList cuentas;
    QStringList bis;
    QStringList tipos;
    QStringList cuotas_sop;
    QStringList cuotas_rep;
    QStringList tres;
    QStringList res;
    QStringList binversion;
    QStringList cuotas_efectivas;
    QStringList cuotas_criterio_caja;

    int registros=0;
    int pos=0;
    while (pos<ui.repercutidotable->rowCount())
        {
          conceptos << tr("IVA REPERCUTIDO");
          cuentas << ui.repercutidotable->item(pos,0)->text();
          bis << convapunto(ui.repercutidotable->item(pos,1)->text());
          tipos << convapunto(ui.repercutidotable->item(pos,2)->text());
          cuotas_sop << "";
          cuotas_rep << convapunto(ui.repercutidotable->item(pos,3)->text());
          tres << convapunto(ui.repercutidotable->item(pos,4)->text());
          res << convapunto(ui.repercutidotable->item(pos,5)->text());
          binversion << "";
          cuotas_efectivas << "";
          if (ui.repercutidotable->item(pos,7)!=NULL)
            cuotas_criterio_caja << convapunto(ui.repercutidotable->item(pos,7)->text());
           else cuotas_criterio_caja << "";
          pos++;
       }
    //QMessageBox::information( this, tr("INFO"),"stop");

    registros+=pos;
    pos=0;
    while (pos<ui.soportadotable->rowCount())
        {
          conceptos << tr("IVA SOPORTADO");
          cuentas << ui.soportadotable->item(pos,0)->text();
          bis << convapunto(ui.soportadotable->item(pos,1)->text());
          tipos << convapunto(ui.soportadotable->item(pos,2)->text());
          cuotas_sop << convapunto(ui.soportadotable->item(pos,3)->text());
          cuotas_rep << "";
          tres << "";
          res << "";
          QString estrella=(ui.soportadotable->item(pos,5)!=0) ? ui.soportadotable->item(pos,5)->text() : "";
          binversion << estrella;
          if (ui.soportadotable->item(pos,6)!=NULL)
            cuotas_efectivas << convapunto(ui.soportadotable->item(pos,6)->text());
           else cuotas_efectivas << "";
          if (ui.soportadotable->item(pos,7)!=NULL)
            cuotas_criterio_caja << convapunto(ui.soportadotable->item(pos,7)->text());
            else cuotas_criterio_caja << "";
          pos++;
        }


   // ----------------------------------------------------------------------------------
   // --- IVA SOPORTADO EN IMPORTACIONES
    registros+=pos;

    pos=0;
    while (pos<ui.imp_soportadotable->rowCount())
        {
          conceptos << tr("IVA SOPORTADO IMPORTACIONES");
          cuentas << ui.imp_soportadotable->item(pos,0)->text();
          bis << convapunto(ui.imp_soportadotable->item(pos,1)->text());
          tipos << convapunto(ui.imp_soportadotable->item(pos,2)->text());
          cuotas_sop << convapunto(ui.imp_soportadotable->item(pos,3)->text());
          cuotas_rep << "";
          tres << "";
          res << "";
          QString estrella=(ui.imp_soportadotable->item(pos,5)!=0) ? ui.imp_soportadotable->item(pos,5)->text() : "";
          binversion << estrella;
          if (ui.imp_soportadotable->item(pos,6)!=NULL)
             cuotas_efectivas << convapunto(ui.imp_soportadotable->item(pos,6)->text());
           else cuotas_efectivas << "";
          cuotas_criterio_caja << "";
          pos++;
       }

    // ----------------------------------------------------------------------------------
    registros+=pos;
    pos=0;
    while (pos<ui.aibtable->rowCount())
        {
          conceptos << tr("ADQUISICIONES INTRACOMUNITARIAS DE BIENES");
          cuentas << ui.aibtable->item(pos,0)->text();
          bis << convapunto(ui.aibtable->item(pos,1)->text());
          tipos << convapunto(ui.aibtable->item(pos,2)->text());
          cuotas_sop << convapunto(ui.aibtable->item(pos,3)->text());
          cuotas_rep << convapunto(ui.aibtable->item(pos,4)->text());
          tres << "";
          res << "";
          QString estrella=(ui.aibtable->item(pos,5)!=0) ? ui.aibtable->item(pos,5)->text() : "";
          binversion << estrella;
          cuotas_efectivas << "";
          cuotas_criterio_caja << "";
          pos++;
        }
    registros+=pos;

    pos=0;
    while (pos<ui.autofacturastable->rowCount())
        {
          conceptos << tr("ADQUISICIONES INTRACOMUNITARIAS DE SERVICIOS");
          cuentas << ui.autofacturastable->item(pos,0)->text();
          bis << convapunto(ui.autofacturastable->item(pos,1)->text());
          tipos << convapunto(ui.autofacturastable->item(pos,2)->text());
          cuotas_sop << convapunto(ui.autofacturastable->item(pos,3)->text());
          cuotas_rep << convapunto(ui.autofacturastable->item(pos,4)->text());
          tres << "";
          res << "";
          binversion << "";
          cuotas_efectivas << "";
          cuotas_criterio_caja << "";
          pos++;
        }
    registros+=pos;

// -----------------------------------------------------------------------------------
    pos=0;
    while (pos<ui.autofacturas_exttable->rowCount())
        {
          conceptos << tr("COMPRAS DE SERVICIOS EXTERIORES UE");
          cuentas << ui.autofacturas_exttable->item(pos,0)->text();
          bis << convapunto(ui.autofacturas_exttable->item(pos,1)->text());
          tipos << convapunto(ui.autofacturas_exttable->item(pos,2)->text());
          cuotas_sop << convapunto(ui.autofacturas_exttable->item(pos,3)->text());
          cuotas_rep << convapunto(ui.autofacturas_exttable->item(pos,4)->text());
          tres << "";
          res << "";
          binversion << "";
          cuotas_efectivas << "";
          cuotas_criterio_caja << "";
          pos++;
        }
    registros+=pos;

// -----------------------------------------------------------------------------------
// ISP operaciones interiores

    // -----------------------------------------------------------------------------------
        pos=0;
        while (pos<ui.autofacturas_interiorestable->rowCount())
            {
              conceptos << tr("INVERSIÓN SUJETO PASIVO OPERACIONES INTERIORES");
              cuentas << ui.autofacturas_interiorestable->item(pos,0)->text();
              bis << convapunto(ui.autofacturas_interiorestable->item(pos,1)->text());
              tipos << convapunto(ui.autofacturas_interiorestable->item(pos,2)->text());
              cuotas_sop << convapunto(ui.autofacturas_interiorestable->item(pos,3)->text());
              cuotas_rep << convapunto(ui.autofacturas_interiorestable->item(pos,4)->text());
              tres << "";
              res << "";
              binversion << "";
              cuotas_efectivas << "";
              cuotas_criterio_caja << "";
              pos++;
            }
     registros+=pos;
    // -----------------------------------------------------------------------------------

     // Operaciones régimen agrario/pesca

         // -----------------------------------------------------------------------------------
             pos=0;
             while (pos<ui.agrariotableWidget->rowCount())
                 {
                   conceptos << tr("IVA COMPENSACIÓN EN RÉGIMEN AGRARIO/PESCA");
                   cuentas << ui.agrariotableWidget->item(pos,0)->text();
                   bis << convapunto(ui.agrariotableWidget->item(pos,1)->text());
                   tipos << convapunto(ui.agrariotableWidget->item(pos,2)->text());
                   cuotas_sop << convapunto(ui.agrariotableWidget->item(pos,3)->text());
                   cuotas_rep << "";
                   tres << "";
                   res << "";
                   binversion << "";
                   cuotas_efectivas << "";
                   cuotas_criterio_caja << "";
                   pos++;
                 }
          registros+=pos;

        // ------------------------------------------------------------------------------------

        // QMessageBox::information( this, tr("INFO"),"stop");

        QString fileName;
        fileName =  ":/informes/liq_iva.xml" ;
        QtRPT *report = new QtRPT(this);
        if (report->loadReport(fileName) == false)
        {
            QMessageBox::information( this, tr("Liquidación de IVA"),
                                      tr("NO SE HA ENCONTRADO EL FICHERO DE INFORME") );
            return;
        }

        QObject::connect(report, &QtRPT::setDSInfo,
                         [&](DataSetInfo &dsInfo) {
            dsInfo.recordCount=conceptos.count();
        });

        QString imagen=basedatos::instancia()->logo_empresa();

          connect(report, &QtRPT::setValueImage,[&](const int recNo, const QString paramName, QImage &paramValue,
                  const int reportPage) {
            Q_UNUSED(recNo);
            Q_UNUSED(reportPage);
            if (paramName == "logo") {

                auto foto= new QImage();
                if (imagen.isEmpty()) imagen = logodefecto();
                if (imagen.length()>0)
                   {
                    QByteArray byteshexa;
                    byteshexa.append ( imagen.toUtf8() );
                    QByteArray bytes;
                    bytes=bytes.fromHex ( byteshexa );
                    foto->loadFromData( bytes, "PNG");
                   } else return;

                paramValue = *foto;
            }
        });

         connect(report, &QtRPT::setValue, [&](const int recNo, const QString paramName, QVariant &paramValue,
                 const int reportPage) {
              // campos: descrip, nota, cifra1, cifra2
              // recordCount es una lista, añadir también segunda página

              // QMessageBox::warning( this, tr("Estados Contables"),tr("Num %1").arg(recNo));
              Q_UNUSED(reportPage);

              if (paramName == "titulo") paramValue=tr("LIQUIDACIÓN DE IVA");
              if (paramName == "empresa") paramValue=nombreempresa() + " - NIF: "+basedatos::instancia()->cif();
              if (paramName == "periodo") paramValue=tr("Período de ") + ui.inicialdateEdit->date().toString("dd/MM/yyyy") +
                      tr(" a ") + ui.finaldateEdit->date().toString("dd/MM/yyyy");

              if (paramName == "decl") paramValue=ui.concepto_liq_label->text();
              if (paramName == "liquid") paramValue=convapunto(ui.resultadolineEdit->text());

              if (paramName == "concepto") paramValue = conceptos.at(recNo);
              if (paramName == "cuenta") paramValue = cuentas.at(recNo);
              if (paramName == "bi") paramValue = bis.at(recNo);
              if (paramName == "tipo") paramValue = tipos.at(recNo);
              if (paramName == "soportado") paramValue = cuotas_sop.at(recNo);
              if (paramName == "repercutido") paramValue = cuotas_rep.at(recNo);
              if (paramName == "tre") paramValue = tres.at(recNo);
              if (paramName == "re") paramValue = res.at(recNo);
              if (paramName == "inversion") paramValue = binversion.at(recNo);
              if (paramName == "efectiva") paramValue = cuotas_efectivas.at(recNo);
              if (paramName == "cuotacaja") paramValue = cuotas_criterio_caja.at(recNo);

              if (paramName == "eib") paramValue = convapunto(ui.eiblineEdit->text());
              if (paramName == "eis") paramValue = convapunto(ui.pr_servlineEdit->text());
              if (paramName == "export") paramValue = convapunto(ui.exportlineEdit->text());
              if (paramName == "ens") paramValue = convapunto(ui.export_no_sujeta_lineEdit->text());
              if (paramName == "emit_isp") paramValue = convapunto(ui.sujetas_isp_lineEdit->text());
              if (paramName == "exnoded") paramValue = convapunto(ui.exentas_no_deduclineEdit->text());

          });


          // report->printExec(true);
          if (hay_visualizador_extendido) report->printExec(true);
          else {
                 QString qfichero=dirtrabajo();
                 qfichero.append(QDir::separator());
                 qfichero=qfichero+(tr("liquid_IVA.pdf"));
                 report->printPDF(qfichero);
          }



}
