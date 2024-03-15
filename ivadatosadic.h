/* ----------------------------------------------------------------------------------
    KEME-Contabilidad; aplicación para llevar contabilidades

    Copyright (C)   José Manuel Díez Botella

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

#ifndef IVADATOSADIC_H
#define IVADATOSADIC_H

#include <QtGui>
#include "ui_ivadatosadic.h"
#include "etiqueta_fich.h"


class ivadatosadic : public QDialog {
    Q_OBJECT
	public:
                ivadatosadic();
                void pasafecha( QDate fecha );
                QString documento();
                QString fichdocumento();
                QDate fechaasiento();
                bool esverificar();
                void pasaconcepto(QString concepto);
                QString recuperaconcepto();
                bool navegar_anterior();

         private slots:
                void buscacopiadoc();
                void visdoc();
                void borraasignaciondoc();
                void doc_cambiado();
                void fichero_soltado();
                void on_anterior_pushButton_clicked();

private:
                Ui::ivadatosadic ui;
                QString guarda_concepto;
                CustomQLabel *etiqueta;
                bool anterior;
};



#endif 
 
 
