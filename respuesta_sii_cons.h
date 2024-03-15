#ifndef RESPUESTA_SII_CONS_H
#define RESPUESTA_SII_CONS_H

#include <QDialog>
#include <QDomDocument>
#include <QProgressDialog>

namespace Ui {
class respuesta_sii_cons;
}

class respuesta_sii_cons : public QDialog
{
    Q_OBJECT

public:
    explicit respuesta_sii_cons(QWidget *parent = 0);
    ~respuesta_sii_cons();
    void pasa_dom(QDomDocument qdom);
    int filas();
    QString contenido_celda(int fila, int columna);
    void pararecibidas();

private:
    Ui::respuesta_sii_cons *ui;
    QDomDocument doc;
    bool recibidas;

private slots:
    void verxml();
    void copiar();
};

#endif // RESPUESTA_SII_CONS_H
