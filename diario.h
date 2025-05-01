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

#ifndef DIARIO_H
#define DIARIO_H

#include <QtGui>
#include "ui_diario.h"
#include <QSqlQueryModel>
#include <QSqlRecord>
#include "funciones.h"

class CustomSqlModel : public QSqlQueryModel
    {
        Q_OBJECT

    public:
        CustomSqlModel(QObject *parent = 0);
        void pasainfo(bool qcomadecimal, bool qsindecimales);
        QVariant data(const QModelIndex &item, int role) const;
        QVariant datagen(const QModelIndex &index, int role) const;
        // void chknumeracion();
    private:
        // bool primerasiento1;
        bool comadecimal, sindecimales, tabla_ci;
        // qlonglong primas;
    };


class diario: public QWidget {
    Q_OBJECT
        public:
           diario();
            void dragEnterEvent(QDragEnterEvent *e);
            void dropEvent(QDropEvent *e);
           void activaconfiltro(QString filtro,bool qcomadecimal,bool sindecimales, QString usuario);
           void activaconfiltrob(QString filtro, bool qcomadecimal, bool qsindecimales);
           bool pasafiltro(QString filtro,bool qcomadecimal, bool sindecimales);
           bool pasafiltrob(QString filtro,bool qcomadecimal, bool sindecimales);
           void pasafiltroedlin(QString filtro);
           void pasafiltroedlinb(QString filtro);
           int anchocolumna(int columna);
           void pasaanchocolumna(int columna,int ancho);
           void pasaanchos(int ancho[]);
           void muestratabla(void);
           void refresca();
           void irfinal();
           void irafila(int fila);
           int fila_actual();
           QString subcuentaactual();
           qlonglong asientoactual();
           QString externoactual();
           bool apunterevisado();
           QDate fechapaseactual();
           QString conceptoactual();
           QString codigo_var_evpn_pymes();
           QString documentoactual();
           QString fichdocumentoactual();
           QModelIndex indiceactual(void);
           void situate(QModelIndex indice);
           CustomSqlModel *modelo();
           QTableView *tabladiario();
           QString ciactual();
           qlonglong paseactual();
           double debe();
           double haber();
           void asignaimagen(QPixmap imagen);
           void resetimagen();
           qlonglong clave_ci_actual();
           QDate fechasel();
           int columna_actual();
           void cabecera_campo_orden(int campo);
           void esconde_filtro();
           void muestra_filtro();
           bool borrador();
           QString file_droped();
           void activa_tab(int qtab);
           void actu_usuario();
           ~diario();
        private:
           Ui::diario ui;
           CustomSqlModel *model,*modelb;
           bool comadecimal, sindecimales;
           QString usuario;
           QString guardafiltro, guardafiltro2;
           QString fichero_droped;
           void renumera_borr();
       private slots:
           void infocuentapase(QModelIndex primaryKeyIndex);
           void editarasientod();
           void inicio();
           void fin();
           void editafechaasien();
           void edcondoc();
           void cambiacuenpase();
           void ed_registro_iva();
           void ed_registro_ret();
           void on_tabWidget_currentChanged(int index);
           void on_contabilizar_pushButton_clicked();

           void on_renum_borrador_pushButton_clicked();

       signals:
           void cambio_tab(int ntab);
           void filedroped();
};



#endif
