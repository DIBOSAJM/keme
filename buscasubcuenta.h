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

#ifndef BUSCASUBCUENTA_H
#define BUSCASUBCUENTA_H
#include <QSqlTableModel>

#include <QtGui>
#include "ui_buscasubcuenta.h"


class buscasubcuenta : public QDialog {
    Q_OBJECT
	public:

                buscasubcuenta(QString cadena);
                ~buscasubcuenta();
                virtual QString seleccioncuenta( void );
                void cambiatitulo(QString titulo);
        protected:
                Ui::buscasubcuenta ui;
                QSqlTableModel *modeloplan;
                virtual QString filtroini();
        protected slots :
                void ComienceRadioBotoncambiado();
                void ContengaRadioBotoncambiado();
                void recargartabla(QString cadena);
                void codigosubcuentacambiado();
                void titulocuentacambiado();
                virtual void fuerafocosubcuenta();
                void tablaclickada();

};



/*class CustomSqlModel : public QSqlQueryModel
    {
        Q_OBJECT

    public:
        CustomSqlModel(QObject *parent = 0);
        void pasainfo(bool qcomadecimal, bool qsindecimales);
        QVariant data(const QModelIndex &item, int role) const;
        QVariant datagen(const QModelIndex &index, int role) const;
        // void chknumeracion();
    private:
        // bool primerasiento1; */


class buscacuenta : public buscasubcuenta {
      Q_OBJECT
        public:
                buscacuenta(QString cadena);
                virtual QString seleccioncuenta( void );
                // ~buscacuenta();
        protected :
                 virtual QString filtroini();
        protected slots :
                virtual void fuerafocosubcuenta();

};




#endif 
 
 
