#include "configuracion_ia.h"
#include "ui_configuracion_ia.h"
#include "basedatos.h"
#include <QMessageBox>

Configuracion_IA::Configuracion_IA(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Configuracion_IA)
{
    ui->setupUi(this);
    QString api_url, api_key, prompt_factura, prompt_fact_emitida, prompt_prov_cli;
    bool ia_no_gestion_iva, ia_no_gestion_isp;
    bool mostrar_respuesta_ia;
    basedatos::instancia()->parametrosIA(&api_url, &api_key, &prompt_factura, &prompt_fact_emitida, &prompt_prov_cli,
                                         &ia_no_gestion_iva, &ia_no_gestion_isp, &mostrar_respuesta_ia);
    ui->api_url_lineEdit->setText(api_url);
    ui->api_key_lineEdit->setText(api_key);
    ui->plainTextEdit->setPlainText(prompt_factura);
    ui->emitidas_plainTextEdit->setPlainText(prompt_fact_emitida);
    ui->datos_cliprov_plainTextEdit->setPlainText(prompt_prov_cli);
    ui->no_iva_checkBox->setChecked(ia_no_gestion_iva);
    ui->no_isp_checkBox->setChecked(ia_no_gestion_isp);
    ui->resultados_checkBox->setChecked(mostrar_respuesta_ia);
}

Configuracion_IA::~Configuracion_IA()
{
    delete ui;
}

void Configuracion_IA::on_pushButton_clicked()
{
    if (basedatos::instancia()->guarda_parametrosIA(ui->api_url_lineEdit->text(),
                                                    ui->api_key_lineEdit->text(),
                                                    ui->plainTextEdit->toPlainText(),
                                                    ui->emitidas_plainTextEdit->toPlainText(),
                                                    ui->datos_cliprov_plainTextEdit->toPlainText(),
                                                    ui->no_iva_checkBox->isChecked(),
                                                    ui->no_isp_checkBox->isChecked(),ui->resultados_checkBox->isChecked()))
        accept();
    else {
        QMessageBox::warning(this,tr("Configuración IA"),tr("ERROR en la grabación de los parámetros"));
        reject();
    }

}

