#include "formas_pago.h"
#include "ui_formas_pago.h"
#include <QCheckBox>
#include "pidenombre.h"
#include "basedatos.h"
#include <QMessageBox>

formas_pago::formas_pago(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::formas_pago)
{
    ui->setupUi(this);

    ui->tableWidget->setColumnWidth(0,200);
    ui->tableWidget->setEditTriggers( QAbstractItemView::NoEditTriggers );

    // cargamos tabla
    QSqlQuery q=basedatos::instancia()->carga_formas_pago();
    if (q.isActive())
        while (q.next())
           {
             inserta_linea(q.value(0).toString(),q.value(1).toBool(),q.value(2).toBool());
           }
    connect(ui->nueva_pushButton,SIGNAL(clicked(bool)),SLOT(nuevo_elemento()));
    connect(ui->borrar_pushButton,SIGNAL(clicked(bool)),SLOT(borrar()));
    connect(ui->aceptar_pushButton,SIGNAL(clicked(bool)),SLOT(aceptar()));
}

formas_pago::~formas_pago()
{
    delete ui;
}


void formas_pago::inserta_linea(QString qfpago, bool cuenta_propia, bool cuenta_cliente)
{
   // comprobamos si existe el código primero
   for (int veces=0; veces< ui->tableWidget->rowCount(); veces++)
      {
       if (ui->tableWidget->item(veces,0)!=NULL)
        if (ui->tableWidget->item(veces,0)->text()==qfpago)
          {
           QMessageBox::warning( this, tr("Formas de pago"),
                tr("ERROR: Forma de pago repetida" ));
           return;
          }
      }
   int fila=ui->tableWidget->rowCount();
   ui->tableWidget->insertRow(fila);

   ui->tableWidget->setItem(fila,0,new QTableWidgetItem(qfpago));

   QCheckBox *checkitem = new QCheckBox();
   if (cuenta_propia) checkitem->setChecked(true);
   ui->tableWidget->setCellWidget (fila, 1, checkitem );

   QCheckBox *checkitem2 = new QCheckBox();
   if (cuenta_cliente) checkitem2->setChecked(true);
   ui->tableWidget->setCellWidget (fila, 2, checkitem2 );

  // connect((QCheckBox*)ui.variablestable->cellWidget(veces,4),
  //   SIGNAL(stateChanged ( int )),this,SLOT(activacambiado()));

  // QCheckBox *checkitem;
  // checkitem=(QCheckBox*)ui.variablestable->cellWidget(veces,4);
  // bool ini=checkitem->isChecked();


}

void formas_pago::nuevo_elemento()
{
    QString nombre;
    pidenombre *p = new pidenombre();
    p->asignaetiqueta(tr("Forma de pago"));
    p->asignanombreventana(tr("Nueva forma de pago"));
    int resultado=p->exec();
    nombre=p->contenido();
    delete(p);
    if (resultado==QDialog::Accepted)
      {
         inserta_linea(nombre,false,false);
      }
}

void formas_pago::borrar()
{

    int fila=ui->tableWidget->currentRow();
    if (fila<0 || fila>ui->tableWidget->rowCount())
       {
           QMessageBox::warning( this, tr("Formas de pago"),
                tr("ERROR: Hay que seleccionar forma de pago para borrado" ));
           return;
       }
    if (ui->tableWidget->item(fila,0)==NULL) return;
    // FALTA COMPROBAR NO EXISTENCIA EN CUENTAS AUXILIARES
    if (QMessageBox::question(
           this,
           tr("Formas de pago"),
           tr("¿ Desea borrar el elemento '%1' ?").arg(ui->tableWidget->item(fila,0)->text())) == QMessageBox::No )
                         return;
    ui->tableWidget->removeRow(fila);

}

void formas_pago::aceptar()
{
    // borramos tabla formas_pago
    basedatos::instancia()->borra_tabla_formas_pago();

    // añadimos registros nuevos
    for (int fila=0; fila<ui->tableWidget->rowCount(); fila++)
        {
          if (ui->tableWidget->item(fila,0)==NULL) continue;
          QString forma=ui->tableWidget->item(fila,0)->text();
          QCheckBox *checkitem;
          checkitem=(QCheckBox*)ui->tableWidget->cellWidget(fila,1);
          bool cuenta_propia=checkitem->isChecked();

          QCheckBox *checkitem2;
          checkitem2=(QCheckBox*)ui->tableWidget->cellWidget(fila,2);
          bool cuenta_cliente=checkitem2->isChecked();

          basedatos::instancia()->inserta_formapago(forma, cuenta_propia, cuenta_cliente);
        }
    accept();
}
