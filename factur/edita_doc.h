#ifndef EDITA_DOC_H
#define EDITA_DOC_H

#include <QtGui>
#include <QDialog>
#include "ui_edita_doc.h"


class edita_doc : public QDialog {
    Q_OBJECT
        public:
                edita_doc();
                void pasacodigo(QString codigo);
                QString fototexto();

        private:
           Ui::edita_doc ui;
       private slots:
           void terminar();
           void importafoto();
           void borrafoto();
           void carga_fich_jasp();
           void on_operacioncomboBox_currentIndexChanged(int index);
};


#endif // EDITA_DOC_H
