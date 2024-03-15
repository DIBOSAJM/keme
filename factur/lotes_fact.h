#ifndef LOTES_FACT_H
#define LOTES_FACT_H

#include <QtGui>
#include "ui_lotes_fact.h"
#include <QSqlTableModel>

class lotes_fact : public QDialog {
    Q_OBJECT
        public:
                lotes_fact();
                ~lotes_fact();

        private:
           Ui::lotes_fact ui;
           QSqlTableModel *modeloreg;
           void refrescar();
       private slots :
               void compruebacodigo();
               void codigocambiado();
               void descripcioncambiada();
               void botonguardarpulsado();
               void botoncancelarpulsado();
               void botoneliminarpulsado();
               void tablapulsada();
               void eddoc();
};

#endif // LISTADOCS_H
