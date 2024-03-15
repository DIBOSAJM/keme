#include "vies_msj.h"
#include "ui_vies_msj.h"

vies_msj::vies_msj(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::vies_msj)
{
    ui->setupUi(this);
}

vies_msj::~vies_msj()
{
    delete ui;
}

void vies_msj::haz_num_incorrecto()
{
  ui->ok_img_label->hide();
  ui->mensaje_label->setText(tr("NIF-IVA INCORRECTO"));
}

void vies_msj::haz_num_correcto(QString razon, QString direccion)
{
  ui->incorrecto_label->hide();
  ui->textEdit->setText(razon+"\n"+direccion);
}

