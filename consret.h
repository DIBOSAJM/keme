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

#ifndef CONSRET_H
#define CONSRET_H

#include <QtGui>
#include "ui_consret.h"



class consret : public QDialog {
    Q_OBJECT
      public:
                consret(QString qusuario);

      private:
                Ui::consret ui;
                bool comadecimal, decimales;
                QString usuario;

      private slots:
                void arrendamientoscambiado();
                void excluir_arrendamientoscambiado();
                void refrescar();
                void activarefrescar();
                void edit_asiento();
                void cons_asiento();
                void borra_asiento();
                void cambiaagrup();
                void copia_resumen();
                void copia_detalle();
                void generalatex();
                void imprimir();
                void edlatex();
                void ed_registro_ret();
                void cambiacuenpase();
                void actufechas();
                void on_detalletableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
};



#endif 
