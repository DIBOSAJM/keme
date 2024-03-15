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

#ifndef FACTURAS_H
#define FACTURAS_H

#include <QtGui>
#include "ui_facturas.h"
#include <QSqlQueryModel>

class FacSqlModel : public QSqlQueryModel
    {
        Q_OBJECT

    public:
        FacSqlModel(QObject *parent = 0);
        QVariant data(const QModelIndex &item, int role) const;
        QVariant datagen(const QModelIndex &index, int role) const;
    private:
        bool comadecimal, decimales;
    };



class facturas : public QDialog {
    Q_OBJECT
      public:
                facturas(QString qusuario);
                void activaconfiltro();
                void pasafiltro(QString filtro);
                void refresca();
                void escondesalir();

      private:
                Ui::facturas ui;
                FacSqlModel *model;
                QString guardafiltro;
                QString serie();
                QString numero();
                QString apunte_diario();
                QString usuario;
                void actufiltro();
                void latexinforme();
                int numero_de_filas();
                bool comadecimal, decimales;
                void informe (QString qserie, QString qnumero);


      private slots:
                void nuevafactura();
                void editafactura();
                void exectrasp();
                void xmldoc();
                void genera_facturae();
                void latexdoc();
                void contabilizar();
                void contabilizartodas();
                void borralinea();
                void consultaasiento();
                void numini_cambiado();
                void cuentaini_cambiada();
                void cuentaini_finedicion();
                void cuentafin_finedicion();
                void boton_refrescar();
                void consultainforme();
                void consulta();
                void borraasiento();
                void edita_tex();
                void informe_documento();

};



#endif 
