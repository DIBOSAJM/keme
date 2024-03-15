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

#ifndef CONTA347_H
#define CONTA347_H

#include <QtGui>
#include "ui_conta347.h"



class conta347 : public QDialog {
    Q_OBJECT
      public:
                conta347(QString qusuario);
      private:
                Ui::conta347 ui;
                QString usuario;
                bool comadecimal,decimales;
                void generalatex();
                bool genfich347(QString nombre);
                void carga_agrup_nif();
                bool importar_csv(QString qfichero);
      private slots:
                void cargadatos();
                void borratabla();
                void importefinedic();
                void imprime();
                void latex();
                void datoscuenta();
                void borralinea( void );
                void combocambiado();
                void consultamayor();
                void genera347();
                void copiar();
                void gen_cartas_347_latex();
                void csv_import();

};



#endif
