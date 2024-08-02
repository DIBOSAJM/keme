#ifndef TIPOS_CAMBIO_H
#define TIPOS_CAMBIO_H

#include <QDialog>

namespace Ui {
class tipos_cambio;
}

class tipos_cambio : public QDialog
{
    Q_OBJECT

public:
    explicit tipos_cambio(QWidget *parent = 0);
    ~tipos_cambio();

private slots:

    void on_nuevo_pushButton_clicked();

    void on_borrar_pushButton_clicked();

    void on_tableWidget_cellChanged(int row, int column);

    void on_cancelar_pushButton_clicked();

    void on_actualizar_pushButton_clicked();

    void on_mueve_arriba_pushButton_clicked();

    void on_mueve_abajo_pushButton_clicked();

    void on_aceptar_pushButton_clicked();

private:
    Ui::tipos_cambio *ui;
    bool cambios=false;
    bool comadecimal;
    QStringList cod_moneda;
    QStringList descrip_moneda;

};

#endif // TIPOS_CAMBIO_H
