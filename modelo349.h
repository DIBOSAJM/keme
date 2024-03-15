#ifndef MODELO349_H
#define MODELO349_H

#include <QDialog>

namespace Ui {
class modelo349;
}

class modelo349 : public QDialog
{
    Q_OBJECT

public:
    explicit modelo349(QString qusuario, QWidget *parent = 0);
    ~modelo349();

private:
    Ui::modelo349 *ui;
    bool comadecimal, decimales;
    QString usuario;
    void generalatex();
    bool genfich349(QString nombre);

private slots:
    void actufechas();
    void cargadatos();
    void borratabla();
    void datoscuenta();
    void consultamayor();
    void borralinea( void );
    void imprime();
    void edlatex();
    void copiar();
    void genera349();

};

#endif // MODELO349_H
