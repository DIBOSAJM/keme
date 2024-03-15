#ifndef LISTA_FACT_PREDEF_H
#define LISTA_FACT_PREDEF_H

#include <QtGui>
#include "ui_lista_fact_predef.h"
#include <QSqlTableModel>

class lista_fact_predef : public QDialog {
    Q_OBJECT
        public:
                lista_fact_predef();
                ~lista_fact_predef();

        private:
           Ui::lista_fact_predef ui;
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
