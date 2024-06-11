#include "tipos_cambio.h"
#include "ui_tipos_cambio.h"
#include "pidenombre.h"
#include <qmessagebox.h>
#include "funciones.h"

tipos_cambio::tipos_cambio(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tipos_cambio)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnWidth(0,350);
    comadecimal=haycomadecimal();
}

tipos_cambio::~tipos_cambio()
{
    delete ui;
}


void tipos_cambio::on_nuevo_pushButton_clicked()
{
    cambios=true;
    pidenombre *p = new pidenombre;
    p->asignaetiqueta(tr("Introduzca código de divisa"));
    p->exec();
    QString codigo=p->contenido();
    bool posicion_encontrada=false;
    int lugar=0;
    for (int i=0; i< ui->tableWidget->rowCount(); i++) {
        if (ui->tableWidget->verticalHeaderItem(i)->text()==codigo) return;
        if (ui->tableWidget->verticalHeaderItem(i)->text()>codigo) {
            posicion_encontrada=true;
            lugar=i;
            break;
        }
    }
    if (!posicion_encontrada) lugar=ui->tableWidget->rowCount();
    //if (lugar<(ui->tableWidget->rowCount()-1)) lugar++;
    ui->tableWidget->insertRow(lugar);
    ui->tableWidget->setVerticalHeaderItem(lugar,new QTableWidgetItem(codigo));

}

void tipos_cambio::on_borrar_pushButton_clicked()
{
    cambios=true;
    if (ui->tableWidget->currentIndex().isValid()) {
        int i=ui->tableWidget->currentRow();
        if (QMessageBox::question(
               this,
               tr("TIPOS DE CAMBIO"),
                tr("¿ Desea borrar '%1' ?").arg(ui->tableWidget->verticalHeaderItem(i)->text())) == QMessageBox::No )
                             return;
        ui->tableWidget->removeRow(i);
    }
}

void tipos_cambio::on_tableWidget_cellChanged(int row, int column)
{
    cambios=true;
    if (column==1) {
        if (ui->tableWidget->item(row,column)==NULL) return;
        QString contenido=ui->tableWidget->item(row,column)->text();
        if (comadecimal) ui->tableWidget->item(row,column)->setText(convacoma(contenido));
          else ui->tableWidget->item(row,column)->setText(convapunto(contenido));
        if (convapunto(contenido).toDouble()<0.0000001) ui->tableWidget->item(row,column)->setText(QString());

    }
}

void tipos_cambio::on_cancelar_pushButton_clicked()
{
    if (cambios) {
        if (QMessageBox::question(
               this,
               tr("TIPOS DE CAMBIO"),
                tr("HAY CAMBIOS ¿ Desea cancelar y salir ?")) == QMessageBox::No )
                             return;
    }
    reject();

}
