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

#ifndef EIB_H
#define EIB_H

#include <QtGui>
#include "ui_eib.h"


class eib : public QDialog {
    Q_OBJECT
	public:
                eib(bool concomadecimal, bool condecimales);
                void pasadatos( QString cuentabase, QString baseimponible, QString ctafra,
                                QDate qfechafra, QDate fechaop, QString claveop );
                void recuperadatos( QString *cuentabase, QString *baseimponible,  QString *ctafra,
                                    QDate *qfechafra, QDate *qfechaop, QString *claveop,
                                    QString *prservicios);
                void hazbaseoblig();
                void selec_prservicios();
                void modoconsulta();
                void esconde_externo();
                QString externo();

        private:
                Ui::eib ui;
                bool comadecimal;
                bool decimales;
                bool baseoblig;
                void cargacombooperaciones();
        private slots:
                void ctabasecambiada();
                void ctabasefinedicion();
                void ctafracambiada();
                void ctafrafinedicion();
                void botonaceptar();
                void chequeabotonaceptar();
                void buscactabase();
                void buscactafra();
                void fechafracambiada();
                void buscar_externo();
                void externo_cambiado();
};



#endif 
 
