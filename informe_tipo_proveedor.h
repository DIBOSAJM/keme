#ifndef INFORME_TIPO_PROVEEDOR_H
#define INFORME_TIPO_PROVEEDOR_H

#include <QDialog>

namespace Ui {
class informe_tipo_proveedor;
}

class informe_tipo_proveedor : public QDialog
{
    Q_OBJECT

public:
    explicit informe_tipo_proveedor(QWidget *parent = 0);
    ~informe_tipo_proveedor();

private:
    Ui::informe_tipo_proveedor *ui;
    void cargacombos_tipoProveedor();
    void genera(int tipo);
    QString cad_filtro_lista_cuentas();
    QString cad_filtro_checks();
    void inf_ctas_sin_mov(int tipo);

private slots :
    void solo_activo_check();
    void solo_no_activo_check();
    void boton_informe_horizontal();
    void boton_informe_vertical();
    void copiar();
    void check_sin_mov();

};

#endif // INFORME_TIPO_PROVEEDOR_H
