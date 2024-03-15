#ifndef LISTAREFS_H
#define LISTAREFS_H

#include <QtGui>
#include "ui_listarefs.h"
#include <QSqlTableModel>

class listarefs : public QDialog {
    Q_OBJECT
        public:
                listarefs();
                ~listarefs();

        private:
           Ui::listarefs ui;
           QSqlTableModel *modeloreg;
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

#endif // LISTAREFS_H
