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

#ifndef CONSINPUT_H
#define CONSINPUT_H

#include <QtGui>
#include "ui_consinput.h"

class consinput : public QDialog {
    Q_OBJECT
	public:
                consinput(QString qusuario);
                void pasadatos( QString qci, QDate qfecha );

        private:
                Ui::consinput ui;                
                bool comadecimal;
                bool decimales;
                QString usuario;
                void terminaedicion();
                void generalatex();

        private slots:
                void buscapulsado();
                void fijadescripciones();
                void refrescar();
                void editarpulsado();
                void imprimir();
                void visdoc();
                void copiar();
                void latex();
                void botoncuentapulsado();
                void ctaexpandepunto();
                void consultarpulsado();

};


#endif 
