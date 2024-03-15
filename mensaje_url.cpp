#include "mensaje_url.h"
#include "ui_mensaje_url.h"

mensaje_url::mensaje_url(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mensaje_url)
{
    ui->setupUi(this);
}

mensaje_url::~mensaje_url()
{
    delete ui;
}

void mensaje_url::get_line(QString line)
{
    ui->label->setText(line);
}

void mensaje_url::get_url_line(QString line)
{
   ui->label_2->setText(line);
   ui->label_2->setOpenExternalLinks(true);
}
