#include "config_verifactu.h"
#include "ui_config_verifactu.h"
#include "basedatos.h"

Config_Verifactu::Config_Verifactu(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Config_Verifactu)
{
    ui->setupUi(this);

    QSqlQuery q = basedatos::instancia()->config_sif_verifactu();
    if (q.isActive())
        if (q.next()) {
            //"select sif_nif,sif_nombre_razon, sif_nombre_sif, sif_id_sistema_informatico,sif_numero_instalacion, "
            //"sif_tipo_uso_verifactu, sif_posible_multi_ot, sif_multi_ot, endpoint_verifactu from configuracion"
            ui->nif_lineEdit->setText(q.value(0).toString());
            ui->razon_lineEdit->setText(q.value(1).toString());
            ui->nombre_sistema_lineEdit->setText(q.value(2).toString());
            ui->id_sistema_lineEdit->setText(q.value(3).toString());
            ui->numero_instalacion_lineEdit->setText(q.value(4).toString());
            ui->solo_verifactu_checkBox->setChecked(q.value(5).toBool());
            ui->posible_multi_obligados_checkBox->setChecked(q.value(6).toBool());
            ui->multiples_obligados_checkBox->setChecked(q.value(7).toBool());
            ui->endpoint_lineEdit->setText(q.value(8).toString());
            ui->url_val_QR_lineEdit->setText(q.value(9).toString());
        }
}

Config_Verifactu::~Config_Verifactu()
{
    delete ui;
}

void Config_Verifactu::on_aceptar_pushButton_clicked()
{
    basedatos::instancia()->actualiza_config_sif_verifactu(ui->nif_lineEdit->text(), ui->razon_lineEdit->text(),
                                                           ui->nombre_sistema_lineEdit->text(),
                                                           ui->id_sistema_lineEdit->text(),
                                                           ui->numero_instalacion_lineEdit->text(), ui->solo_verifactu_checkBox->isChecked(),
                                                           ui->posible_multi_obligados_checkBox->isChecked(), ui->multiples_obligados_checkBox->isChecked(),
                                                           ui->endpoint_lineEdit->text(), ui->url_val_QR_lineEdit->text());

    accept();
}

