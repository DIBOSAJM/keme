#include "busca_externo.h"
#include "ui_busca_externo.h"
#include "basedatos.h"

busca_externo::busca_externo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::busca_externo)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnWidth(0,90);
    ui->tableWidget->setColumnWidth(1,250);
    ui->tableWidget->setColumnWidth(2,200);
    ui->tableWidget->setColumnWidth(3,250);
    ui->tableWidget->setColumnWidth(4,150);
    ui->tableWidget->setColumnWidth(5,150);
    ui->tableWidget->setColumnWidth(6,80);
    ui->tableWidget->setColumnWidth(7,150);
    ui->tableWidget->setColumnWidth(8,200);
    carga_datos();
    connect(ui->tableWidget,SIGNAL(cellClicked(int,int)),SLOT(fila_tabla_clicked()));
    connect(ui->refrescar_pushButton,SIGNAL(clicked(bool)),SLOT(refrescar()));
    connect(ui->lineEdit, SIGNAL(textChanged(QString)),SLOT(refrescar()));
}

busca_externo::~busca_externo()
{
    delete ui;
}


void busca_externo::carga_datos()
{
    // por campo y criterio (0 comience, 1 contenga)
    int criterio=ui->criterio_comboBox->currentIndex();
    QString campo;
    switch (ui->campo_comboBox->currentIndex()) {
    case 0: campo="razon";
        break;
    case 1: campo="nombrecomercial";
      break;
    case 2: campo="codigo";
      break;
    case 3: campo="domicilio";
      break;
    case 4: campo="poblacion";
      break;
    case 5: campo="provincia";
      break;
    case 6: campo="codigopostal";
      break;
    case 7: campo="pais";
      break;
    case 8: campo="email";
      break;
    }

    QSqlQuery q = basedatos::instancia()->consulta_busca_externos(campo,criterio,ui->lineEdit->text());
    if (q.isActive())
     {
      ui->tableWidget->clearContents();
      ui->tableWidget->setRowCount(0);
      while (q.next())
       {
        int fila=ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(fila);
        for (int veces=0;veces<9;veces++)
           ui->tableWidget->setItem(fila,veces,new QTableWidgetItem(q.value(veces).toString()));
       }
     }
   if (ui->tableWidget->rowCount()>0)
      codigo=ui->tableWidget->item(0,0)->text();

}


QString busca_externo::codigo_elec()
{
    return codigo;
}

QString busca_externo::nombre_elec()
{
    return nombre;
}



void busca_externo::fila_tabla_clicked()
{
    int fila=ui->tableWidget->currentRow();
    if (fila>=0 && fila<ui->tableWidget->rowCount())
      {
        codigo=ui->tableWidget->item(fila,0)->text();
        nombre==ui->tableWidget->item(fila,0)->text();
      }
    accept();
}

void busca_externo::refrescar()
{
    carga_datos();
}
