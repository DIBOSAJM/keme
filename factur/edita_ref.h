#ifndef EDITA_REF_H
#define EDITA_REF_H

#include <QtGui>
#include <QDialog>
#include "ui_edita_ref.h"


class edita_ref : public QDialog {
    Q_OBJECT
        public:
                edita_ref();
                void pasacodigo(QString codigo);

        private:
           Ui::edita_ref ui;
           bool coma,decimales;
       private slots:
           void terminar();
           void ivacombocambiado();
           void ivarecombocambiado();
           void pvpfinedic();
           void buscacta();
           void cuentacambiada();
           void ctaexpandepunto();

           void suplidos_check();
           void anticipos_check();

};


#endif // EDITA_REF_H
