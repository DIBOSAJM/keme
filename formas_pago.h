#ifndef FORMAS_PAGO_H
#define FORMAS_PAGO_H

#include <QDialog>

namespace Ui {
class formas_pago;
}

class formas_pago : public QDialog
{
    Q_OBJECT

public:
    explicit formas_pago(QWidget *parent = 0);
    ~formas_pago();

private:
    Ui::formas_pago *ui;
    void inserta_linea(QString qfpago, bool cuenta_propia, bool cuenta_cliente);

private slots:
    void nuevo_elemento();
    void borrar();
    void aceptar();
};

#endif // FORMAS_PAGO_H
