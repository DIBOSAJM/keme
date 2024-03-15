#ifndef CTAS_IVA_INVERSION_H
#define CTAS_IVA_INVERSION_H

#include <QDialog>
#include <QLineEdit>

namespace Ui {
class ctas_iva_inversion;
}

class ctas_iva_inversion : public QDialog
{
    Q_OBJECT

public:
    explicit ctas_iva_inversion(QWidget *parent = 0);
    ~ctas_iva_inversion();

private:
    Ui::ctas_iva_inversion *ui;
    void buscacta(QLineEdit * lineedit);
private slots:
    void boton_sop_aib();
    void boton_rep_aib();
    void boton_sop_ais();
    void boton_rep_ais();
    void boton_sop_aeb();
    void boton_rep_aeb();
    void boton_sop_oisp();
    void boton_rep_oisp();
    void terminar();

    void AIBctasoplineEdit_finedic();
    void AIBcuentareperlineEdit_finedic();
    void AISctasoplineEdit_finedic();
    void AIScuentareperlineEdit_finedic();
    void AS_ext_ctasoplineEdit_finedic();
    void AS_ext_cuentareperlineEdit_finedic();
    void oi_ctasoplineEdit_finedic();
    void oicuentareperlineEdit_finedic();

};

#endif // CTAS_IVA_INVERSION_H
