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

#ifndef PLANAMORTIZACIONES_H
#define PLANAMORTIZACIONES_H

#include <QtGui>
#include "ui_planamortizaciones.h"
#include <QSqlTableModel>


class CustomSqlModel2 : public QSqlTableModel
    {
        Q_OBJECT

    public:
        CustomSqlModel2(QObject *parent = 0);
        QVariant data(const QModelIndex &item, int role) const;
        QVariant datagen(const QModelIndex &index, int role) const;
    private:
        bool comadecimal, sindecimales;
        // qlonglong primas;
    };


class planamortizaciones : public QDialog {
    Q_OBJECT
	public:
                planamortizaciones();
                ~planamortizaciones();
        private:
                Ui::planamortizaciones ui;
                bool comadecimal, sindecimales;
                CustomSqlModel2 *model;
                void refrescar();
                QString cuentaactivoactual();
                void generalatex();
        private slots :
                void nuevoelemento();
                void editarelemento();
                void eliminarelemento();
                void imprimir();
};





#endif 
