#include "datos_accesorios_export.h"
#include "qclipboard.h"
#include "ui_datos_accesorios_export.h"
#include "basedatos.h"
#include <QMessageBox>

Datos_accesorios_export::Datos_accesorios_export(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Datos_accesorios_export)
{
    ui->setupUi(this);


}

Datos_accesorios_export::~Datos_accesorios_export()
{
    delete ui;
}

void Datos_accesorios_export::carga_datossubcuenta()
{
    QSqlQuery query=basedatos::instancia()->select_datossubcuenta();
    // 2. Obtener el registro (metadatos)
    QSqlRecord record = query.record();
    int columnCount = record.count();

    // 3. Configurar el QTableWidget
    ui->tableWidget->setColumnCount(columnCount);

    // 4. Rellenar las cabeceras
    QStringList headers;
    for (int i = 0; i < columnCount; ++i) {
        headers << record.fieldName(i); // nombre de la columna
    }
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // 5. Rellenar los datos
    int row = 0;
    while (query.next()) {
        ui->tableWidget->insertRow(row);
        for (int col = 0; col < columnCount; ++col) {
            ui->tableWidget->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        }
        row++;
    }
}

void Datos_accesorios_export::carga_externos()
{
    QSqlQuery query=basedatos::instancia()->select_datos_externo();
    // 2. Obtener el registro (metadatos)
    QSqlRecord record = query.record();
    int columnCount = record.count();

    // 3. Configurar el QTableWidget
    ui->tableWidget->setColumnCount(columnCount);

    // 4. Rellenar las cabeceras
    QStringList headers;
    for (int i = 0; i < columnCount; ++i) {
        headers << record.fieldName(i); // nombre de la columna
    }
    ui->tableWidget->setHorizontalHeaderLabels(headers);

    // 5. Rellenar los datos
    int row = 0;
    while (query.next()) {
        ui->tableWidget->insertRow(row);
        for (int col = 0; col < columnCount; ++col) {
            ui->tableWidget->setItem(row, col, new QTableWidgetItem(query.value(col).toString()));
        }
        row++;
    }

}

void Datos_accesorios_export::on_copiar_pushButton_clicked()
{
    QString data;
    int rows = ui->tableWidget->rowCount();
    int cols = ui->tableWidget->columnCount();

    // 1. Obtener los nombres de las columnas (Cabeceras)
    for (int i = 0; i < cols; ++i) {
        data += ui->tableWidget->horizontalHeaderItem(i)->text();
        if (i < cols - 1) data += "\t"; // Separador de pestaña
    }
    data += "\n"; // Salto de línea tras la cabecera

    // 2. Obtener los datos de las celdas
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            QTableWidgetItem *item = ui->tableWidget->item(i, j);
            if (item) {
                data += item->text();
            }
            if (j < cols - 1) data += "\t";
        }
        data += "\n";
    }

    // 3. Enviar al portapapeles del sistema
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(data);

    QMessageBox::information(this,tr("EXPORTACIÓN DATOS"),tr("El contenido se ha pasado al portapapeles"));
}

