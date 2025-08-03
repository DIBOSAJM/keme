#include "contenido_mail_factur.h"
#include "ui_contenido_mail_factur.h"
#include "basedatos.h"

Contenido_mail_factur::Contenido_mail_factur(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Contenido_mail_factur)
{
    ui->setupUi(this);
    QString email_from, email_to, email_subject, email_content;
    basedatos::instancia()->campos_email_cli(&email_from, &email_to, &email_subject, &email_content);
    ui->from_lineEdit->setText(email_from);
    ui->to_lineEdit->setText(email_to);
    ui->subject_lineEdit->setText(email_subject);
    ui->content_plainTextEdit->setPlainText(email_content);

}

Contenido_mail_factur::~Contenido_mail_factur()
{
    delete ui;
}

void Contenido_mail_factur::on_accept_pushButton_clicked()
{
    basedatos::instancia()->actu_campos_email_cli(ui->from_lineEdit->text(),
                                                  ui->to_lineEdit->text(),
                                                  ui->subject_lineEdit->text(),
                                                  ui->content_plainTextEdit->toPlainText());
    accept();
}

