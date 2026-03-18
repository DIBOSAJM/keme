#ifndef DATOS_ACCESORIOS_EXPORT_H
#define DATOS_ACCESORIOS_EXPORT_H

#include <QDialog>

namespace Ui {
class Datos_accesorios_export;
}

class Datos_accesorios_export : public QDialog
{
    Q_OBJECT

public:
    explicit Datos_accesorios_export(QWidget *parent = nullptr);
    ~Datos_accesorios_export();
    void carga_datossubcuenta();
    void carga_externos();

private slots:
    void on_copiar_pushButton_clicked();

private:
    Ui::Datos_accesorios_export *ui;
};

#endif // DATOS_ACCESORIOS_EXPORT_H
