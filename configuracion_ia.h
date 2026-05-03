#ifndef CONFIGURACION_IA_H
#define CONFIGURACION_IA_H

#include <QDialog>

namespace Ui {
class Configuracion_IA;
}

class Configuracion_IA : public QDialog
{
    Q_OBJECT

public:
    explicit Configuracion_IA(QWidget *parent = nullptr);
    ~Configuracion_IA();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Configuracion_IA *ui;
};

#endif // CONFIGURACION_IA_H
