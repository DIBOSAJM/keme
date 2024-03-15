/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)  José Manuel Díez Botella

    Este programa es software libre: usted puede redistribuirlo y/o modificarlo 
    bajo los términos de la Licencia Pública General GNU publicada 
    por la Fundación para el Software Libre, ya sea la versión 3 
    de la Licencia, o (a su elección) cualquier versión posterior.

    Este programa se distribuye con la esperanza de que sea útil, pero 
    SIN GARANTÍA ALGUNA; ni siquiera la garantía implícita 
    MERCANTIL o de APTITUD PARA UN PROPÓSITO DETERMINADO. 
    Consulte los detalles de la Licencia Pública General GNU para obtener 
    una información más detallada. 

    Debería haber recibido una copia de la Licencia Pública General GNU 
    junto a este programa. 
    En caso contrario, consulte <http://www.gnu.org/licenses/>.
  ----------------------------------------------------------------------------------*/

#ifndef EDIT_CONC_DOC_H
#define EDIT_CONC_DOC_H

#include <QtGui>
#include "ui_edit_conc_doc.h"



class edit_conc_doc : public QDialog {
    Q_OBJECT
      public:
                edit_conc_doc();
                void pasadatos( QString qnumpase, QDate qfecha, QString concepto, QString documento );
                void todo_el_asiento();
      private:
                Ui::edit_conc_doc ui;
                QDate fecha;
                QString numpase;
                bool asiento;
      private slots:
                void botonaceptar();

};



#endif 
 
