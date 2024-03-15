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

#ifndef NUEVO_VENCIMIENTO_H
#define NUEVO_VENCIMIENTO_H

#include <QtGui>
#include "ui_nuevo_vencimiento.h"

class nuevo_vencimiento : public QDialog {
    Q_OBJECT
	public:
                nuevo_vencimiento();
                void pasaInfo(QString cuentaordenante, QDate fechaoperacion, QString importe, QString externo="");
                void pasaInfo2(QString ctatesoreria, QDate qvencimiento,
                                QString qconcepto, bool domiciliable , QString externo="");
                void cambiavencimiento( QString cadnumvto );
                void guardanuevovencimiento( QString pase );
        private:
                Ui::nuevo_vencimiento ui;
                bool comadecimal,decimales;
                void chequeaintro();

        private slots:
                void buscactaordenante();
                void ctaordenantecambiada();
                void expandepuntoordenante();
                void buscactatesoreria();
                void ctatesoreriacambiada();
                void expandepuntotesoreria();
                void importecambiado();
                void buscaexterno();
                void externo_cambiado();

};


#endif 
 
