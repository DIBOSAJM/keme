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

#ifndef ASIGNAINPUTCTA_H
#define ASIGNAINPUTCTA_H

#include <QtGui>
#include "ui_asignainputcta.h"
#include <QSqlTableModel>

class CustomSqlModelITA : public QSqlTableModel
    {
        Q_OBJECT

    public:
        CustomSqlModelITA(QObject *parent = 0);
        QVariant data(const QModelIndex &item, int role) const;
    private:
        bool comadecimal;
    };


class asignainputcta : public QDialog {
    Q_OBJECT
	public:
                asignainputcta();
                void pasacta( QString codcta );
                ~asignainputcta();
        private:
                Ui::asignainputcta ui;
                bool comadecimal;
                bool decimales;
                QString ctaactivo;
                CustomSqlModelITA *modeloamortci;
                void actualizasumaci();

        private slots:
                void fijadescripciones();

                void compruebaci();
                void cicambiado();
                void buscapulsado();
                void asignacioncambiada();
                void aceptarcipulsado();
                void botoncancelarcipulsado();
                void botonborrarcipulsado();
                void tablacipulsada();
};



#endif
