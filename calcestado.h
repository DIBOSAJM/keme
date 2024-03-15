/* ----------------------------------------------------------------------------------
    KEME-Contabilidad 2.1; aplicación para llevar contabilidades

    Copyright (C) 2007  José Manuel Díez Botella

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

#ifndef CALCESTADO_H
#define CALCESTADO_H

#include <QtGui>
#include "ui_calcestado.h"
#include <qsqlquery.h>


class calcestado : public QDialog {
    Q_OBJECT
	public:
                calcestado();
                QString noejercicio();
                void cargaestado( QString titulo );
                void pasaejercicio1(QString ejercicio);
                void pasaejercicio2(QString ejercicio);
                void no_msj_calculado();

        private:
                Ui::calcestado ui;
                bool comadecimal,decimales;
                bool condesglose,consaldosmedios;
                bool condesglosectas;
                bool hay_analitica_tabla;
                bool msj_calculado;
                QStringList auxiliares;
                QList<double> saldos;
                QStringList auxiliares2;
                QList<double> saldos2;
                // QStringList auxm1,auxm2,auxm3,auxm4,auxm5,auxm6,auxm7,auxm8,auxm9,auxm10,auxm11,auxm12;
                // QList<double> saldosm1,saldosm2,saldosm3,saldosm4,saldosm5,saldosm6,saldosm7,saldosm8,saldosm9,saldosm10,
                //              saldosm11,saldosm12;
                QStringList auxm[12];
                QList<double> saldosm[12];

                bool haymedias();
                bool saldosmediosbloqueados();
                void bloqueasaldosmedios();
                void desbloqueasaldosmedios();
                void calculasaldosmediosperiodo(bool ejercicio1);
                double calculapartado( QString formula, QString ejercicio, bool *calculado, bool estadosmedios,
                                      bool acotacion, QDate fecha1, QDate fecha2, int mes=0 );
                void cargarinfo();
                QSqlQuery consulta_aux_saldo(QDate fechaini, QDate fechafin, QString subcadena);

                double saldo_cuenta_lista_ej1(QString codigo);
                double saldo_cuenta_lista_ej2(QString codigo);
                double saldo_cuenta_lista_mes(QString codigo, int mes);
                double calculacuenta(QString codigo, QString ejercicio, bool acotacion, QDate fecha1, QDate fecha2,
                                     QString const & ciAdd = "");
                double calculaformula( QString codigo, int ejercicio1,bool *calculado, int mes);
                void grabacabeceraestado();
                double calculamediacuenta(QString codigo,bool ejercicio1);
                double cuentaespec( QString extract, QString ejercicio,
                                   bool acotacion, QDate fecha1, QDate fecha2 );
                double calculacuentadiario( QString codigo, QString ejercicio, QString qdiario,
                                          bool acotacion, QDate fecha1, QDate fecha2 );
                void generalatex();
                void generalatex2();
                void generalatex_meses();

                void copia2();
                double cuentaespec_aux( QString extract, QString ejercicio,
                                       bool acotacion, QDate fecha1, QDate fecha2 );
                double calculacuenta_aux(QString codigo, QString ejercicio,
                                         bool acotacion, QDate fecha1, QDate fecha2);
                double calculamediacuenta_aux(QString codigo,bool ejercicio1);
                double calculacuentadiario_aux( QString codigo, QString ejercicio, QString qdiario,
                                              bool acotacion, QDate fecha1, QDate fecha2 );
                double calculacuentaci(QString const & codigo, QString const & ejercicio,
                                                   bool acotacion,
                                                 QDate const & fecha1, QDate const & fecha2);


        private slots:
                void buscapulsado();
                void fijadescripciones();
                void procesareferencias();
                void consulta( void );
                void imprime();
                void copia();
                void generagrafico();
                void actualizacomboej1();
                void actualizacomboej2();
                void fecha_ej1_ini_cambiada();
                void fecha_ej1_fin_cambiada();
                void fecha_ej2_ini_cambiada();
                void fecha_ej2_fin_cambiada();
                void latex();
                void genera_xml();

        public slots:
                void calculaestado();
};




#endif
