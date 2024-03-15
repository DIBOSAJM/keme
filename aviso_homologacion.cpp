#include "aviso_homologacion.h"
#include "ui_aviso_homologacion.h"

aviso_homologacion::aviso_homologacion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::aviso_homologacion)
{
    ui->setupUi(this);
}

aviso_homologacion::~aviso_homologacion()
{
    delete ui;
}
