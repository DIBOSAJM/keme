#ifndef ITEM_INMOV_H
#define ITEM_INMOV_H

#include <QDialog>
#include <QDate>

namespace Ui {
    class item_inmov;
}

class item_inmov : public QDialog
{
    Q_OBJECT

public:
    explicit item_inmov(QWidget *parent = 0);
    ~item_inmov();
    void pasadatos(QString num,
                               QString cta_aa,
                               QString cta_ag,
                               QString concepto,
                               QDate fecha_inicio,
                               QDate fecha_compra,
                               QDate fecha_ult_amort,
                               QString coef,
                               QString valor_adq,
                               QString valor_amortizado,
                               QString factura,
                               QString proveedor);


private:
    Ui::item_inmov *ui;
    bool comadecimal, decimales;
private slots:
    void botonaapulsado();
    void botongcuentapulsado();
    void finedicctaaa();
    void finedictag();
    void finediccoef();
    void finedicvadq();
    void finedicvamort();
    void guardar_y_salir();

};

#endif // ITEM_INMOV_H
