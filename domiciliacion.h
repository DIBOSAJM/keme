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

#ifndef DOMICILIACION_H
#define DOMICILIACION_H

#include <QtGui>
#include "ui_domiciliacion.h"
#include <QSqlTableModel>

class CustomSqlModel_dom : public QSqlTableModel
    {
        Q_OBJECT

    public:
        CustomSqlModel_dom(QObject *parent = 0);
        QVariant data(const QModelIndex &item, int role) const;
        QVariant datagen(const QModelIndex &index, int role) const;
        QString nombre_columna(int columna);
    private:
        bool comadecimal, decimales;
        // qlonglong primas;
    };


class domiciliacion : public QDialog {
    Q_OBJECT
      public:
                domiciliacion(QString qusuario);
                void carga_domiciliacion(QString id);
                void para_pago();
                ~domiciliacion();

      private:
                Ui::domiciliacion ui;
                void asigna_id_remesa();
                void guarda_cabecera_remesa();
                CustomSqlModel_dom *model;
                bool comadecimal, decimales;
                QString usuario;
                bool guardado;
                void asignafiltro();
                void calculasuma();
                void informelatex();
                bool pago=false;
                QString gen_xml_sepa_adeudos_iso20022();
                QString gen_xml_cuad34_transf();
      private slots:
               void botoncodigo_pulsado();
               void datos_banco();
               void cargar();
               void refrescar();
               void infocuentalin(QModelIndex primaryKeyIndex);
               void borrar_seleccion();
               void salir();
               void auxbanco_finedic();
               void c19();
               void c58();
               void sepa19_14();
               void sepa_xml();
               void check19_cambiado();
               void check58_cambiado();
               void checksepa1914cambiado();
               void registrar_realizacion();
               void checkprocesado();
               void impagados();
               void imprime();

};



#endif 
