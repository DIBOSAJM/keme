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

#ifndef ASIGNAINPUTDIARIO_H
#define ASIGNAINPUTDIARIO_H

#include <QtGui>
#include "ui_asignainputdiario.h"



class asignainputdiario : public QDialog {
    Q_OBJECT
	public:
                asignainputdiario(bool qcomadecimal, bool qdecimales);
                void pasacta( QString codcta );
                void pasainputacion(QString CI, QString total, double porcentaje);
                bool tablavacia();
                void vaciatabla();
                void actualiza_porcentajes();
                void actualiza_total();
                bool input_OK();
                QStringList lista_ci();
                QStringList lista_total();
                void pasatotal(QString total);
                void activa_modoconsulta();
                bool en_edicion_ext();

        private:
                Ui::asignainputdiario ui;
                bool comadecimal;
                bool decimales;
                QString ctaactivo;
                bool modoconsulta;
                bool edicion_ext; // edición desde fuera tabla asientos

        private slots:
                void fijadescripciones();
                void aceptarcipulsado();
                void botonborrarcipulsado();
                void compruebaci();
                void cicambiado();
                void buscapulsado();
                void asignacioncambiada();
                void botoncancelarcipulsado();
                void tablacipulsada();
                void carga_input_predef();
                void activa_edicion();

};



#endif
