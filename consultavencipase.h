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

#ifndef CONSULTAVENCIPASE_H
#define CONSULTAVENCIPASE_H

#include <QtGui>
#include "ui_consultavencipase.h"
#include <QSqlQueryModel>


class CustomSqlModel4 : public QSqlQueryModel
    {
        Q_OBJECT

    public:
        CustomSqlModel4(QObject *parent = 0);
        QVariant data(const QModelIndex &item, int role) const;
        QVariant datagen(const QModelIndex &index, int role) const;
    private:
        bool comadecimal, decimales;
        // qlonglong primas;
    };


class consultavencipase : public QDialog {
    Q_OBJECT
        public:
                 consultavencipase(QString qpase, QString asiento, QString debe,
                   QString haber, QString qcuentadiario,QDate qfechadiario, QString qexterno="");
                ~consultavencipase();
        private:
                Ui::consultavencipase ui;
                bool comadecimal, decimales;
                CustomSqlModel4 *model;
                QString eldebe;
                QString elhaber;
                QString cuentadiario;
                QString externo;
                QString sinvenci;
                QDate fechadiario;
                QString elpase;
                void calculasinvencimiento();
                void refrescar();
                QString vencimientoactual();
                bool pendiente( QString venci );
        private slots:
                void nuevovencimiento();
                void borravencimiento();
                void editavencimiento();
        public:
                bool vencimientoscubiertos();

};





#endif
