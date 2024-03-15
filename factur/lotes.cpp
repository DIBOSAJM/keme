#include "lotes.h"
#include "ui_lotes.h"
#include "basedatos.h"
#include "ed_cta_lote.h"
#include "funciones.h"
#include <QMessageBox>
#include <QProgressDialog>

lotes::lotes(QString qcodigo, QString  descrip, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::lotes)
{
    ui->setupUi(this);
    codigo=qcodigo;

    setWindowTitle(codigo+" - " + descrip);

    comadecimal=haycomadecimal();
    decimales=haydecimales();

    ui->predefinidascomboBox->addItems(basedatos::instancia()->lista_cod_predef());

    combo_predef_cambiado();

    check_cuotas_cambiado();

    connect(ui->predefinidascomboBox,SIGNAL(currentIndexChanged(int)),
            SLOT(combo_predef_cambiado()));
    connect(ui->cuotascheckBox,SIGNAL(stateChanged(int)),SLOT(check_cuotas_cambiado()));

    connect(ui->addpushButton,SIGNAL(clicked()),SLOT(nueva_cuenta()));

    connect(ui->edpushButton,SIGNAL(clicked()),SLOT(ed_cuenta()));

    connect(ui->suppushButton,SIGNAL(clicked()),SLOT(borra_linea()));

    connect(ui->addctaspushButton,SIGNAL(clicked()), SLOT(carga_auxiliares()));

    connect(ui->presupuestolineEdit,SIGNAL(editingFinished()), SLOT(fin_edic_presupuesto()));

    connect(ui->aceptarpushButton,SIGNAL(clicked()),SLOT(aceptar()));

    connect(ui->generarpushButton, SIGNAL(clicked()), SLOT(genera_docs()));

    ui->tableWidget->setColumnWidth(1,200);

    ui->tableWidget->setEditTriggers ( QAbstractItemView::NoEditTriggers );

    carga_datos();

    ui->fecha_dateEdit->setDate(QDate::currentDate());
}

void lotes::carga_datos()
{
    QSqlQuery q = basedatos::instancia()->cabecera_lote_fact(codigo);
    if (q.isActive())
        if (q.next())
          {
            // select cuotas, cod_factura_predef, presupuesto
            if (q.value(0).toBool()) ui->cuotascheckBox->setChecked(true);
               else ui->cuotascheckBox->setChecked(false);
            check_cuotas_cambiado();
            for (int veces=0; veces<ui->predefinidascomboBox->count(); veces++)
                {
                 if (ui->predefinidascomboBox->itemText(veces)==q.value(1).toString())
                    { ui->predefinidascomboBox->setCurrentIndex(veces); break;}
                }
            QString presup=q.value(2).toString();
            if  (comadecimal) presup=convacoma(presup);
            ui->presupuestolineEdit->setText(presup);
          }

    q=basedatos::instancia()->lineas_lin_lote_predef(codigo);
    // select cuenta, importe, cuota from lin_lote_predef
    if (q.isActive())
        while (q.next())
          {
           int lineas=ui->tableWidget->rowCount();
           ui->tableWidget->insertRow(lineas);
           ui->tableWidget->setItem(lineas,0,new QTableWidgetItem(q.value(0).toString()));

           ui->tableWidget->setItem(lineas,1,new QTableWidgetItem(
                   basedatos::instancia()->selectDescripcionplancontable(q.value(0).toString())));

           QTableWidgetItem *newItem = new QTableWidgetItem(q.value(2).toString());
           newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui->tableWidget->setItem(lineas,2,newItem);

           QTableWidgetItem *newItem2 = new QTableWidgetItem(q.value(1).toString());
           newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui->tableWidget->setItem(lineas,3,newItem2);

           ui->tableWidget->setItem(lineas,4,new QTableWidgetItem(q.value(3).toString())); // ojo cargar aquí externo

          }

    actualiza_total();

}


void lotes::combo_predef_cambiado()
{
    QString qdescrip;
    basedatos::instancia()->existecodigo_fact_predef (ui->predefinidascomboBox->currentText(),
                                                      &qdescrip);
    ui->descrip_predeflabel->setText(qdescrip);
}

void lotes::check_cuotas_cambiado()
{
  if (ui->cuotascheckBox->isChecked())
    {
      ui->presupuestolineEdit->show();
      ui->presupuesto_label->show();
      ui->sumacuotaslineEdit->show();
      ui->cuotas_label->show();
      ui->tableWidget->hideColumn(3);
      ui->tableWidget->showColumn(2);
    }
    else
        {
         ui->presupuestolineEdit->hide();
         ui->presupuesto_label->hide();
         ui->sumacuotaslineEdit->hide();
         ui->cuotas_label->hide();
         ui->tableWidget->hideColumn(2);
         ui->tableWidget->showColumn(3);
        }
}

lotes::~lotes()
{
    delete ui;
}

void lotes::nueva_cuenta()
{
  ed_cta_lote *l = new ed_cta_lote(comadecimal);
  l->con_cuota(ui->cuotascheckBox->isChecked());
  if (l->exec()!=QDialog::Accepted) return;
  if (!esauxiliar(l->cuenta()))
    {
     QMessageBox::warning( this, tr("Cuentas auxiliares"),
                        tr("ERROR: El código de cuenta suministrado no existe"));
     return;
    }

  // existe ya la cuenta ??
  for (int veces=0; veces<ui->tableWidget->rowCount();veces++)
    {
     if (ui->tableWidget->item(veces,0)->text()==l->cuenta() &&
         ui->tableWidget->item(veces,4)->text()==l->externo())
       {
         QMessageBox::warning( this, tr("Cuentas auxiliares"),
                            tr("ERROR: El código suministrado ya existe"));
         return;

       }
    }
  int lineas=ui->tableWidget->rowCount();
  ui->tableWidget->insertRow(lineas);
  // QTableWidgetItem *newItem = new QTableWidgetItem("");
  // if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
  ui->tableWidget->setItem(lineas,0,new QTableWidgetItem(l->cuenta()));
  ui->tableWidget->setItem(lineas,1,new QTableWidgetItem(l->descrip()));

  QTableWidgetItem *newItem = new QTableWidgetItem(l->cuota());
  newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
  ui->tableWidget->setItem(lineas,2,newItem);

  QTableWidgetItem *newItem2 = new QTableWidgetItem(l->importe());
  newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
  ui->tableWidget->setItem(lineas,3,newItem2);

  ui->tableWidget->setItem(lineas,4,new QTableWidgetItem(l->externo()));

  delete(l);
  actualiza_total();

}

void lotes::ed_cuenta()
{
    if (!ui->tableWidget->currentIndex().isValid()) return;
    ed_cta_lote *l = new ed_cta_lote(comadecimal);
    l->con_cuota(ui->cuotascheckBox->isChecked());
    int fila=ui->tableWidget->currentRow();
    l->pasa_edicion(ui->tableWidget->item(fila,0)->text(),
                    ui->tableWidget->item(fila,3)->text(),
                    ui->tableWidget->item(fila,2)->text(),
                    ui->tableWidget->item(fila,4)->text());
   if (l->exec()!=QDialog::Accepted) return;
   ui->tableWidget->item(fila,0)->setText(l->cuenta());
   ui->tableWidget->item(fila,1)->setText(l->descrip());
   ui->tableWidget->item(fila,2)->setText(l->cuota());
   ui->tableWidget->item(fila,3)->setText(l->importe());
    delete(l);
    actualiza_total();

}

void lotes::borra_linea()
{
   if (!ui->tableWidget->currentIndex().isValid()) return;

   if (QMessageBox::question(
          this,
          tr("Eliminar registros"),
          tr("¿ Desea borrar la línea seleccionada ?"),
          tr("&Sí"), tr("&No"),
          QString(), 0, 1 ) ==1 )
                        return;

   ui->tableWidget->removeRow(ui->tableWidget->currentRow());
   actualiza_total();

}

void lotes::carga_auxiliares()
{
    if (ui->principallineEdit->text().trimmed().isEmpty()) return;

    QSqlQuery q=basedatos::instancia()->selec_auxiliares_codigo(ui->principallineEdit->text());

    if (q.isActive())
        while (q.next())
          {
           if (existe_cod_entabla(q.value(0).toString())) continue;
           int lineas=ui->tableWidget->rowCount();
           ui->tableWidget->insertRow(lineas);
           // QTableWidgetItem *newItem = new QTableWidgetItem("");
           // if (veces==2 || veces==3) newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui->tableWidget->setItem(lineas,0,new QTableWidgetItem(q.value(0).toString()));
           ui->tableWidget->setItem(lineas,1,new QTableWidgetItem(q.value(1).toString()));

           QTableWidgetItem *newItem = new QTableWidgetItem(comadecimal ?
                                                            convacoma(q.value(2).toString()):
                                                            q.value(2).toString()
                                                            );
           newItem->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui->tableWidget->setItem(lineas,2,newItem);

           QTableWidgetItem *newItem2 = new QTableWidgetItem("");
           newItem2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
           ui->tableWidget->setItem(lineas,3,newItem2);

           ui->tableWidget->setItem(lineas,4,new QTableWidgetItem("")); // externos
          }
    actualiza_total();
}



bool lotes::existe_cod_entabla(QString codigo)
{
    for (int veces=0; veces<ui->tableWidget->rowCount();veces++)
      {
       if (ui->tableWidget->item(veces,0)->text()==codigo)
           return true;

      }
 return false;
}


void lotes::actualiza_total()
{
    double presupuesto=convapunto(ui->presupuestolineEdit->text()).toDouble();
    double suma=0;
    for (int veces=0; veces<ui->tableWidget->rowCount();veces++)
      {
        if (!ui->tableWidget->item(veces,2)->text().isEmpty())
          suma+=presupuesto*convapunto(ui->tableWidget->item(veces,2)->text()).toDouble()/100;
      }
    if (suma>0.001)
      {
       ui->sumacuotaslineEdit->setText(formatea_redondeado(suma,comadecimal,decimales));
      }
}

void lotes::fin_edic_presupuesto()
{
   double importe=convapunto(ui->presupuestolineEdit->text()).toDouble();
   ui->presupuestolineEdit->setText(formateanumero(importe,comadecimal,decimales));
   actualiza_total();
}

void lotes::aceptar()
{
/*
    void actualiza_cabecera_lote_fact(QString codigo, bool cuotas,
                                                 QString cod_factura_predef,
                                                 QString presupuesto);
    void borra_lins_lote_predef(QString codigo);
    void inserta_lin_lote_predef(QString codigo, QString cuenta,
                                 QString importe,
                                 QString cuota);
  */
    basedatos::instancia()->actualiza_cabecera_lote_fact(codigo,
                                                         ui->cuotascheckBox->isChecked(),
                                                         ui->predefinidascomboBox->currentText(),
                                                         convapunto(ui->presupuestolineEdit->text()));
    basedatos::instancia()->borra_lins_lote_predef(codigo);
    for (int veces=0; veces<ui->tableWidget->rowCount(); veces++)
       {
        // cuenta, descripcion, cuota, nuevo importe
        basedatos::instancia()->inserta_lin_lote_predef(codigo,
                                                        ui->tableWidget->item(veces,0)->text(),
                                                        ui->tableWidget->item(veces,3)->text(),
                                                        ui->tableWidget->item(veces,2)->text(),
                                                        ui->tableWidget->item(veces,4)->text());
       }
  accept();
}


void lotes::genera_docs()
{
  // cuál es el documento modelo
    // facturas predefinidas
    /* table facturas_predef (";
    "codigo  varchar(40),";
    "descrip  varchar(254),";
    "serie   varchar(80),";
    "contabilizable bool default 0";
    "con_ret bool default 0,";
    "con_re bool default 0,";
    "tipo_ret  numeric(5,2),"
    "tipo_doc varchar(80), "
    "notas text, "
    "pie1 varchar(254),"
    "pie2 varchar(254), ";

    "select descrip, serie, contabilizable, con_ret, con_re, tipo_ret, tipo_doc, notas, "
     "pie1, pie2 from facturas_predef where codigo='";


   "create table lin_fact_predef ( ";
   "linea integer primary key autoincrement,";
   "cod_fact     varchar(40), "  // por este campo se relaciona con cabecera
   "codigo       varchar(80),"
   "descripcion  varchar(254),"
   "cantidad     numeric(19,3),"
   "precio       numeric(19,4),"
   "clave_iva    varchar(40),"
   "tipo_iva     numeric(8,4),"
   "tipo_re      numeric(8,4),"
   "dto          numeric(8,4)";

   "select codigo, descripcion, cantidad, precio, clave_iva, tipo_iva, tipo_re, dto "
   "from lin_fact_predef where cod_fact='"
   */

    if (QMessageBox::question(
           this,
           tr("Lotes de documentos"),
           tr("¿ Desea generar documentos para la fecha ") +
              ui->fecha_dateEdit->date().toString("dd-MM-yyyy") + " ?",
           tr("&Sí"), tr("&No"),
           QString(), 0, 1 ) ==1 )
                         return;


    QSqlQuery q = basedatos::instancia()->select_facturas_predef(
            ui->predefinidascomboBox->currentText());
    // QString descrip_doc=q.value(0).toString();
    QString serie;
    bool contabilizable=false;
    bool con_ret=false;
    bool con_re=false;
    double tipo_ret=0;
    QString tipo_doc;
    QString notas;
    QString pie1;
    QString pie2;
    QString concepto_sii;
    if (q.isActive())
        if (q.next())
           {
             serie=q.value(1).toString();
             contabilizable=q.value(2).toBool();
             con_ret=q.value(3).toBool();
             con_re=q.value(4).toBool();
             tipo_ret=q.value(5).toDouble();
             tipo_doc=q.value(6).toString();
             notas=q.value(7).toString();
             pie1=q.value(8).toString();
             pie2=q.value(9).toString();
             concepto_sii=q.value(10).toString();
           }

    q = basedatos::instancia()->select_lin_facturas_predef(ui->predefinidascomboBox->currentText());

    QProgressDialog progreso(tr("Procesando documentos ..."), 0, 0, ui->tableWidget->rowCount()-1);
     progreso.setMinimumDuration(100);



    // vamos generando las facturas una a una
    for (int veces=0;veces<ui->tableWidget->rowCount();veces++)
       {
        progreso.setValue(veces);
        QCoreApplication::processEvents();
        // calculamos el total de la factura
        bool primero=true;
        double total_base=0;
        if (q.isActive())
           {
            if (q.first())
              do
               {
                double importe=q.value(3).toDouble()*q.value(4).toDouble();
                if (primero)
                   {
                    if (ui->cuotascheckBox->isChecked())
                      {
                        importe=convapunto(ui->presupuestolineEdit->text()).toDouble()*
                                convapunto(ui->tableWidget->item(veces,2)->text()).toDouble()/100;
                      }
                      else
                          {
                           if (!ui->tableWidget->item(veces,3)->text().isEmpty())
                              importe=convapunto(ui->tableWidget->item(veces,3)->text()).toDouble();
                          }
                    primero=false;
                   }
                total_base+=importe*(1-q.value(8).toDouble()/100);
               } while (q.next());
           }
        // Comenzamos transacción: tablas series_fact, facturas, detalle_fact (añadir)
        QSqlDatabase contabilidad=QSqlDatabase::database();
        contabilidad.transaction();

        basedatos::instancia()->bloquea_para_facturas();

       // Está el campo facturalineEdit vacío ?
       //
       //   Sí --->  aumentamos campo de número (tabla series_fact) y asignamos a facturalineEdit;
        //           incrementamos valor campo proxnum, para código de serie (codigo) en series_fact

         qlonglong numero=basedatos::instancia()->proxnum_serie(serie);
         QString cadnum; cadnum.setNum(numero);

         // insertamos
         QString cadret; cadret.setNum(total_base*tipo_ret/100,'f',2);
         QString cadtiporet; cadtiporet.setNum(tipo_ret,'f',2);
         int clave_cabecera=basedatos::instancia()->nuevacabecerafactura(
                      serie,
                      cadnum,
                      ui->tableWidget->item(veces,0)->text(),
                      ui->fecha_dateEdit->date(),
                     ui->fecha_dateEdit->date(),
                      ui->fecha_dateEdit->date(),
                      false, // contabilizado
                      contabilizable,
                      con_ret,
                      con_re,
                      cadtiporet,
                      cadret,
                      tipo_doc,
                      notas,
                      pie1,
                      pie2,
                      "0","",ui->tableWidget->item(veces,4)->text(),concepto_sii);
         primero=true;
         if (q.isActive())
            {
             if (q.first())
               do
               {
                 double cantidad=q.value(3).toDouble();
                 double precio=q.value(4).toDouble();
                 if (primero)
                    {
                     if (ui->cuotascheckBox->isChecked())
                       {
                         precio=convapunto(ui->presupuestolineEdit->text()).toDouble()*
                                 convapunto(ui->tableWidget->item(veces,2)->text()).toDouble()/100;
                         cantidad=1;
                       }
                       else
                           {
                            cantidad=1;
                            if (!ui->tableWidget->item(veces,3)->text().isEmpty())
                               precio=convapunto(ui->tableWidget->item(veces,3)->text()).toDouble();
                           }
                     primero=false;
                    }
                 QString cadcantidad; cadcantidad.setNum(cantidad, 'f',2);
                 QString cadprecio; cadprecio.setNum(precio,'f',2);
                 //"select linea, codigo, descripcion, cantidad, precio, clave_iva, tipo_iva, "
                 //     "tipo_re, dto "
                 basedatos::instancia()->insert_lin_doc(clave_cabecera,
                                                q.value(1).toString(),   // codigo
                                                q.value(2).toString(),   // descrip
                                                cadcantidad,   // cantidad
                                                cadprecio,   // precio
                                                q.value(5).toString(), // clave_iva
                                                q.value(6).toString(), // tipo_iva
                                                q.value(7).toString(), // recargo equivalencia
                                                q.value(8).toString(),
                                                q.value(10).toString(),
                                                q.value(9).toString()); // dto.

               } while (q.next());
            }


        basedatos::instancia()->desbloquea_y_commit();

       }
    QMessageBox::information( this, tr("Lotes de facturas"),tr("Se han generado los documentos"));

}
