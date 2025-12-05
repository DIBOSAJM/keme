#ifndef RESPUESTA_VF_H
#define RESPUESTA_VF_H

#include <QDialog>
#include <QDomDocument>
#include <QProgressDialog>

namespace Ui {
class respuesta_vf;
}

class respuesta_vf : public QDialog
{
    Q_OBJECT

public:
    explicit respuesta_vf(QWidget *parent = 0);
    ~respuesta_vf();
    void pasa_dom(QDomDocument qdom);
    void pasa_dom_consulta_facts(QDomDocument qdom);
    int filas();
    QString contenido_celda(int fila, int columna);
    bool es_correcto();
    bool es_aceptado_con_errores();

private:
    Ui::respuesta_vf *ui;
    QDomDocument doc;
    bool correcto=false;
    bool aceptadoConErrores=false;
private slots:
    void verxml();
    void copiar();
};

#endif // RESPUESTA_VF_H
