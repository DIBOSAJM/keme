#ifndef RESPUESTA_SII_H
#define RESPUESTA_SII_H

#include <QDialog>
#include <QDomDocument>
#include <QProgressDialog>

namespace Ui {
class respuesta_sii;
}

class respuesta_sii : public QDialog
{
    Q_OBJECT

public:
    explicit respuesta_sii(QWidget *parent = 0);
    ~respuesta_sii();
    void pasa_dom(QDomDocument qdom);
    int filas();
    QString contenido_celda(int fila, int columna);
    void pararecibidas();
    void activa_pruebas();

private:
    Ui::respuesta_sii *ui;
    QDomDocument doc;
    bool recibidas;
    bool pruebas;

private slots:
    void verxml();
    void copiar();
};

#endif // RESPUESTA_SII_H
