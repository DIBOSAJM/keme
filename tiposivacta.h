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

#ifndef TIPOSIVACTA_H
#define TIPOSIVACTA_H
#include <QSqlTableModel>

#include <QtGui>
#include "ui_tiposivacta.h"
#include <QSqlTableModel>

class CustomSqlModelT2 : public QSqlTableModel
    {
        Q_OBJECT

    public:
        CustomSqlModelT2(QObject *parent = 0);
        QVariant data(const QModelIndex &item, int role) const;
    private:
        bool comadecimal;
        // qlonglong primas;
    };



class tiposivacta : public QDialog {
    Q_OBJECT
	public:
                tiposivacta(bool es_soportado=false);
                ~tiposivacta();
                void pasacodigo( QString qconcepto );
        private:
                Ui::tiposivacta ui;
                CustomSqlModelT2 *modelotiposivacta;
                bool comadecimal;
                bool soportado;
        private slots :
                void compruebacodigo();
                // void tipocambiado();
                // void recambiado();
                // void descripcioncambiada();
                void cuentacambiada();
                void botonguardarpulsado();
                void botoncancelarpulsado();
                //void botoneliminarpulsado();
                void tablapulsada();
                void buscacta();
                void ctaexpandepunto();

};





#endif
