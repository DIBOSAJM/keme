#ifndef CAMBIA_ACTIVIDAD_H
#define CAMBIA_ACTIVIDAD_H

#include <QDialog>

namespace Ui {
class Cambia_actividad;
}

class Cambia_actividad : public QDialog
{
    Q_OBJECT

public:
    explicit Cambia_actividad(QWidget *parent = nullptr);
    ~Cambia_actividad();
    void pasa_actividad(QString cod_actividad);
    QString nueva_actividad();
private:
    Ui::Cambia_actividad *ui;
};

#endif // CAMBIA_ACTIVIDAD_H
