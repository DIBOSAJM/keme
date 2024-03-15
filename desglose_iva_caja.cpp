#include "desglose_iva_caja.h"
#include "ui_desglose_iva_caja.h"
#include "basedatos.h"
#include "pidefecha.h"

desglose_iva_caja::desglose_iva_caja(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::desglose_iva_caja)
{
    ui->setupUi(this);

    comadecimal=haycomadecimal();
    decimales=haydecimales();
    cambios=false;
    connect(ui->forzarpushButton,SIGNAL(clicked()),SLOT(forzar_liquidacion()));
    ui->tableWidget->setEditTriggers ( QAbstractItemView::NoEditTriggers );
}

desglose_iva_caja::~desglose_iva_caja()
{
    delete ui;
}


void desglose_iva_caja::pasa_info(QString qapunte, QString documento,
                                  QDate fecha_fra, double totalfra, double qcuotaiva)
{
    cuotaiva=qcuotaiva;
    apunte=qapunte;
    ui->doclineEdit->setText(documento);
    ui->total_facturalineEdit->setText(formateanumero(totalfra,comadecimal,decimales));
    ui->fechafacturadateEdit->setDate(fecha_fra);
    refrescar();
}


void desglose_iva_caja::refrescar()
{
    QSqlQuery q=basedatos::instancia()->datos_venci_apunte_iva_caja_liquidados(apunte);
    ui->tableWidget->clearContents();

    num_vencis.clear();

    int numorden=0;
    if ( q.isActive() ) {
       while ( q.next() )
         {

          if (ui->tableWidget->rowCount()-1<numorden) ui->tableWidget->insertRow(numorden);

          QTableWidgetItem *newItem1 = new QTableWidgetItem(q.value(0).toDate().toString("dd-MM-yyyy"));
          ui->tableWidget->setItem(numorden,0,newItem1);


          ui->tableWidget->setItem(numorden,1,new QTableWidgetItem(q.value(1).toDate().toString("dd-MM-yyyy")));

          QTableWidgetItem *newItem2 = new QTableWidgetItem(
                  q.value(2).toBool() ? "*" : "");
          newItem2->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,2,newItem2);


          QTableWidgetItem *newItem3 = new QTableWidgetItem(
               formateanumero(q.value(3).toDouble(),comadecimal,decimales));
          newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,3,newItem3);

          double cuota_liquidada=0;
          double totalfactur=convapunto(ui->total_facturalineEdit->text()).toDouble();
          if (totalfactur>-0.001 && totalfactur<0.001) cuota_liquidada=0;
           else
             cuota_liquidada=(q.value(3).toDouble()/
                           convapunto(ui->total_facturalineEdit->text()).toDouble()*
                            cuotaiva);
          cuota_liquidada=CutAndRoundNumberToNDecimals (cuota_liquidada, 4);
          QString cadnum; // redondear a varios decimales
          cadnum.setNum(cuota_liquidada,'f',4);
          if (comadecimal) cadnum=convacoma(cadnum);
          QTableWidgetItem *newItem4 = new QTableWidgetItem(cadnum);
          newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,4,newItem4);

          num_vencis << q.value(4).toString();

          numorden++;
         }
        }

   // QSqlQuery datos_venci_apunte_iva_caja_no_liquidados(QString apunte);
    q=basedatos::instancia()->datos_venci_apunte_iva_caja_no_liquidados(apunte);
    if ( q.isActive() ) {
       while ( q.next() )
         {

          if (ui->tableWidget->rowCount()-1<numorden) ui->tableWidget->insertRow(numorden);

          QTableWidgetItem *newItem1 = new QTableWidgetItem(q.value(0).toDate().toString("dd-MM-yyyy"));
          ui->tableWidget->setItem(numorden,0,newItem1);


          ui->tableWidget->setItem(numorden,1,new QTableWidgetItem(""));

          QTableWidgetItem *newItem2 = new QTableWidgetItem("");
          newItem2->setTextAlignment (Qt::AlignCenter | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,2,newItem2);


          QTableWidgetItem *newItem3 = new QTableWidgetItem(
               formateanumero(q.value(1).toDouble(),comadecimal,decimales));
          newItem3->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,3,newItem3);

          QTableWidgetItem *newItem4 = new QTableWidgetItem("");
          newItem4->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
          ui->tableWidget->setItem(numorden,4,newItem4);

          num_vencis << q.value(2).toString();

          numorden++;
         }
        }

}


void desglose_iva_caja::forzar_liquidacion()
{
    if (!ui->tableWidget->currentIndex().isValid()) return;
    int fila=ui->tableWidget->currentRow();
    if (ui->tableWidget->item(fila,2)->text().isEmpty())
      {
        if (!ui->tableWidget->item(fila,1)->text().isEmpty()) return;
            // ya se ha liquidado con vencimiento, no procede forzar
        pidefecha *p = new pidefecha;
        p->esconde_concepto_documento();
        p->asignaetiqueta(tr("FORZAR A FECHA"));
        p->exec();
        QDate fecha=p->fecha();
        delete(p);
        //
        basedatos::instancia()->actu_venci_forzar_liq_iva(num_vencis.at(fila), fecha);
      }
      else
           {
             basedatos::instancia()->actu_venci_quitar_forzar_liq_iva(num_vencis.at(fila));
           }
      cambios=true;
     refrescar();
}

bool desglose_iva_caja::hay_cambios()
{
  return cambios;
}
