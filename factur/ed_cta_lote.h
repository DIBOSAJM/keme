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

#ifndef ED_CTA_LOTE_H
#define ED_CTA_LOTE_H

#include <QtGui>
#include "ui_ed_cta_lote.h"



class ed_cta_lote : public QDialog {
    Q_OBJECT
      public:
                ed_cta_lote(bool comadecimal);
                QString cuenta();
                QString importe();
                QString cuota();
                QString descrip();
                QString externo();
                void con_cuota(bool cuota);
                void pasa_edicion(QString cuenta, QString importe, QString cuota, QString externo);

      private:
                Ui::ed_cta_lote ui;
                bool comadecimal;
      private slots:
                void buscacta();
                void ctaexpandepunto();
                void fin_edic_cuota();
                void fin_edic_importe();

                void buscar_externo();
                void campo_externo_cambiado();
                void borra_externo();
                void ver_externo();

};



#endif 
