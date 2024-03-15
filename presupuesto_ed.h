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

#ifndef PRESUPUESTO_ED_H
#define PRESUPUESTO_ED_H

#include <QtGui>
#include "ui_presupuesto_ed.h"
#include <QSqlTableModel>

class CustomSqlModeledpres : public QSqlQueryModel
    {
        Q_OBJECT

    public:
        CustomSqlModeledpres(QObject *parent = 0);
        void pasainfo(bool qcomadecimal, bool qsindecimales);
        QVariant data(const QModelIndex &item, int role) const;
        QVariant datagen(const QModelIndex &index, int role) const;
    private:
        bool comadecimal, sindecimales;
        // qlonglong primas;
    };

class presupuesto_ed : public QDialog {
    Q_OBJECT
	public:
                presupuesto_ed(bool qsubcuentas);
                ~presupuesto_ed();
                void pasaejercicio(QString qejercicio);
                void cargaespec(QString base_ejercicio, QString base_presupuesto,
                                bool presupuesto_base_cero, QString incremento);
                int fila_tabla_pre();
                void ir_fila_pre(int fila);

        private:
                Ui::presupuesto_ed ui;
                QSqlTableModel *modeloplan;
                CustomSqlModeledpres *modelpre;
                QString ejercicio;
                bool comadecimal,sindecimales;
                int radioactual;
                bool subcuentas;
                void calcbase(QString cuenta);
                void grabaespec();
                void desactivaimportpres();
                void activaimportpres();
                void calcbasepresupuesto(QString cuenta);
                void calcpresupuesto(QString cuenta);
                void borraimport(QString cuenta);
                void generalatex();
        private slots:
                void refrescalistacuentas();
                void pasaselec();
                void aceptar();
                void presupuestocuenta();
                void refrescapres();
                void borratodo();
                void incremento_fin_edic();
                void borralinea();
                void pasatodasctas();
                void importacuentaspresupuesto();
                void imprime();
};


#endif
