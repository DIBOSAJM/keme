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

#ifndef LIBRO_RECIBIDAS_H
#define LIBRO_RECIBIDAS_H

#include <QtGui>
#include "ui_libro_recibidas.h"



class libro_recibidas : public QDialog {
    Q_OBJECT
      public:
                libro_recibidas(QString usuario);
      private:
                Ui::libro_recibidas ui;
                bool comadecimal,decimales;
                bool generalatexsoportado();
                bool ordenrecepcion;
                QString nombrefichero();
                QString usuario;
                bool generalatexsoportado_horizontal();
                bool generalatexsoportado_horizontal_bi();
                bool generalatexprorrata();
                void informe();
                void copiavertical();
                void copiahorizontal();
                void copiahorizontal_bi();
                void copiaprorrata();

      private slots:
               void prorratacambiado();
               void bicheckcambiado();
               void consultatex();
               void imprime();
               void copiar();
               void latex();
               void tabla();
               void cuenta_finedicion();
               void boton_cuenta_pulsado();
               void cuenta_cambiada();
               void externo_cambiado();
               void buscaexterno();
               void check_grupo_externo_cambiado();

               void on_documentos_pushButton_clicked();
};



#endif

