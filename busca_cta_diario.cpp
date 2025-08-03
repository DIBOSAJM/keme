#include "busca_cta_diario.h"
#include "ui_busca_cta_diario.h"
#include "basedatos.h"
#include "funciones.h"

Busca_cta_diario::Busca_cta_diario(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Busca_cta_diario)
{
    ui->setupUi(this);
    ui->fecha_dateEdit->setDate(QDate().currentDate());
    ui->diario_tableWidget->setColumnWidth(3,300);
}

Busca_cta_diario::~Busca_cta_diario()
{
    delete ui;
}

QString Busca_cta_diario::cuenta_seleccionada()
{
    return cuenta_sel;
}

void Busca_cta_diario::on_fecha_checkBox_stateChanged(int arg1)
{
    Q_UNUSED(arg1);
    if (ui->fecha_checkBox->isChecked()) ui->fecha_dateEdit->setEnabled(true);
      else ui->fecha_dateEdit->setEnabled(false);
}


void Busca_cta_diario::on_refrescar_pushButton_clicked()
{
    cuenta_sel.clear();
    ui->diario_tableWidget->clearContents();
    ui->diario_tableWidget->setRowCount(0);
 // basedatos::selec_diario_extract(QString concepto, QString documento, bool hay_fecha, QDate fecha, QString debe, QString haber)
    QSqlQuery q=basedatos::instancia()->selec_diario_extract(ui->concepto_lineEdit->text(), ui->doc_lineEdit->text(),ui->fecha_checkBox->isChecked(),
                                                            ui->fecha_dateEdit->date(), ui->debe_lineEdit->text(), ui->haber_lineEdit->text());
 int pos=0;
 if (q.isActive())
        while (q.next()) {
         ui->diario_tableWidget->insertRow(pos);
         //QTableWidgetItem *newItemx = new QTableWidgetItem(query.value(2).toString());
         //newItemx->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         //ui.mayortable->setItem(fila,2,newItemx);  // asiento

        // select cuenta, fecha, asiento, concepto, debe, haber, documento from diario where
         ui->diario_tableWidget->setItem(pos,0,new QTableWidgetItem(q.value(0).toString()));
         ui->diario_tableWidget->setItem(pos,1,new QTableWidgetItem(q.value(1).toDate().toString("dd-MM-yyyy")));
         ui->diario_tableWidget->setItem(pos,2,new QTableWidgetItem(q.value(2).toString()));
         ui->diario_tableWidget->setItem(pos,3,new QTableWidgetItem(q.value(3).toString()));
         QTableWidgetItem *newItemx = new QTableWidgetItem(formateanumero(q.value(4).toDouble(), haycomadecimal(), haydecimales()));
         newItemx->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui->diario_tableWidget->setItem(pos, 4, newItemx);
         QTableWidgetItem *newItemx2 = new QTableWidgetItem(formateanumero(q.value(5).toDouble(), haycomadecimal(), haydecimales()));
         newItemx2->setTextAlignment (Qt::AlignRight | Qt::AlignVCenter);
         ui->diario_tableWidget->setItem(pos, 5, newItemx2);
         ui->diario_tableWidget->setItem(pos,6,new QTableWidgetItem(q.value(6).toString()));

        }
 if (ui->diario_tableWidget->rowCount()>0) cuenta_sel=ui->diario_tableWidget->item(0,0)->text();
}


void Busca_cta_diario::on_diario_tableWidget_cellClicked(int row, int column)
{
    if (ui->diario_tableWidget->item(row,0)!=NULL) cuenta_sel=ui->diario_tableWidget->item(row,0)->text();
    accept();
}

