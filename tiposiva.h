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

#ifndef TIPOSIVA_H
#define TIPOSIVA_H
#include <QSqlTableModel>

#include <QtGui>
#include "ui_tiposiva.h"
#include <QSqlTableModel>

class CustomSqlModelT : public QSqlTableModel
    {
        Q_OBJECT

    public:
        CustomSqlModelT(QObject *parent = 0);
        QVariant data(const QModelIndex &item, int role) const;
    private:
        bool comadecimal;
        // qlonglong primas;
    };



class tiposiva : public QDialog {
    Q_OBJECT
	public:
                tiposiva();
                ~tiposiva();
                void pasacodigo( QString qconcepto );
        private:
                Ui::tiposiva ui;
                CustomSqlModelT *modelotiposiva;
                bool comadecimal;
        private slots :
                void compruebacodigo();
                void tipocambiado();
                void recambiado();
                void descripcioncambiada();
                void botonguardarpulsado();
                void botoncancelarpulsado();
                void botoneliminarpulsado();
                void tablapulsada();
                void finedictipo();
                void finedicre();

};





#endif
