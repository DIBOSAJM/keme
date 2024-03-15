#ifndef IMPORT_CPLUS_H
#define IMPORT_CPLUS_H

#include <QDialog>

namespace Ui {
    class import_cplus;
}

class import_cplus : public QDialog
{
    Q_OBJECT

public:
    explicit import_cplus(QWidget *parent = 0);
    ~import_cplus();

private:
    Ui::import_cplus *ui;

private slots:
    void ruta_pulsado();
    void mostrar_ancho_auxiliares();
    void procesar();
};

#endif // IMPORT_CPLUS_H
