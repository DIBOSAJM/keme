/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)

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

#ifndef ASIGNA_FICHDOC_H
#define ASIGNA_FICHDOC_H

#include <QtGui>
#include "ui_asigna_fichdoc.h"
#include "etiqueta_fich.h"


class asigna_fichdoc : public QDialog {
    Q_OBJECT
      public:
                asigna_fichdoc(QString elasiento, QString eldocumento,QString ejercicio);
                ~asigna_fichdoc();
                void pasafichdoc(QString cadena);
                QString ruta_docs();
      private:
                Ui::asigna_fichdoc ui;
                QString asiento;
                QString documento;
                QString directorio_doc;
                CustomQLabel *etiqueta;
                void guarda_doc();
      private slots:
                void visdocdef();
                void suprimedocdef();
                void buscacopiadoc();
                void visdoc();
                void borraasignaciondoc();
                void botonaceptar();
                void terminar_droped();
};



#endif 
