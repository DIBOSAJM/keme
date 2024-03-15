/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2.1; aplicación para llevar contabilidades

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

#ifndef CARGAESPERA_H
#define CARGAESPERA_H

#include <QtGui>
#include "ui_cargaespera.h"
#include <QSqlQueryModel>


class CustomSqlModelC : public QSqlQueryModel
    {
        Q_OBJECT

    public:
        CustomSqlModelC(QObject *parent = 0);
        QVariant data(const QModelIndex &item, int role) const;
        QVariant datagen(const QModelIndex &index, int role) const;
    private:
        bool comadecimal, decimales;
    };


class cargaespera : public QDialog {
    Q_OBJECT
	public:
                cargaespera();
                ~cargaespera();
                qlonglong asientoactual();
                void elimasientoactual();
        private:
                Ui::cargaespera ui;
                bool comadecimal, decimales;
                CustomSqlModelC *model;
        private slots:
                void refrescar();
                void eliminarasiento();

};





#endif 
