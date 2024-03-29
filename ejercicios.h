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

#ifndef EJERCICIOS_H
#define EJERCICIOS_H

#include <QtGui>
#include "ui_ejercicios.h"
#include <QSqlQueryModel>

class CustomSqlModel3 : public QSqlQueryModel
    {
        Q_OBJECT

    public:
        CustomSqlModel3(QObject *parent = 0);
        QVariant data(const QModelIndex &item, int role) const;
        QVariant datagen(const QModelIndex &index, int role) const;
    };



class ejercicios : public QDialog {
    Q_OBJECT
	public:
                ejercicios();
                ~ejercicios();
        private:
                Ui::ejercicios ui;
                CustomSqlModel3 *model;
                void refrescar();
                QString ejercicioactual();
                QString num_recepcion_actual();
                bool bloqueado_actual();

        private slots:
                void nuevejercicio();
                void borraejercicio();
                void editacierre();
                void editaperiodos();
                void procesanumeracion();
                void procesanum_recepcion();
                void on_bloquea_pushButton_clicked();
};





#endif 
