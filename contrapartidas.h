#ifndef CONTRAPARTIDAS_H
#define CONTRAPARTIDAS_H

#include <QDialog>

namespace Ui {
class contrapartidas;
}

class contrapartidas : public QDialog
{
    Q_OBJECT

public:
    explicit contrapartidas(QWidget *parent = 0);
    ~contrapartidas();
    void valores(QString *aux1, QString *aux2, QString *concepto, QString *valorcontra,
                 bool *coeficiente, bool *verificar, bool *concepto_cuaderno);
    void pasa_valores(QString aux1, QString aux2, QString concepto, QString valorcontra,
                      bool coeficiente, bool verificar, bool concepto_cuaderno);
    void activa_una_linea();
    bool guardar_valores();


private:
    Ui::contrapartidas *ui;
    bool haycoma;

private slots :
         void ctacambiada();
         void ctafinedicion();
         void buscacta();

         void ctacambiada2();
         void ctafinedicion2();
         void buscacta2();
         void chequea_boton_aceptar();

         void campo_valor_fin_edicion();

         void check_concepto();

         void boton_aceptar();
         void on_busca_diario_pushButton_clicked();
         void on_buscapush_diario2_pushButton_clicked();
};

#endif // CONTRAPARTIDAS_H
