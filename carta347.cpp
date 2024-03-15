#include "carta347.h"
#include "ui_carta347.h"
#include "basedatos.h"

carta347::carta347(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::carta347)
{
    ui->setupUi(this);

    // cargamos datos en plantilla
    QSqlQuery q=basedatos::instancia()->datos_carta347();
    // cabecera347, cuerpo347, desglose347, cuerpo2_347, pie347, saludo347, firma347
    if (q.isActive())
        if (q.next())
          {
            ui->cabeceralineEdit->setText(q.value(0).toString());
            ui->cuerpoplainTextEdit->setPlainText(q.value(1).toString());
            ui->desglosecheckBox->setChecked(q.value(2).toBool());
            ui->cuerpoplainTextEdit_2->setPlainText(q.value(3).toString());
            ui->pielineEdit->setText(q.value(4).toString());
            ui->saludolineEdit->setText(q.value(5).toString());
            ui->firmalineEdit->setText(q.value(6).toString());
          }
    connect(ui->aceptarpushButton, SIGNAL(clicked()),SLOT(aceptar()));
}

carta347::~carta347()
{
    delete ui;
}


void carta347::aceptar()
{
    basedatos::instancia()->actu_config_carta347(ui->cabeceralineEdit->text(),
                                                 ui->cuerpoplainTextEdit->toPlainText(),
                                                 ui->desglosecheckBox->isChecked(),
                                                 ui->cuerpoplainTextEdit_2->toPlainText(),
                                                 ui->pielineEdit->text(),
                                                 ui->saludolineEdit->text(),
                                                 ui->firmalineEdit->text());
    accept();
}
