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

#ifndef LIBRO_EMITIDAS_H
#define LIBRO_EMITIDAS_H

#include <QtGui>
#include "ui_libro_emitidas.h"



class libro_emitidas : public QDialog {
    Q_OBJECT
      public:
                libro_emitidas(QString usuario);
      private:
                Ui::libro_emitidas ui;
                bool comadecimal,decimales;
                QString usuario;
                bool generalatexrepercutido();
                QString nombrefichero();
                bool generalatexrepercutido_horizontal();
                bool generalatexrepercutido_horizontal_bi();
                void copiavertical();
                void copiahorizontal();
                void copiahorizontal_bi();
                void informe();

      private slots:
                void consultatex();
                void imprime();
                void copiar();
                void bicheckcambiado();
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

