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

#ifndef DOMICILIACIONES_H
#define DOMICILIACIONES_H

#include <QtGui>
#include "ui_domiciliaciones.h"
#include <QSqlTableModel>

class CustomSqlModel_doms : public QSqlTableModel
    {
        Q_OBJECT

    public:
        CustomSqlModel_doms(QObject *parent = 0);
        QVariant data(const QModelIndex &item, int role) const;
        QVariant datagen(const QModelIndex &index, int role) const;
        QString nombre_columna(int columna);
    private:
        bool comadecimal, decimales;
        bool pagos=false;
        // qlonglong primas;
    };


class domiciliaciones : public QDialog {
    Q_OBJECT
      public:
              domiciliaciones(QString qusuario);
              void para_pagos();
      private:
              Ui::domiciliaciones ui;
              CustomSqlModel_doms  *modelo;
              QString usuario;
              bool pagos=false;
              void refrescar();

      private slots:
              void nueva();
              void editar();
              void borrar();

};



#endif 
