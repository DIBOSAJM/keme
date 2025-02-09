#ifndef ACTIVIDAD_EDICION_H
#define ACTIVIDAD_EDICION_H

#include <QDialog>

namespace Ui {
class Actividad_edicion;
}

class Actividad_edicion : public QDialog
{
    Q_OBJECT

public:
    explicit Actividad_edicion(QWidget *parent = nullptr);
    ~Actividad_edicion();
    void pasa_ref_descrip(QString qreferencia, QString qdescripcion);
    void pasa_params(QString codigo, QString tipo, QString epigrafe, QString cnae);
    void devuelve_params(QString *codigo, QString *tipo, QString *epigrafe, QString *cnae);

private slots:
    void on_codigo_comboBox_currentIndexChanged(int index);

    void on_tipo_comboBox_currentIndexChanged(int index);

private:
    Ui::Actividad_edicion *ui;
    QString referencia, descripcion;
    QStringList tipos_actividades_A;
    QStringList tipos_actividades_B;
    QStringList tipo_actividad_1;
    QStringList tipo_actividad_2;
    QStringList tipo_actividad_3;
    QStringList tipo_actividad_4;
    QStringList tipo_actividad_5;
    QStringList lista_cnae_2009;
};

#endif // ACTIVIDAD_EDICION_H
