#ifndef BUSCA_EXTERNO_H
#define BUSCA_EXTERNO_H

#include <QDialog>

namespace Ui {
class busca_externo;
}

class busca_externo : public QDialog
{
    Q_OBJECT

public:
    explicit busca_externo(QWidget *parent = 0);
    ~busca_externo();
    QString codigo_elec();
    QString nombre_elec();

private:
    Ui::busca_externo *ui;
    void carga_datos();
    QString codigo;
    QString nombre;

private slots:
    void fila_tabla_clicked();
    void refrescar();


};

#endif // BUSCA_EXTERNO_H
