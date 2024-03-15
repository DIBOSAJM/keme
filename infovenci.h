#ifndef INFOVENCI_H
#define INFOVENCI_H

#include <QDialog>

namespace Ui {
class infovenci;
}

class infovenci : public QDialog
{
    Q_OBJECT

public:
    explicit infovenci(QWidget *parent = 0);
    ~infovenci();

private:
    Ui::infovenci *ui;
    bool haycoma, decimales;
    QString archivo;
    void generalatex();

private slots:
    void refrescar();
    void limpiar();
    void imprimir();
    void ed_latex();
};

#endif // INFOVENCI_H
