/* ----------------------------------------------------------------------------------
    "patrimonio", estado de cambios en el patrimonio neto de pymes
     complemento de KEME-Contabilidad 2.0

    Copyright (C) 2007  José Manuel Díez Botella

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

#ifndef PATRIMONIO_H
#define PATRIMONIO_H

#include <QtGui>
#include "ui_patrimonio.h"



class patrimonio : public QDialog {
    Q_OBJECT
      public:
                patrimonio();
                void pasa_ejercicio(QString ejercicio);
                QString contenido(int fila, int columna);
                void modo_deposito();

      private:
                Ui::patrimonio ui;
                QStringList columnas;
                void generalatex();
                QString fichero;
                bool grabado(QString nombre);
                bool cargado(QString nombre);
                QString ejercicio;
                bool comadecimal, decimales;
                bool pregunta_salir;
      private slots:
                void gentex();
                void contenidocambiado(int fila,int col);
                void salir();
                void impritex();
                void grabafich();
                void recuperafich();
                void copiar();
                void calculo();
};



#endif 
