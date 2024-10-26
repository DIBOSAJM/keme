#include "traspasoborrador.h"
#include "ui_traspasoborrador.h"
#include "basedatos.h"

TraspasoBorrador::TraspasoBorrador(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TraspasoBorrador)
{
    ui->setupUi(this);
    ui->renum_checkBox->setChecked(basedatos::instancia()->config_renum_borr());
}

TraspasoBorrador::~TraspasoBorrador()
{
    delete ui;
}

bool TraspasoBorrador::renum()
{
    return ui->renum_checkBox->isChecked();
}
