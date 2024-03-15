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

#ifndef LISTAVENCI_H
#define LISTAVENCI_H

#include <QtGui>
#include "ui_listavenci.h"
#include <QSqlTableModel>


class CustomSqlModel5 : public QSqlTableModel
    {
        Q_OBJECT

    public:
        CustomSqlModel5(QObject *parent = 0);
        QVariant data(const QModelIndex &item, int role) const;
        QVariant datagen(const QModelIndex &index, int role) const;
    private:
        bool comadecimal, decimales;
        // qlonglong primas;
    };


class listavenci : public QDialog {
    Q_OBJECT
        public:
                 listavenci(QString qusuario);
                ~listavenci();
                void esconde_domiciliables();
                void pasa_int_fechas_vencimiento(QDate fechaini, QDate fechafin, int margen);
                void pasa_cta_tesoreria(QString cuenta);
        private:
                Ui::listavenci ui;
                bool comadecimal, decimales;
                QString usuario;
                CustomSqlModel5 *model;
                void asignafiltro();
                QString vencimientoactual();
                void calculasuma();
                void generalatex();
                void generalatex2();
       private slots:
                void estadogrupofanotcambiado();
                void estadogrupovencicambiado();
                void cuentacheckBoxcambiado( int activado );
                void trasctaordenante();
                void buscacuenta();
                void ctaordenantecambiada();
                void cuentatcheckBoxcambiado( int activado );
                void trasctatordenante();
                void ctatordenantecambiada();
                void buscacuentat();
                void externo_checkBoxcambiado( int activado );
                void busca_ext();
                void externo_cambiado();
                void infocuentalin(QModelIndex primaryKeyIndex);
                void infovencipase();
                void procesar();
                void anular();
                void borrar();
                void unificarproc();
                void imprimir();
                void imprimir2();
                void copia_informe2();
                void consulta_asiento();
                void edita_asiento();
                void consulta_asiento_pago();
                void edita_asiento_pago();
                void editavenci();
                void copiar();
                void importe_desde_cambiado();
                void importe_hasta_cambiado();
                void checkpendientes_procesados_cambiado();
        public slots:
                void refrescar();

};

#endif
