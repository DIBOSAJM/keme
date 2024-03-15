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

#ifndef RELOP_H
#define RELOP_H

#include <QtGui>
#include "ui_relop.h"



class relop : public QDialog {
    Q_OBJECT
      public:
                relop();
                void prepara182();
      private:
                Ui::relop ui;
                bool comadecimal,decimales;
                bool m182;
                void borratabla();
                void generalatex();
                bool genfich182(QString nombre);
      private slots:
                void cargadatos();
                void radioadquisicionescambiado();
                void radioventascambiado();
                void importecambiado();
                void finedicimporte();
                void imprime();
                void combocambiado();
                void latex();
                void datoscuenta();
                void borralinea( void );
                void genera182();
                void copiar();

};



#endif 
