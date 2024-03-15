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

#ifndef ITEMPLANAMORT_H
#define ITEMPLANAMORT_H

#include <QtGui>
#include "ui_itemplanamort.h"
#include <QSqlTableModel>

class CustomSqlModelIT : public QSqlTableModel
    {
        Q_OBJECT

    public:
        CustomSqlModelIT(QObject *parent = 0);
        QVariant data(const QModelIndex &item, int role) const;
    private:
        bool comadecimal;
    };


class itemplanamort : public QDialog {
    Q_OBJECT
	public:
                itemplanamort();
                void pasaelemactivo( QString codcta );
                ~itemplanamort();
        private:
                Ui::itemplanamort ui;
                bool comadecimal;
                bool decimales;
                QString ctaactivo;
                CustomSqlModelIT *modeloamortci;
                void actualizasumaci();
        public:
                void pasainfo(QString codigo, QDate fecha, QString cuentaaa,
                              QString cuentaam, QString coefcontable, QString residual,
                              QString coeffiscal, bool acelerada, bool amortpersonalizada,
                              bool baja, QDate fecha_baja, QString motivo_baja,
                              QString cuenta_proveedor, QString externo);

        private slots:
                void cuentaaacambiada();
                void ctaamcambiada();
                void fuerafococtaaa();
                void fuerafococtaam();
                void buscactaaa();
                void buscactaam();
                void f1cambiado();
                void coefcontcambiado();
                void residualcambiado();
                void coeffiscalcambiado();
                void tablapersoncheck();
                void tablaposcambiada( int row, int col );
                void tablavalorcambiado(int fila);
                void botonaceptar();
                void buscapulsado();
                void fijadescripciones();

                void compruebaci();
                void cicambiado();
                void asignacioncambiada();
                void aceptarcipulsado();
                void botoncancelarcipulsado();
                void botonborrarcipulsado();
                void tablacipulsada();
                void cuenta_prov_cambiada();
                void externo_cambiado();
                void fuerafococta_prov();
                void buscactaprov();
                void check_baja_cambiado();
                void buscaexterno();
};



#endif
