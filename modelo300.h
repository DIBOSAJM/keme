#ifndef MODELO300_H
#define MODELO300_H

#include <QDialog>

namespace Ui {
class modelo300;
}

class modelo300 : public QDialog
{
    Q_OBJECT

public:
    explicit modelo300(QWidget *parent = 0);
    ~modelo300();
    void parametros(QString *nif, QString *razon, QString *nombre, bool *escomplementaria, bool *essustitutiva,
               QString *declaracion_anterior, bool *redeme, QString *cta_banco, QString *tipo_operacion, double *compensar);

private:
    Ui::modelo300 *ui;

private slots:
    void buscactabase();
};

#endif // modelo300_H
