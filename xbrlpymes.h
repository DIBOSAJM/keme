#ifndef XBRLPYMES_H
#define XBRLPYMES_H

#include <QDialog>
#include "patrimonio.h"

namespace Ui {
    class xbrlpymes;
}

class xbrlpymes : public QDialog
{
    Q_OBJECT

public:
    explicit xbrlpymes(QWidget *parent = 0);
    ~xbrlpymes();
protected:
    Ui::xbrlpymes *ui;
    QString noejercicio();

private:
    virtual QString generar_contenido();
    QString bal_pymes_oficial;
    QString pyg_pymes_oficial;
    QString balance_situacion(QString anyo_cierre, int pos);
    QString pyg(QString anyo_cierre, int pos);
    QString ecpn(QString anyo_actual, QString anyo_anterior);
    QString varpat_instante(int fila, QString etiqueta, QString periodo, patrimonio *p);
    QString dimensiones_ecpn(QString anyo_cierre_actual, QString anyo_cierre_anterior,
                             QDate apertura_actual, QDate cierre_actual,
                             QDate apertura_anterior, QDate cierre_anterior);


private slots:
    void gen_archivo();
};


class xml_is_pymes : public xbrlpymes {
      Q_OBJECT
        public:
                xml_is_pymes();
                // ~buscacuenta();
        private :
                virtual QString generar_contenido();
                QString bal_pymes_is;
                QString pyg_pymes_is;


};



#endif // XBRLPYMES_H
