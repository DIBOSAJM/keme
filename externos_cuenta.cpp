#include "externos_cuenta.h"
#include "ui_externos_cuenta.h"
#include <QMessageBox>
#include "basedatos.h"
#include "funciones.h"

externos_cuenta::externos_cuenta(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::externos_cuenta)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnWidth(1,200);
}

externos_cuenta::~externos_cuenta()
{
    delete ui;
}

void externos_cuenta::pasa_parametros(QString cuenta, QDate inicial, QDate final) {
    // dos pasos: saldos iniciales y movimientos entre fechas
    QStringList codigos;
    QList<double> saldos_ini, sumas_debe, sumas_haber;
    QSqlQuery q = basedatos::instancia()->select_sumDH_externos_cuenta_hasta_fecha (cuenta, final);
    if (q.isActive())
        while (q.next())
          {
            codigos << q.value(1).toString(); // externo
            sumas_debe << q.value(2).toDouble();
            sumas_haber << q.value(3).toDouble();
            saldos_ini << 0;
          }
    QDate inicial_anterior=inicial.addDays(-1);
    q=basedatos::instancia()->select_sumDH_externos_cuenta_hasta_fecha (cuenta, inicial_anterior);
    if (q.isActive())
        while (q.next()) {
            for (int i=0; i<codigos.count(); i++) {
                if (codigos.at(i)==q.value(1).toString()) {
                   saldos_ini.replace(i,q.value(2).toDouble()-q.value(3).toDouble());
                   sumas_debe.replace(i,sumas_debe.at(i)-q.value(2).toDouble());
                   sumas_haber.replace(i,sumas_haber.at(i)-q.value(3).toDouble());
                }
            }
        }

    bool coma=haycomadecimal();
    bool decimales=haydecimales();

    for (int i=0;i<codigos.count(); i++) {
        if (casi_cero(saldos_ini.at(i)) && casi_cero(sumas_debe.at(i)) && casi_cero(sumas_haber.at(i))) continue;
        int row=ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row,0, new QTableWidgetItem(codigos.at(i)));
        ui->tableWidget->setItem(row,1,new QTableWidgetItem(basedatos::instancia()->razon_externo(codigos.at(i))));
        ui->tableWidget->setItem(row,2, new QTableWidgetItem(formateanumero(saldos_ini.at(i),coma,decimales)));
        ui->tableWidget->item(row,2)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tableWidget->setItem(row,3, new QTableWidgetItem(formateanumero(sumas_debe.at(i),coma,decimales)));
        ui->tableWidget->item(row,3)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tableWidget->setItem(row,4, new QTableWidgetItem(formateanumero(sumas_haber.at(i),coma,decimales)));
        ui->tableWidget->item(row,4)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        ui->tableWidget->setItem(row,5, new QTableWidgetItem(formateanumero(saldos_ini.at(i)+sumas_debe.at(i)-sumas_haber.at(i),coma,decimales)));
        ui->tableWidget->item(row,5)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }
}

void externos_cuenta::on_pushButton_2_clicked()
{
    QClipboard *cb = QApplication::clipboard();
    QString global;
    global="EXTERNO\tNOMBRE\tSALDO INICIAL\tSUMA DEBE\tSUMAHABER\tSALDO\n";
    for (int i=0; i<ui->tableWidget->rowCount(); i++) {
        for (int c=0; c<ui->tableWidget->columnCount(); c++) {
            global+=ui->tableWidget->item(i,c)->text();
            if (c<(ui->tableWidget->columnCount()-1)) global+="\t";
              else global+="\n";
        }
    }
    cb->setText(global);
    QMessageBox::information( this, tr("DESGLOSE CUENTA EN EXTERNOS"),
                  tr("Se ha pasado el contenido al portapales"));

}
