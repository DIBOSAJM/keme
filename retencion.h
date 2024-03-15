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

#ifndef RETENCION_H
#define RETENCION_H

#include <QtGui>
#include "ui_retencion.h"



class retencion : public QDialog {
    Q_OBJECT
      public:
                retencion(bool comadecimal, bool decimales);
                void pasadatos(QString qcuentaretencion,
                               QString qbase_percepciones,
                               QString retencion);
                void recuperadatos(QString *cta_retenido, bool *ret_arrendamiento,
                              QString *clave, QString *base_percepciones,
                              QString *tipo_ret, QString *retencion,
                              QString *ing_cta, QString *ing_cta_repercutido,
                              QString *nombre_ret, QString *cif_ret, QString *provincia);
                void pasadatos_all(QString qcuentaretencion,
                                   QString cta_retenido, bool ret_arrendamiento,
                                   QString clave, QString base_percepciones,
                                   QString tipo_ret, QString retencion,
                                   QString ing_cta, QString ing_cta_repercutido,
                                   QString nombre_ret, QString cif_ret, QString provincia);
                void pasadatos2(QString qcuentaretencion,
                                bool ret_arrendamiento,
                                QString clave, QString base_percepciones,
                                QString tipo_ret);
                void modoconsulta();
      private:
                Ui::retencion ui;
                bool comadecimal;
                bool decimales;
                void cheq_aceptar();

      private slots :
              void buscactaretenido();
              void ctaretenidocambiada();
              void ctaretenidofinedicion();

              void base_o_tipo_cambiado();
              void dinerario_cambiado();
              void especie_cambiado();
              void ing_a_cta_replineEdit_cambiado();
              void aceptar_pulsado();

};



#endif 
