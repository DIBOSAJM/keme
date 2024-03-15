#include "direc_sii.h"
#include "ui_direc_sii.h"
#include "basedatos.h"

direc_sii::direc_sii(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::direc_sii)
{
    ui->setupUi(this);

    connect (ui->aceptar_pushButton, SIGNAL(clicked(bool)),SLOT(aceptar()));
    QSqlQuery q=basedatos::instancia()->select_direc_SII();
    if (q.isActive())
        if (q.next())
          {
           ui->emitidas_test_lineEdit->setText(q.value(0).toString());
           ui->recibidas_test_lineEdit->setText(q.value(1).toString());
           ui->emitidas_real_lineEdit->setText(q.value(2).toString());
           ui->recibidas_real_lineEdit->setText(q.value(3).toString());
           ui->sii_lr_lineEdit->setText(q.value(4).toString());
           ui->sii_lineEdit->setText(q.value(5).toString());
          }
}

direc_sii::~direc_sii()
{
    delete ui;
}

void direc_sii::aceptar()
{
    basedatos::instancia()->actualiza_direc_SII(ui->emitidas_test_lineEdit->text(),
                                                ui->recibidas_test_lineEdit->text(),
                                                ui->emitidas_real_lineEdit->text(),
                                                ui->recibidas_real_lineEdit->text(),
                                                ui->sii_lr_lineEdit->text(),
                                                ui->sii_lineEdit->text());
    accept();
}
