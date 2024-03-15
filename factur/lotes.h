#ifndef LOTES_H
#define LOTES_H

#include <QDialog>
#include <QString>

namespace Ui {
    class lotes;
}

class lotes : public QDialog
{
    Q_OBJECT

public:
    explicit lotes(QString codigo, QString descripcion, QWidget *parent = 0);
    ~lotes();

private:
    Ui::lotes *ui;
    bool comadecimal, decimales;
    QString codigo;
    bool existe_cod_entabla(QString codigo);
    void actualiza_total();
    void carga_datos();

private slots:
    void combo_predef_cambiado();
    void check_cuotas_cambiado();
    void nueva_cuenta();
    void ed_cuenta();
    void borra_linea();
    void carga_auxiliares();
    void fin_edic_presupuesto();
    void genera_docs();
    void aceptar();

};

#endif // LOTES_H
