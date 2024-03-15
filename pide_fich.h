#ifndef PIDE_FICH_H
#define PIDE_FICH_H

#include <QDialog>

namespace Ui {
class pide_fich;
}

class pide_fich : public QDialog
{
    Q_OBJECT

public:
    explicit pide_fich(QWidget *parent = 0);
    ~pide_fich();
    void pasa (QString titulo_ventana, QString etiqueta);
    void pasa_nombre_fichero(QString nombre);
    QString nombre_fichero();
    void pasadir(QString dir);


private:
    Ui::pide_fich *ui;
    QString caddir;

private slots:
    void carga_fich();

};

#endif // PIDE_FICH_H
