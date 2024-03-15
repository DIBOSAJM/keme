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

#ifndef IMPRIMEDIARIO_H
#define IMPRIMEDIARIO_H

#include <QtGui>
#include "ui_imprimediario.h"
#include <qtrpt.h>


class imprimediario : public QDialog {
    Q_OBJECT
      public:
                imprimediario();
                void pasafiltro( QString qfiltro );
                void pasafiltro_asiento(QString zfiltro);
      private:
                Ui::imprimediario ui;
                bool comadecimal,decimales;
                QString filtro;
                QString guardafiltro;
                QString filtro_asiento;
                void generalatexdiario();
                void informe();               
                QString cadcsv();
                QString qfiltro();

      private slots:
                void visualizar();
                void imprimir();
                void setValueImage(const int recNo, const QString paramName, QImage &paramValue, const int reportPage);
                void latex();
                void generaXML();
                void copiar();
                void guardacsv();
                void on_imprime_asiento_checkBox_toggled(bool checked);
};



#endif 
