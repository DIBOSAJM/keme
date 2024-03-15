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

#ifndef INV_INMOVILIZADO_H
#define INV_INMOVILIZADO_H

#include <QtGui>
#include "ui_inv_inmovilizado.h"
#include <QSqlTableModel>


class CustomSqlModel25 : public QSqlTableModel
    {
        Q_OBJECT

    public:
        CustomSqlModel25(QObject *parent = 0);
        QVariant data(const QModelIndex &item, int role) const;
        QVariant datagen(const QModelIndex &index, int role) const;
    private:
        bool comadecimal, sindecimales;
        // qlonglong primas;
    };


class inv_inmovilizado : public QDialog {
    Q_OBJECT
	public:
                inv_inmovilizado();
                ~inv_inmovilizado();
        private:
                Ui::inv_inmovilizado ui;
                bool comadecimal, sindecimales;
                CustomSqlModel25 *model;
                void refrescar();
                QString numactivoactual();
                void generalatex();
        private slots :
                void nuevoelemento();
                void editarelemento();
                void borraelemento();
                void imprimir();
};





#endif 
